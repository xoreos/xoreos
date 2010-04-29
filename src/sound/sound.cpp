/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file sound/sound.cpp
 *  The global sound manager, handling all sound output.
 */

#include "sound/sound.h"
#include "sound/audiostream.h"
#include "sound/decoders/mp3.h"
#include "sound/decoders/vorbis.h"
#include "sound/decoders/wave.h"

#include "common/stream.h"
#include "common/util.h"
#include "common/error.h"

#include "events/events.h"

DECLARE_SINGLETON(Sound::SoundManager)

namespace Sound {

#define BUFFER_SIZE 32768

ChannelHandle::ChannelHandle() : channel(0), id(0) {
}


SoundManager::SoundManager() {
	_ready = false;
}

void SoundManager::init() {
	for (int i = 0; i < kChannelCount; i++)
		_channels[i] = 0;

	_curChannel = 1;
	_curID      = 1;

	_dev = alcOpenDevice(0);
	if (!_dev)
		throw Common::Exception("Could not open OpenAL device");

	_ctx = alcCreateContext(_dev, 0);
	alcMakeContextCurrent(_ctx);
	if (!_ctx)
		throw Common::Exception("Could not create OpenAL context");

	if (!createThread())
		throw Common::Exception("Failed to create sound thread: %s", SDL_GetError());

	_ready = true;
}

void SoundManager::deinit() {
	if (!_ready)
		return;

	if (!destroyThread())
		warning("SoundManager::deinit(): Sound thread had to be killed");

	for (uint16 i = 1; i < kChannelCount; i++)
		freeChannel(i);

	alcMakeContextCurrent(0);
	alcDestroyContext(_ctx);
	alcCloseDevice(_dev);

	_ready = false;
}

bool SoundManager::ready() const {
	return _ready;
}

bool SoundManager::isPlaying(const ChannelHandle &handle) const {
	if ((handle.channel == 0) || (handle.id == 0) || !_channels[handle.channel])
		return false;

	if (_channels[handle.channel]->id != handle.id)
		return false;

	return isPlaying(handle.channel);
}

bool SoundManager::isPlaying(uint16 channel) const {
	if ((channel == 0) || !_channels[channel])
		return false;

	ALint val;
	alGetSourcei(_channels[channel]->source, AL_SOURCE_STATE, &val);

	if (val != AL_PLAYING) {
		if (!_channels[channel]->stream || _channels[channel]->stream->endOfData())
			return false;

		alSourcePlay(_channels[channel]->source);
	}

	return true;
}

AudioStream *SoundManager::makeAudioStream(Common::SeekableReadStream *stream) {
	bool isMP3 = false;
	uint32 tag = stream->readUint32BE();

	if (tag == 0xfff360c4) {
		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		stream = new Common::SeekableSubReadStream(stream, 0x1D6, stream->size(), true);

	} else if (tag == MKID_BE('RIFF')) {
		stream->seek(12);
		tag = stream->readUint32BE();

		if (tag != MKID_BE('fmt '))
			throw Common::Exception("Broken WAVE file");

		// Skip fmt chunk
		stream->skip(stream->readUint32LE());
		tag = stream->readUint32BE();

		if (tag == MKID_BE('fact')) {
			// Skip useless chunk and dummied 'data' header
			stream->skip(stream->readUint32LE());
			tag = stream->readUint32BE();
		}

		if (tag != MKID_BE('data'))
			throw Common::Exception("Found invalid tag in WAVE file: %x", tag);

		uint32 dataSize = stream->readUint32LE();
		if (dataSize == 0) {
			isMP3 = true;
			stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);
		} else
			// Just a regular WAVE
			stream->seek(0);

	} else if ((tag == MKID_BE('BMU ')) && (stream->readUint32BE() == MKID_BE('V1.0'))) {

		// BMU files: MP3 with extra header
		isMP3 = true;
		stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);

	} else if (tag == MKID_BE('OggS')) {

		stream->seek(0);
		return makeVorbisStream(stream, true);

	} else
		throw Common::Exception("Unknown sound format");

	if (isMP3)
		return makeMP3Stream(stream, true);

	return makeWAVStream(stream, true);
}

ChannelHandle SoundManager::playAudioStream(AudioStream *audStream, bool disposeAfterUse) {
	if (!_ready)
		throw Common::Exception("SoundManager not ready");

	if (!audStream)
		throw Common::Exception("No audio stream");

	Common::StackLock lock(_mutex);

	ChannelHandle handle = newChannel();

	_channels[handle.channel] = new Channel;
	Channel &channel = *_channels[handle.channel];

	channel.id              = handle.id;
	channel.stream          = audStream;
	channel.source          = 0;
	channel.disposeAfterUse = disposeAfterUse;

	try {

		if (!channel.stream)
			throw Common::Exception("Could not detect stream type");

		ALenum error = AL_NO_ERROR;

		// Create the source
		alGenSources(1, &channel.source);
		if ((error = alGetError()) != AL_NO_ERROR)
			throw Common::Exception("OpenAL error while generating sources: %X", error);

		// Create all needed buffers
		for (int i = 0; i < NUM_OPENAL_BUFFERS; i++) {
			ALuint buffer;

			alGenBuffers(1, &buffer);
			if ((error = alGetError()) != AL_NO_ERROR)
				throw Common::Exception("OpenAL error while generating buffers: %X", error);

			if (fillBuffer(channel.source, buffer, channel.stream)) {
				// If we could fill the buffer with data, queue it

				alSourceQueueBuffers(channel.source, 1, &buffer);
				if ((error = alGetError()) != AL_NO_ERROR)
					throw Common::Exception("OpenAL error while queueing buffers: %X", error);

			} else
				// If not, put it into our free list
				channel.freeBuffers.push_back(buffer);

			channel.buffers.push_back(buffer);
		}

		// Start playing the sound
		alSourcePlay(channel.source);
		if ((error = alGetError()) != AL_NO_ERROR)
			throw Common::Exception("OpenAL error while attempting to play: %X", error);

	} catch (...) {
		freeChannel(handle);
		throw;
	}

	return handle;
}

ChannelHandle SoundManager::playSoundFile(Common::SeekableReadStream *wavStream, bool loop) {
	if (!_ready)
		throw Common::Exception("SoundManager not ready");

	if (!wavStream)
		throw Common::Exception("No stream");

	AudioStream *audioStream = makeAudioStream(wavStream);

	if (loop) {
		RewindableAudioStream *reAudStream = dynamic_cast<RewindableAudioStream *>(audioStream);
		if (!reAudStream)
			warning("SoundManager::playSoundFile(): The input stream cannot be rewound, this will not loop.");
		else
			audioStream = makeLoopingAudioStream(reAudStream, 0);
	}

	return playAudioStream(audioStream);
}

SoundManager::Channel *SoundManager::getChannel(const ChannelHandle &handle) {
	if ((handle.channel == 0) || (handle.id == 0))
		return 0;

	if (!_channels[handle.channel])
		return 0;

	if (_channels[handle.channel]->id != handle.id)
		return 0;

	return _channels[handle.channel];
}

void SoundManager::setChannelPosition(const ChannelHandle &handle, float x, float y, float z) {
	Common::StackLock lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (channel->stream->isStereo())
		throw Common::Exception("Cannot set position on a stereo sound.");

	alSource3f(channel->source, AL_POSITION, x, y, z);
}

void SoundManager::getChannelPosition(const ChannelHandle &handle, float &x, float &y, float &z) {
	Common::StackLock lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (channel->stream->isStereo())
		throw Common::Exception("Cannot get position on a stereo sound.");

	alGetSource3f(channel->source, AL_POSITION, &x, &y, &z);
}

void SoundManager::stopChannel(ChannelHandle &handle) {
	Common::StackLock lock(_mutex);

	freeChannel(handle);
}

void SoundManager::triggerUpdate() {
	_needUpdate.signal();
}

bool SoundManager::fillBuffer(ALuint source, ALuint alBuffer, AudioStream *stream) {
	if (!stream)
		throw Common::Exception("No stream");

	if (stream->endOfData())
		return false;

	// Read in the required amount of samples
	uint32 numSamples = BUFFER_SIZE / 2;

	if (stream->isStereo())
		numSamples /= 2;

	byte *buffer = new byte[BUFFER_SIZE];
	memset(buffer, 0, BUFFER_SIZE);
	numSamples = stream->readBuffer((int16 *)buffer, numSamples);

	uint32 bufferSize = numSamples * 2;

	alBufferData(alBuffer, stream->isStereo() ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, buffer, bufferSize, stream->getRate());

	if (alGetError() != AL_NO_ERROR)
		throw Common::Exception("OpenAL error while filling buffer");

	return true;
}

void SoundManager::bufferData(uint16 channel) {
	if ((channel == 0) || !_channels[channel])
		return;

	bufferData(*_channels[channel]);
}

void SoundManager::bufferData(Channel &channel) {
	if (!channel.stream || channel.stream->endOfData())
		return;

	// Get the number of buffers that have been processed
	ALint buffersProcessed;
	alGetSourcei(channel.source, AL_BUFFERS_PROCESSED, &buffersProcessed);

	// Pull all processed buffers from the queue and put them into our free list
	while (buffersProcessed--) {
		ALuint alBuffer;

		alSourceUnqueueBuffers(channel.source, 1, &alBuffer);

		channel.freeBuffers.push_back(alBuffer);
	}

	// Buffer as long as we still have data and free buffers
	std::list<ALuint>::iterator buffer = channel.freeBuffers.begin();
	while (buffer != channel.freeBuffers.end()) {
		if (!fillBuffer(channel.source, *buffer, channel.stream))
			break;

		alSourceQueueBuffers(channel.source, 1, &*buffer);

		buffer = channel.freeBuffers.erase(buffer);
	}
}

void SoundManager::update() {
	Common::StackLock lock(_mutex);

	for (int i = 1; i < kChannelCount; i++) {
		if (!_channels[i])
			continue;

		// Free the channel if it is no longer playing
		if (!isPlaying(i)) {
			freeChannel(i);
			continue;
		}

		// Try to buffer some more data
		bufferData(i);
	}
}

ChannelHandle SoundManager::newChannel() {
	uint16 foundChannel = 0;

	for (int i = 0; (i < kChannelCount) && !foundChannel; i++) {
		if (!_channels[i])
			foundChannel = i;

		// Channel 0 is reserved for "invalid channel"
		_curChannel = (_curChannel >= kChannelCount) ? 1 : (_curChannel + 1);
	}

	if (!foundChannel)
		throw Common::Exception("All sound channels occupied");

	ChannelHandle handle;

	handle.channel = foundChannel;
	handle.id      = _curID++;

	// ID 0 is reserved for "invalid ID"
	if (_curID == 0)
		_curID++;

	return handle;
}

void SoundManager::freeChannel(ChannelHandle &handle) {
	if ((handle.channel != 0) && (handle.id != 0) && _channels[handle.channel])
		// Only free if there is a channel to free
		if (handle.id == _channels[handle.channel]->id)
			// Only free if the IDs match
			freeChannel(handle.channel);

	handle.channel = 0;
	handle.id      = 0;
}

void SoundManager::freeChannel(uint16 channel) {
	if (channel == 0)
		return;

	Channel *c = _channels[channel];
	if (!c)
		return;

	if (c->disposeAfterUse)
		delete c->stream;

	if (c->source)
		alDeleteSources(1, &c->source);

	for (std::list<ALuint>::iterator buffer = c->buffers.begin(); buffer != c->buffers.end(); ++buffer)
		alDeleteBuffers(1, &*buffer);

	delete c;
	_channels[channel] = 0;
}

void SoundManager::threadMethod() {
	while (!_killThread) {
		update();
		_needUpdate.wait(100);
	}
}

} // End of namespace Sound

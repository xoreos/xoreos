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

#include "common/stream.h"
#include "common/util.h"
#include "common/error.h"

#include "graphics/graphics.h"

#include "events/events.h"

DECLARE_SINGLETON(Sound::SoundManager)

namespace Sound {

#define NUM_CHANNELS 2
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096

SoundManager::SoundManager() {
	_ready = false;
}

void SoundManager::init() {
	if (!GfxMan.ready())
		throw Common::Exception("The GraphicsManager needs to be initialized first");

	_dev = alcOpenDevice(NULL);
	if (!_dev)
		throw Common::Exception("Could not open OpenAL device");

	_ctx = alcCreateContext(_dev, NULL);
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

	for (uint32 i = 0; i < _channels.size(); i++)
		if (_channels[i])
			freeChannel(i);

	_channels.clear();

	alcMakeContextCurrent(NULL);
	alcDestroyContext(_ctx);
	alcCloseDevice(_dev);

	_ready = false;
}

bool SoundManager::ready() const {
	return _ready;
}

bool SoundManager::isPlaying(uint32 channel) const {
	if (channel >= _channels.size() || !_channels[channel])
		return false;

	ALint val;
	alGetSourcei(_channels[channel]->source, AL_SOURCE_STATE, &val);

	return val == AL_PLAYING;
}

AudioStream *SoundManager::makeAudioStream(Common::SeekableReadStream *stream) {
	bool isMP3 = false;
	uint32 tag = stream->readUint32BE();

	if (tag == 0xfff360c4) {
		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		stream = new Common::SeekableSubReadStream(stream, 0x1D6, stream->size(), DisposeAfterUse::YES);
	} else if (tag == MKID_BE('RIFF')) {
		stream->seek(12);
		tag = stream->readUint32BE();
		if (tag != MKID_BE('fmt ')) {
			warning("SoundManager::makeAudioStream(): Broken WAVE file");
			return 0;
		}

		// Skip fmt chunk
		stream->skip(stream->readUint32LE());
		tag = stream->readUint32BE();

		if (tag == MKID_BE('fact')) {
			// Skip useless chunk and dummied 'data' header
			stream->skip(stream->readUint32LE());
			tag = stream->readUint32BE();
		}

		if (tag != MKID_BE('data')) {
			warning("SoundManager::makeAudioStream(): Found invalid tag in WAVE file: %x", tag);
			return 0;
		}

		uint32 dataSize = stream->readUint32LE();
		if (dataSize == 0) {
			isMP3 = true;
			stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), DisposeAfterUse::YES);
		} else // Just a regular WAVE
			stream->seek(0);
	} else if ((tag == MKID_BE('BMU ')) && (stream->readUint32BE() == MKID_BE('V1.0'))) {
		// BMU files: MP3 with extra header
		isMP3 = true;
		stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), DisposeAfterUse::YES);
	} else if (tag == MKID_BE('OggS')) {
		return makeVorbisStream(stream, DisposeAfterUse::YES);
	} else {
		warning("Unknown sound format.");
		return 0;
	}

	if (isMP3)
		return makeMP3Stream(stream, DisposeAfterUse::YES);

	// TODO: WAVE/OGG
	warning("TODO: WAVE/Ogg");
	delete stream;

	return 0;
}

int SoundManager::playSoundFile(Common::SeekableReadStream *wavStream) {
	if (!_ready)
		return -1;

	if (!wavStream) {
		warning("SoundManager::playSoundFile(): No stream");
		return -1;
	}

	Common::StackLock lock(_mutex);

	Channel *channel = new Channel;
	channel->stream = makeAudioStream(wavStream);

	if (!channel->stream) {
		warning("SoundManager::playSoundFile(): Could not detect stream type");
		return -1;
	}

	// For now, just ignore any streams until clone2727 gets off his ass
	// and finishes AudioStream support.
	delete channel->stream;
	delete channel;
	return -1;

#if 0
	// This code won't be used in future iterations of the SoundManager, but it is
	// particularly useful for reference, etc.
	channel->numBuffers = sound->buffer_size / BUFFER_SIZE;

	if (channel->numBuffers % BUFFER_SIZE)
		channel->numBuffers++;

	channel->buffers = new ALuint[channel->numBuffers];

	alGenSources(1, &channel->source);
	alGenBuffers(channel->numBuffers, channel->buffers);

	if (alGetError() != AL_NO_ERROR)
		throw Common::Exception("AL Error of some sort");

	for (uint32 i = 0; i < channel->numBuffers; i++) {
		if (i == channel->numBuffers - 1 && sound->buffer_size % BUFFER_SIZE != 0)
			alBufferData(channel->buffers[i], AL_FORMAT_STEREO16, (byte *)sound->buffer + BUFFER_SIZE * i, sound->buffer_size % BUFFER_SIZE, SAMPLE_RATE);
		else
			alBufferData(channel->buffers[i], AL_FORMAT_STEREO16, (byte *)sound->buffer + BUFFER_SIZE * i, BUFFER_SIZE, SAMPLE_RATE);
	}

	alSourceQueueBuffers(channel->source, channel->numBuffers, channel->buffers);
	alSourcePlay(channel->source);
#endif

	for (uint32 i = 0; i < _channels.size(); i++) {
		if (_channels[i] == 0) {
			_channels[i] = channel;
			return i;
		}
	}

	_channels.push_back(channel);
	return _channels.size() - 1;
}

void SoundManager::update() {
	Common::StackLock lock(_mutex);

	for (uint i = 0; i < _channels.size(); i++) {
		if (!_channels[i])
			continue;

		if (!isPlaying(i))
			freeChannel(i);
	}
}

void SoundManager::freeChannel(uint32 channel) {
	if (channel >= _channels.size() || !_channels[channel])
		return;

	Channel *c = _channels[channel];

	alDeleteSources(1, &c->source);
	alDeleteBuffers(c->numBuffers, c->buffers);

	delete c;
	_channels[channel] = 0;
}

void SoundManager::threadMethod() {
	while (!_killThread) {
		update();
		EventMan.delay(100);
	}
}

} // End of namespace Sound

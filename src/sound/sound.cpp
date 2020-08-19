/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The global sound manager, handling all sound output.
 */

#include <cassert>
#include <cstring>

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/readstream.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/configman.h"
#include "src/common/debug.h"

#include "src/sound/sound.h"
#include "src/sound/audiostream.h"
#include "src/sound/decoders/asf.h"
#ifdef ENABLE_MAD
#include "src/sound/decoders/mp3.h"
#endif
#ifdef ENABLE_VORBIS
#include "src/sound/decoders/vorbis.h"
#endif
#include "src/sound/decoders/wave.h"

#include "src/events/events.h"

DECLARE_SINGLETON(Sound::SoundManager)

/** Control how many buffers per sound OpenAL will create.
 *
 *  @note clone2727 says: 5 is just a safe number. Mine only reached a max of 2.
 */
static const size_t kOpenALBufferCount = 5;

/** Number of bytes per OpenAL buffer.
 *
 *  @note Needs to be high enough to prevent stuttering, but low enough to
 *        prevent a noticeable lag. 32768 seems to work just fine.
 */
static const size_t kOpenALBufferSize = 32768;

namespace Sound {

SoundManager::Channel::Channel(uint32_t i, size_t idx, SoundType t,
                               const TypeList::iterator &ti, AudioStream *s, bool d) :
	id(i), index(idx), state(AL_PAUSED), stream(s, d), source(0),
	type(t), typeIt(ti), finishedBuffers(0), gain(1.0f) {

}


SoundManager::SoundManager() : _ready(false), _hasSound(false), _hasMultiChannel(false), _format51(0) {
}

SoundManager::~SoundManager() {
}

void SoundManager::init() {
	for (size_t i = 0; i < kSoundTypeMAX; i++)
		_types[i].gain = 1.0f;

	_curID = 1;

	_ctx = 0;

	_hasSound = false;

	_hasMultiChannel = false;
	_format51        = 0;

	try {
		_dev = alcOpenDevice(0);
		if (!_dev)
			throw Common::Exception("Could not open OpenAL device");

		_ctx = alcCreateContext(_dev, 0);
		if (!_ctx)
			throw Common::Exception("Could not create OpenAL context: 0x%X", (uint) alGetError());

		alcMakeContextCurrent(_ctx);

		ALenum error = alGetError();
		if (error != AL_NO_ERROR)
			throw Common::Exception("Could not use OpenAL context: 0x%X", (uint) alGetError());

		_hasMultiChannel = alIsExtensionPresent("AL_EXT_MCFORMATS") != 0;
		_format51        = alGetEnumValue("AL_FORMAT_51CHN16");

		if (!createThread("SoundManager"))
			throw Common::Exception("Failed to create sound thread: %s", SDL_GetError());

		_hasSound = true;

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed to initialize OpenAL. Disabling sound output!");
	}

	_ready = true;

	if (!_hasSound)
		return;

	setListenerGain(ConfigMan.getDouble("volume", 1.0));

	setTypeGain(kSoundTypeMusic, ConfigMan.getDouble("volume_music", 1.0));
	setTypeGain(kSoundTypeSFX  , ConfigMan.getDouble("volume_sfx"  , 1.0));
	setTypeGain(kSoundTypeVoice, ConfigMan.getDouble("volume_voice", 1.0));
	setTypeGain(kSoundTypeVideo, ConfigMan.getDouble("volume_video", 1.0));

	alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED);
}

void SoundManager::deinit() {
	if (!_ready)
		return;

	destroyThread();

	for (size_t i = 0; i < kChannelCount; i++)
		freeChannel(i);

	if (_hasSound) {
		alcMakeContextCurrent(0);
		alcDestroyContext(_ctx);
		alcCloseDevice(_dev);
	}

	_ready = false;
}

bool SoundManager::ready() const {
	return _ready;
}

void SoundManager::triggerUpdate() {
	checkReady();

	_needUpdate.notify_one();
}

bool SoundManager::isValidChannel(const ChannelHandle &handle) const {
	if ((handle.channel >= kChannelCount) || (handle.id == 0) || !_channels[handle.channel])
		return false;

	if (_channels[handle.channel]->id != handle.id)
		return false;

	return true;
}

bool SoundManager::isPlaying(const ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if ((handle.channel >= kChannelCount) || (handle.id == 0) || !_channels[handle.channel])
		return false;

	if (_channels[handle.channel]->id != handle.id)
		return false;

	return isPlaying(handle.channel);
}

bool SoundManager::isPlaying(size_t channel) const {
	if ((channel >= kChannelCount) || !_channels[channel])
		return false;

	// TODO: This might pose a problem should we ever need to wait
	//       for sounds to finish (for syncing, ...). We need to
	//       add a way for audio streams to tell us how long they are
	//       and then check if that time has elapsed.
	if (!_hasSound)
		return true;

	ALenum error = AL_NO_ERROR;

	ALint val;
	alGetSourcei(_channels[channel]->source, AL_SOURCE_STATE, &val);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Common::Exception("OpenAL error while getting source state in %s: 0x%X",
		                        formatChannel(_channels[channel].get()).c_str(), error);

	if (val != AL_PLAYING) {
		if (!_channels[channel]->stream || _channels[channel]->stream->endOfStream()) {
			ALint buffersQueued;
			alGetSourcei(_channels[channel]->source, AL_BUFFERS_QUEUED, &buffersQueued);
			if ((error = alGetError()) != AL_NO_ERROR)
				throw Common::Exception("OpenAL error while getting queued buffers in %s: 0x%X",
				                        formatChannel(_channels[channel].get()).c_str(), error);

			ALint buffersProcessed;
			alGetSourcei(_channels[channel]->source, AL_BUFFERS_PROCESSED, &buffersProcessed);
			if ((error = alGetError()) != AL_NO_ERROR)
				throw Common::Exception("OpenAL error while getting processed buffers in %s: 0x%X",
				                        formatChannel(_channels[channel].get()).c_str(), error);

			if (buffersQueued == buffersProcessed)
				return false;
		}

		if (_channels[channel]->state != AL_PLAYING)
			return true;

		alSourcePlay(_channels[channel]->source);
	}

	return true;
}

bool SoundManager::isPaused(const ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if ((handle.channel == 0) || (handle.id == 0) || !_channels[handle.channel])
		return false;

	if (_channels[handle.channel]->id != handle.id)
		return false;

	return _channels[handle.channel]->state == AL_PAUSED;
}

AudioStream *SoundManager::makeAudioStream(Common::SeekableReadStream *stream) {
	bool isMP3 = false;
	uint32_t tag = stream->readUint32BE();

	if (tag == 0xfff360c4) {
		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		stream = new Common::SeekableSubReadStream(stream, 0x1D6, stream->size(), true);

	} else if (tag == MKTAG('R', 'I', 'F', 'F')) {
		stream->seek(12);
		tag = stream->readUint32BE();

		if (tag != MKTAG('f', 'm', 't', ' '))
			throw Common::Exception("Broken WAVE file (%s)", Common::debugTag(tag).c_str());

		// Skip fmt chunk
		stream->skip(stream->readUint32LE());
		tag = stream->readUint32BE();

		while ((tag == MKTAG('f', 'a', 'c', 't')) || (tag == MKTAG('P', 'A', 'D', ' ')) ||
		       (tag == MKTAG('c', 'u', 'e', ' ')) || (tag == MKTAG('L', 'I', 'S', 'T')) ||
		       (tag == MKTAG('s', 'm', 'p', 'l'))) {
			// Skip useless chunks
			stream->skip(stream->readUint32LE());
			tag = stream->readUint32BE();
		}

		if (tag != MKTAG('d', 'a', 't', 'a'))
			throw Common::Exception("Found invalid tag in WAVE file: %s", Common::debugTag(tag).c_str());

		uint32_t dataSize = stream->readUint32LE();
		if (dataSize == 0) {
			isMP3 = true;
			stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);
		} else
			// Just a regular WAVE
			stream->seek(0);

	} else if ((tag                    == MKTAG('B', 'M', 'U', ' ')) &&
	           (stream->readUint32BE() == MKTAG('V', '1', '.', '0'))) {

		// BMU files: MP3 with extra header
		isMP3 = true;
		stream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->size(), true);

	} else if (tag == MKTAG('O', 'g', 'g', 'S')) {

#ifdef ENABLE_VORBIS
		stream->seek(0);
		return makeVorbisStream(stream, true);
#else
		throw Common::Exception("Vorbis decoding disabled when building without libvorbis");
#endif

	} else if (tag == 0x3026B275) {

		// ASF (most probably with WMAv2)
		stream->seek(0);
		return makeASFStream(stream, true);

	} else if (((tag & 0xFFFFFF00) | 0x20) == MKTAG('I', 'D', '3', ' ')) {

		// ID3v2 tag found => Should be MP3.
		stream->seek(0);
		isMP3 = true;

	} else if ((tag & 0xFFFA0000) == 0xFFFA0000) {

		// MPEG sync + MPEG1 layer 3 bits found => Should be MP3.
		// NOTE: To decrease the chances of false positives, we could look at more than just the first frame.
		stream->seek(0);
		isMP3 = true;

	} else
		throw Common::Exception("Unknown sound format %s", Common::debugTag(tag).c_str());

	if (isMP3)
#ifdef ENABLE_MAD
		return makeMP3Stream(stream, true);
#else
		throw Common::Exception("MP3 decoding disabled when building without libmad");
#endif

	return makeWAVStream(stream, true);
}

ChannelHandle SoundManager::playAudioStream(AudioStream *audStream, SoundType type, bool disposeAfterUse) {
	assert((type >= 0) && (type < kSoundTypeMAX));

	checkReady();

	if (!audStream)
		throw Common::Exception("No audio stream");

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	ChannelHandle handle = newChannel();

	bool success = false;
	BOOST_SCOPE_EXIT ( (&success) (&handle) (this_) ) {
		if (!success)
			this_->freeChannel(handle);
	} BOOST_SCOPE_EXIT_END

	const TypeList::iterator typeEndIt = _types[type].list.end();

	_channels[handle.channel] = std::make_unique<Channel>(handle.id, handle.channel, type, typeEndIt, audStream, disposeAfterUse);
	Channel &channel = *_channels[handle.channel];

	if (!channel.stream)
		throw Common::Exception("Could not detect stream type");

	ALenum error = AL_NO_ERROR;

	if (_hasSound) {
		// Create the source
		alGenSources(1, &channel.source);
		if ((error = alGetError()) != AL_NO_ERROR)
			throw Common::Exception("OpenAL error while generating sources: 0x%X", error);

		// Create all needed buffers
		for (size_t i = 0; i < kOpenALBufferCount; i++) {
			ALuint buffer;

			alGenBuffers(1, &buffer);
			if ((error = alGetError()) != AL_NO_ERROR)
				throw Common::Exception("OpenAL error while generating buffers: 0x%X", error);

			if (fillBuffer(channel, buffer, channel.stream.get(), channel.bufferSize[buffer])) {
				// If we could fill the buffer with data, queue it

				alSourceQueueBuffers(channel.source, 1, &buffer);
				if ((error = alGetError()) != AL_NO_ERROR)
					throw Common::Exception("OpenAL error while queueing buffers: 0x%X", error);

			} else
				// If not, put it into our free list
				channel.freeBuffers.push_back(buffer);

			channel.buffers.push_back(buffer);
		}

		// Set the gain to the current sound type gain
		alSourcef(channel.source, AL_GAIN, _types[channel.type].gain);
		// Set the sound per default as relative.
		alSourcei(channel.source, AL_SOURCE_RELATIVE, AL_TRUE);
	}

	// Add the channel to the correct type list
	_types[channel.type].list.push_back(&channel);
	channel.typeIt = --_types[channel.type].list.end();

	debugC(Common::kDebugSound, 2, "Created sound channel %s", formatChannel(handle).c_str());

	success = true;
	return handle;
}

ChannelHandle SoundManager::playSoundFile(Common::SeekableReadStream *wavStream, SoundType type, bool loop) {
	checkReady();

	if (!wavStream)
		throw Common::Exception("No stream");

	AudioStream *audioStream = makeAudioStream(wavStream);

	if (!audioStream)
		throw Common::Exception("No audio stream");

	if (loop) {
		RewindableAudioStream *reAudStream = dynamic_cast<RewindableAudioStream *>(audioStream);
		if (!reAudStream)
			warning("SoundManager::playSoundFile(): The input stream cannot be rewound, this will not loop.");
		else
			audioStream = makeLoopingAudioStream(reAudStream, 0);
	}

	return playAudioStream(audioStream, type);
}

const SoundManager::Channel *SoundManager::getChannel(const ChannelHandle &handle) const {
	if ((handle.channel >= kChannelCount) || (handle.id == 0))
		return 0;

	if (!_channels[handle.channel])
		return 0;

	if (_channels[handle.channel]->id != handle.id)
		return 0;

	return _channels[handle.channel].get();
}

SoundManager::Channel *SoundManager::getChannel(const ChannelHandle &handle) {
	if ((handle.channel >= kChannelCount) || (handle.id == 0))
		return 0;

	if (!_channels[handle.channel])
		return 0;

	if (_channels[handle.channel]->id != handle.id)
		return 0;

	return _channels[handle.channel].get();
}

void SoundManager::startChannel(ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	channel->state = AL_PLAYING;

	debugC(Common::kDebugSound, 1, "Start sound channel %s", formatChannel(handle).c_str());

	triggerUpdate();
}

void SoundManager::pauseChannel(ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	pauseChannel(channel);
}

void SoundManager::pauseChannel(ChannelHandle &handle, bool pause) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	debugC(Common::kDebugSound, 1, "%s sound channel %s", pause ? "Pause" : "Unpause",
	       formatChannel(handle).c_str());

	pauseChannel(channel, pause);
}

void SoundManager::stopChannel(ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (isValidChannel(handle))
		debugC(Common::kDebugSound, 1, "Stop sound channel %s", formatChannel(handle).c_str());

	freeChannel(handle);
}

void SoundManager::pauseAll(bool pause) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (size_t i = 0; i < kChannelCount; i++)
		pauseChannel(_channels[i].get(), pause);
}

void SoundManager::stopAll() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	for (size_t i = 0; i < kChannelCount; i++)
		freeChannel(i);
}

void SoundManager::setListenerGain(float gain) {
	checkReady();

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if (_hasSound)
		alListenerf(AL_GAIN, gain);
}

void SoundManager::setListenerPosition(float x, float y, float z) {
	checkReady();

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	alListener3f(AL_POSITION, x, y, z);
}

void SoundManager::setListenerOrientation(float dirX, float dirY, float dirZ, float upX, float upY, float upZ) {
	checkReady();

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	float orientation[] = {dirX, dirY, dirZ, upX, upY, upZ};
	alListenerfv(AL_ORIENTATION, orientation);
}

void SoundManager::setChannelPosition(const ChannelHandle &handle, float x, float y, float z) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (channel->stream->getChannels() > 1)
		throw Common::Exception("Cannot set position of a non-mono sound in %s",
		                        formatChannel(handle).c_str());

	if (_hasSound)
		alSource3f(channel->source, AL_POSITION, x, y, z);
}

void SoundManager::getChannelPosition(const ChannelHandle &handle, float &x, float &y, float &z) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (channel->stream->getChannels() > 1)
		throw Common::Exception("Cannot get position of a non-mono sound in %s",
		                        formatChannel(handle).c_str());

	if (_hasSound)
		alGetSource3f(channel->source, AL_POSITION, &x, &y, &z);
}

void SoundManager::setChannelGain(const ChannelHandle &handle, float gain) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	channel->gain = gain;

	if (_hasSound)
		alSourcef(channel->source, AL_GAIN, _types[channel->type].gain * gain);
}

void SoundManager::setChannelPitch(const ChannelHandle &handle, float pitch) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (_hasSound)
		alSourcef(channel->source, AL_PITCH, pitch);
}

void SoundManager::setChannelRelative(const ChannelHandle &handle, bool relative) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (_hasSound)
		alSourcei(channel->source, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE);
}

void SoundManager::setChannelDistance(const ChannelHandle &handle, float minDistance, float maxDistance) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		throw Common::Exception("Invalid channel");

	if (_hasSound) {
		alSourcef(channel->source, AL_REFERENCE_DISTANCE, minDistance);
		alSourcef(channel->source, AL_MAX_DISTANCE, maxDistance);
	}
}

uint64_t SoundManager::getChannelSamplesPlayed(const ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		return 0;

	// Update the queued/unqueued buffers to make sure the channel is up-to-date
	bufferData(*channel);

	// The position within the currently playing buffer
	ALint currentPosition;
	alGetSourcei(channel->source, AL_BYTE_OFFSET, &currentPosition);

	// Total number of bytes processed
	uint64_t byteCount = channel->finishedBuffers + currentPosition;

	// Number of 16bit samples per channel
	return byteCount / channel->stream->getChannels() / 2;
}

uint64_t SoundManager::getChannelDurationPlayed(const ChannelHandle &handle) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Channel *channel = getChannel(handle);
	if (!channel || !channel->stream)
		return 0;

	return (getChannelSamplesPlayed(handle) * 1000) / channel->stream->getRate();
}

void SoundManager::setTypeGain(SoundType type, float gain) {
	assert((type >= 0) && (type < kSoundTypeMAX));

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	// Set the new type gain
	_types[type].gain = gain;

	// Update all currently playing channels of that type
	for (TypeList::iterator t = _types[type].list.begin(); t != _types[type].list.end(); ++t) {
		assert(*t);

		if (_hasSound)
			alSourcef((*t)->source, AL_GAIN, (*t)->gain * gain);
	}
}

bool SoundManager::fillBuffer(const Channel &channel, ALuint alBuffer,
                              AudioStream *stream, ALsizei &bufferedSize) const {

	bufferedSize = 0;

	if (!stream)
		throw Common::Exception("No stream in %s", formatChannel(&channel).c_str());

	if (!_hasSound)
		return true;

	if (stream->endOfData())
		return false;

	ALenum format;

	const int channelCount = stream->getChannels();
	if        (channelCount == 1) {
		format = AL_FORMAT_MONO16;
	} else if (channelCount == 2) {
		format = AL_FORMAT_STEREO16;
	} else if (channelCount == 6) {
		if (!_hasMultiChannel) {
			warning("SoundManager::fillBuffer(): TODO: !_hasMultiChannel in %s",
			        formatChannel(&channel).c_str());
			return false;
		}

		format = _format51;

	} else {
		warning("SoundManager::fillBuffer(): Unsupported channel count in %s: %d",
		        formatChannel(&channel).c_str(), channelCount);
		return false;
	}

	// Read in the required amount of samples
	size_t numSamples = kOpenALBufferSize / 2;

	std::unique_ptr<byte[]> buffer = std::make_unique<byte[]>(kOpenALBufferSize);
	std::memset(buffer.get(), 0, kOpenALBufferSize);

	numSamples = stream->readBuffer(reinterpret_cast<int16_t *>(buffer.get()), numSamples);
	if (numSamples == AudioStream::kSizeInvalid) {
		warning("Failed reading from stream while filling buffer in %s", formatChannel(&channel).c_str());
		return false;
	}

	bufferedSize = numSamples * 2;
	alBufferData(alBuffer, format, buffer.get(), bufferedSize, stream->getRate());

	ALenum error = alGetError();
	if (error != AL_NO_ERROR) {
		warning("OpenAL error while filling buffer in %s: 0x%X", formatChannel(&channel).c_str(), error);
		return false;
	}

	return true;
}

void SoundManager::bufferData(size_t channel) {
	if ((channel >= kChannelCount) || !_channels[channel])
		return;

	bufferData(*_channels[channel]);
}

void SoundManager::bufferData(Channel &channel) {
	if (!channel.stream)
		return;

	if (!_hasSound)
		return;

	ALenum error = AL_NO_ERROR;

	// Get the number of buffers that have been processed
	ALint buffersProcessed = -1;
	alGetSourcei(channel.source, AL_BUFFERS_PROCESSED, &buffersProcessed);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Common::Exception("OpenAL error while getting processed buffers in %s: 0x%X",
		                        formatChannel(&channel).c_str(), error);

	assert(buffersProcessed >= 0);

	if ((size_t)buffersProcessed > kOpenALBufferCount)
		throw Common::Exception("Got more processed buffers than total source buffers in %s?!?",
		                        formatChannel(&channel).c_str());

	// Unqueue the processed buffers
	ALuint freeBuffers[kOpenALBufferCount];
	alSourceUnqueueBuffers(channel.source, buffersProcessed, freeBuffers);
	if ((error = alGetError()) != AL_NO_ERROR)
		throw Common::Exception("OpenAL error while unqueueing buffers in %s: 0x%X",
		                        formatChannel(&channel).c_str(), error);

	// Put them into the free buffers list
	for (size_t i = 0; i < (size_t)buffersProcessed; i++) {
		channel.freeBuffers.push_back(freeBuffers[i]);

		channel.finishedBuffers += channel.bufferSize[freeBuffers[i]];
	}

	// Buffer as long as we still have data and free buffers
	std::list<ALuint>::iterator buffer = channel.freeBuffers.begin();
	while (buffer != channel.freeBuffers.end()) {
		if (!fillBuffer(channel, *buffer, channel.stream.get(), channel.bufferSize[*buffer]))
			break;

		alSourceQueueBuffers(channel.source, 1, &*buffer);
		if ((error = alGetError()) != AL_NO_ERROR)
			throw Common::Exception("OpenAL error while queueing buffers in %s: 0x%X",
			                        formatChannel(&channel).c_str(), error);

		buffer = channel.freeBuffers.erase(buffer);
	}
}

void SoundManager::checkReady() {
	if (!_ready)
		throw Common::Exception("SoundManager not ready");
}

void SoundManager::update() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	size_t channelCount = 0;
	for (size_t i = 0; i < kChannelCount; i++) {
		if (!_channels[i])
			continue;

		channelCount++;

		// Free the channel if it is no longer playing
		if (!isPlaying(i)) {
			freeChannel(i);
			continue;
		}

		// Try to buffer some more data
		bufferData(i);
	}

	debugC(Common::kDebugSound, 9, "Active sound channel: %s", Common::composeString(channelCount).c_str());
}

ChannelHandle SoundManager::newChannel() {
	size_t foundChannel = kChannelInvalid;

	for (size_t i = 0; (i < kChannelCount) && (foundChannel == kChannelInvalid); i++)
		if (!_channels[i])
			foundChannel = i;

	if (foundChannel == kChannelInvalid)
		throw Common::Exception("All sound channels occupied");

	ChannelHandle handle;

	handle.channel = foundChannel;
	handle.id      = _curID++;

	// ID 0 is reserved for "invalid ID"
	if (_curID == 0)
		_curID++;

	return handle;
}

void SoundManager::pauseChannel(Channel *channel, bool pause) {
	if (!channel || channel->id == 0)
		return;

	ALenum error = AL_NO_ERROR;
	if (pause) {
		if (_hasSound) {
			alSourcePause(channel->source);
			if ((error = alGetError()) != AL_NO_ERROR)
				warning("OpenAL error while attempting to pause channel %s: 0x%X",
				        formatChannel(channel).c_str(), error);
		}

		channel->state = AL_PAUSED;
	} else
		channel->state = AL_PLAYING;

	triggerUpdate();
}

void SoundManager::pauseChannel(Channel *channel) {
	if (!channel)
		return;

	if      (channel->state == AL_PAUSED)
		pauseChannel(channel, false);
	else if (channel->state == AL_PLAYING)
		pauseChannel(channel, true);
}

void SoundManager::freeChannel(ChannelHandle &handle) {
	if ((handle.channel < kChannelCount) && (handle.id != 0) && _channels[handle.channel])
		// Only free if there is a channel to free
		if (handle.id == _channels[handle.channel]->id)
			// Only free if the IDs match
			freeChannel(handle.channel);

	handle.channel = kChannelInvalid;
	handle.id      = 0;
}

void SoundManager::freeChannel(size_t channel) {
	if (channel >= kChannelCount)
		return;

	Channel *c = _channels[channel].get();
	if (!c)
		// Nothing to do
		return;

	// Discard the stream
	c->stream.reset();

	if (_hasSound) {
		// Delete the channel's OpenAL source
		if (c->source)
			alDeleteSources(1, &c->source);

		// Delete the OpenAL buffers
		for (std::list<ALuint>::iterator buffer = c->buffers.begin(); buffer != c->buffers.end(); ++buffer)
			alDeleteBuffers(1, &*buffer);
	}

	// Remove the channel from the type list
	if (c->typeIt != _types[c->type].list.end())
		_types[c->type].list.erase(c->typeIt);

	// And finally delete the channel itself
	_channels[channel].reset();
}

void SoundManager::threadMethod() {
	while (!_killThread.load(std::memory_order_relaxed)) {
		update();
		std::unique_lock<std::recursive_mutex> lock(_needUpdateMutex);
		_needUpdate.wait_for(lock, std::chrono::duration<int, std::milli>(100));
	}
}

Common::UString SoundManager::formatChannel(const Channel *channel) const {
	if (!channel)
		return "[0:0]";

	return "[" + Common::composeString(channel->index) + ":" + Common::composeString(channel->id) + "]";
}

Common::UString SoundManager::formatChannel(const ChannelHandle &handle) const {
	return formatChannel(getChannel(handle));
}

} // End of namespace Sound

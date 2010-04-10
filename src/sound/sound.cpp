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

	Sound_Init();

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

	Sound_Quit();

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

static int RWStreamSeek(SDL_RWops *context, int offset, int whence) {
	if (!context || (context->type != 0xc0ffeeee))
		return -1;

	Common::SeekableReadStream *stream = (Common::SeekableReadStream *) context->hidden.unknown.data1;
	if (!stream)
		return -1;

	if (!stream->seek(offset, whence))
		return -1;

	return stream->pos();
}

static int RWStreamRead(SDL_RWops *context, void *ptr, int size, int maxnum) {
	if (!context || (context->type != 0xc0ffeeee))
		return 0;

	Common::SeekableReadStream *stream = (Common::SeekableReadStream *) context->hidden.unknown.data1;
	if (!stream)
		return 0;

	int n = stream->read(ptr, size * maxnum);

	return n / size;
}

static int RWStreamWrite(SDL_RWops *context, const void *ptr, int size, int num) {
	return 0;
}

static int RWStreamClose(SDL_RWops *context) {
	if (!context || (context->type != 0xc0ffeeee))
		return -1;

	Common::SeekableReadStream *stream = (Common::SeekableReadStream *) context->hidden.unknown.data1;
	delete stream;

	SDL_FreeRW(context);
	return 0;
}

SDL_RWops *RW_FromStream(Common::SeekableReadStream *stream) {
	if (!stream)
		return 0;

	SDL_RWops *rw = SDL_AllocRW();
	if (!rw)
		return 0;

	rw->seek  = RWStreamSeek;
	rw->read  = RWStreamRead;
	rw->write = RWStreamWrite;
	rw->close = RWStreamClose;

	rw->type = 0xc0ffeeee;
	rw->hidden.unknown.data1 = stream;

	return rw;
}

void FreeRW_FromStream(SDL_RWops *rw) {
	if (!rw || (rw->type != 0xc0ffeeee))
		return;

	rw->close(rw);
}

int SoundManager::playSoundFile(Common::SeekableReadStream *wavStream) {
	if (!_ready)
		return -1;

	if (!wavStream) {
		warning("SoundManager::playSoundFile(): No stream");
		return -1;
	}

	Common::StackLock lock(_mutex);

	bool isMP3 = false;
	uint32 tag = wavStream->readUint32BE();
	if (tag == 0xfff360c4) {

		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		wavStream->seek(0x1D6);

	} else if (tag == MKID_BE('RIFF')) {

		wavStream->seek(12);
		tag = wavStream->readUint32BE();
		if (tag != MKID_BE('fmt ')) {
			warning("SoundManager::playSoundFile(): Broken WAVE file");
			return -1;
		}

		// Skip fmt chunk
		wavStream->skip(wavStream->readUint32LE());
		tag = wavStream->readUint32BE();

		if (tag == MKID_BE('fact')) {
			// Skip useless chunk and dummied 'data' header
			wavStream->skip(wavStream->readUint32LE());
			tag = wavStream->readUint32BE();
		}

		if (tag != MKID_BE('data')) {
			warning("SoundManager::playSoundFile(): Found invalid tag in WAVE file: %x", tag);
			return -1;
		}

		uint32 dataSize = wavStream->readUint32LE();
		if (dataSize == 0)
			isMP3 = true;
		else
			wavStream->seek(0);

	} else if ((tag == MKID_BE('BMU ')) && (wavStream->readUint32BE() == MKID_BE('V1.0'))) {

		// BMU files, MP3 with extra header
		isMP3 = true;

	} else
		// Let SDL_sound sort it out
		wavStream->seek(0);

	SDL_RWops *rw = RW_FromStream(wavStream);
	if (!rw) {
		warning("SoundManager::playSoundFile(): Failed to create SDL_RWops from wav stream");
		return -1;
	}

	Sound_AudioInfo audioInfo;
	audioInfo.channels = NUM_CHANNELS;
	audioInfo.format = AUDIO_S16SYS;
	audioInfo.rate = SAMPLE_RATE;

	Sound_Sample *sound = Sound_NewSample(rw, isMP3 ? "mp3" : "wav", &audioInfo, BUFFER_SIZE);
	if (!sound) {
		warning("SoundManager::playSoundFile(): Unable to load sound file: %s", Sound_GetError());
		return -1;
	}

	// Decode all samples
	Sound_DecodeAll(sound);

	Channel *channel = new Channel;
	channel->sound = sound;
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

	if (c->sound) {
		Sound_FreeSample(c->sound);
		c->sound = 0;
	}

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

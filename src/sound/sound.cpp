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

#include <SDL_mixer.h>
#include <SDL_sound.h>

#include "sound/sound.h"

#include "common/stream.h"
#include "common/util.h"

#include "graphics/graphics.h"

DECLARE_SINGLETON(Sound::SoundManager)

namespace Sound {

#define NUM_CHANNELS 2
#define SAMPLE_RATE 44100
#define BUFFER_SIZE 4096

SoundManager::SoundManager() {
	_ready = false;
}

bool SoundManager::init() {
	if (!GfxMan.ready()) {
		warning("SoundManager::init(): The GraphicsManager needs to be initialized first");
		return false;
	}

	Sound_Init();

	if (Mix_OpenAudio(SAMPLE_RATE, AUDIO_S16SYS, NUM_CHANNELS, BUFFER_SIZE)) {
		warning("SoundManager::init(): Unable to initialize audio: %s", Mix_GetError());
		return false;
	}

	_ready = true;
	return true;
}

void SoundManager::deinit() {
	if (!_ready)
		return;

	Mix_CloseAudio();
	Sound_Quit();

	_ready = false;
}

bool SoundManager::ready() const {
	return _ready;
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

bool SoundManager::playSoundFile(Common::SeekableReadStream *wavStream) {
	if (!wavStream) {
		warning("SoundManager::playSoundFile(): No stream");
		return false;
	}

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
			return false;
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
			return false;
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
		return false;
	}

	Sound_AudioInfo audioInfo;
	audioInfo.channels = NUM_CHANNELS;
	audioInfo.format = AUDIO_S16SYS;
	audioInfo.rate = SAMPLE_RATE;

	Sound_Sample *sound = Sound_NewSample(rw, isMP3 ? "mp3" : "wav", &audioInfo, BUFFER_SIZE);
	if (!sound) {
		warning("SoundManager::playSoundFile(): Unable to load sound file: %s", Sound_GetError());
		return false;
	}

	// Decode all samples
	Sound_DecodeAll(sound);

	Mix_Chunk *wav = Mix_QuickLoad_RAW((byte *)sound->buffer, sound->buffer_size);
	if (!wav) {
		warning("SoundManager::playSoundFile(): Unable to load WAV file: %s", Mix_GetError());
		Sound_FreeSample(sound);
		return false;
	}

	int channel = Mix_PlayChannel(-1, wav, 0);
	if(channel == -1) {
		warning("SoundManager::playSoundFile(): Unable to play WAV file: %s", Mix_GetError());
		Sound_FreeSample(sound);
		Mix_FreeChunk(wav);
		return false;
	}

	while (Mix_Playing(channel))
		sleep(1);

	Sound_FreeSample(sound);
	Mix_FreeChunk(wav);

	return true;
}

} // End of namespace Sound

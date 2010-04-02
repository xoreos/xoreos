/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include <SDL_mixer.h>

#include "common/stream.h"
#include "common/util.h"

#include "sound/sound.h"

namespace Sound {

bool initMixer() {
	return Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) == 0;
}

void deinitMixer() {
	Mix_CloseAudio();
}

const char *getMixerError() {
	return Mix_GetError();
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

void playSoundFile(Common::SeekableReadStream *wavStream) {
	if (!wavStream) {
		warning("Attempting to play NULL wavStream");
		return;
	}

	if (wavStream->readUint32BE() == 0xfff360c4) {
		// Modified WAVE file (used in streamsounds folder, at least in KotOR 1/2)
		wavStream->seek(0x1D6);
	} else
		wavStream->seek(0);

	SDL_RWops *rw = RW_FromStream(wavStream);
	if (!rw) {
		warning("Failed to create SDL_RWops from wav stream");
		return;
	}

	Mix_Chunk *wav = Mix_LoadWAV_RW(rw, true);
	if (!wav) {
		warning("Unable to load WAV file: %s", Mix_GetError());
		FreeRW_FromStream(rw);
		return;
	}

	int channel = Mix_PlayChannel(-1, wav, 0);
	if(channel == -1) {
		warning("Unable to play WAV file: %s", Mix_GetError());
		Mix_FreeChunk(wav);
		return;
	}

	warning("And smile :)");

	while (Mix_Playing(channel)) {
		sleep(1);
	}

	Mix_FreeChunk(wav);
}

} // End of namespace Sound

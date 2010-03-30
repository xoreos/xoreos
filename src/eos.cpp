/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include <cstdio>

#include <SDL.h>
#include <SDL_mixer.h>

#include "common/stream.h"
#include "common/util.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/keyfile.h"
#include "aurora/biffile.h"
#include "aurora/resman.h"
#include "aurora/talktable.h"

bool initSDL();
void deinitSDL();

void doAuroraStuff(Aurora::ResourceManager &resMan);
void playWav(Common::SeekableReadStream *wavStream);

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	if (!Common::FilePath::isDirectory(argv[1])) {
		warning("No such directory \"%s\"", argv[1]);
		return 0;
	}

	if (!initSDL())
		error("Fatal");

	atexit(deinitSDL);

	Aurora::ResourceManager resMan;

	bool success = resMan.registerDataBaseDir(argv[1]);

	warning("Useful base dir? %s - %d keys", success ? "Yes" : "No", (int) resMan.getKEYList().size());

	if (success)
		doAuroraStuff(resMan);

	return 0;
}

bool initSDL() {
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0) {
		warning("initSDL(): Failed to initialize SDL: %s", SDL_GetError());
		return false;
	}

	if (Mix_OpenAudio(44100, AUDIO_S16SYS, 2, 4096) != 0) {
		warning("initSDL(): Unable to initialize audio: %s", Mix_GetError());
		return false;
	}

	return true;
}

void deinitSDL() {
	Mix_CloseAudio();
	SDL_Quit();
}

static const char *wavFiles[] = {"p_hk-47_tia", "p_hk47_tia", "as_pl_evanglstm1"};

void doAuroraStuff(Aurora::ResourceManager &resMan) {
	const Common::FileList &keyList = resMan.getKEYList();

	Common::SeekableReadStream *chitinKEY = keyList.openFile(".*/chitin.key", true);
	if (!chitinKEY) {
		warning("But has no chitin.key");
		return ;
	}

	warning("And has a chitin.key");

	if (!resMan.loadKEY(*chitinKEY)) {
		warning("But loading it failed");
		return;
	}

	Common::SeekableReadStream *wav = 0;
	for (int i = 0; i < ARRAYSIZE(wavFiles); i++)
		if ((wav = resMan.getResource(wavFiles[i], Aurora::kFileTypeWAV)))
			break;

	if (wav) {
		warning("Found a wav. Trying to play it. Turn up your speakers");
		playWav(wav);
	}

	delete chitinKEY;
}

static int RWStreamSeek(SDL_RWops *context, int offset, int whence) {
	if (context->type != 0xc0ffeeee)
		return -1;

	return ((Common::SeekableReadStream *) context->hidden.unknown.data1)->seek(offset, whence);
}

static int RWStreamRead(SDL_RWops *context, void *ptr, int size, int maxnum) {
	if (context->type != 0xc0ffeeee)
		return 0;

	int n = ((Common::SeekableReadStream *) context->hidden.unknown.data1)->read(ptr, size * maxnum);

	return n / size;
}

static int RWStreamWrite(SDL_RWops *context, const void *ptr, int size, int num) {
	return 0;
}

static int RWStreamClose(SDL_RWops *context) {
	if (context->type != 0xc0ffeeee)
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

	rw->seek  = RWStreamSeek;
	rw->read  = RWStreamRead;
	rw->write = RWStreamWrite;
	rw->close = RWStreamClose;

	rw->type = 0xc0ffeeee;
	rw->hidden.unknown.data1 = stream;

	return rw;
}

void FreeRW_FromStream(SDL_RWops *rw) {
	if (rw->type != 0xc0ffeeee)
		return;

	rw->close(rw);
}

void playWav(Common::SeekableReadStream *wavStream) {
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

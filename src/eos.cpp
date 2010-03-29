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

bool initSDL();
void playWav(Common::SeekableReadStream &wavStream);

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

	Aurora::ResourceManager resMan;

	bool success = resMan.registerDataBaseDir(argv[1]);

	const Common::FileList &keyList = resMan.getKeyList();

	warning("Useful base dir? %s - %d keys", success ? "Yes" : "No", (int) keyList.size());

	if (success) {
		Common::SeekableReadStream *chitinKey = keyList.openFile(".*/chitin.key", true);
		if (chitinKey) {
			warning("And has a chitin.key");

			bool loaded = resMan.loadKey(*chitinKey);
			warning("Could load chitin.key? %s", loaded ? "Yes" : "No");

			Common::SeekableReadStream *wav1 = resMan.getResource("p_zaalbar_tia", Aurora::kFileTypeWAV);
			Common::SeekableReadStream *wav2 = resMan.getResource("as_pl_evanglstm1", Aurora::kFileTypeWAV);

			if (wav1 || wav2) {
				warning("Found a wav. Trying to play it. Turn up your speakers");

				Common::SeekableReadStream *wavR = (wav1) ? wav1 : wav2;

				playWav(*wavR);
/*				warning("Found a WAV. Writing it to foo.wav. Play it and smile :)");
				Common::DumpFile wav;
				if (wav.open("foo.wav")) {
					wav.writeStream(*wavR);
					wav.close();
				} else
					warning("Failed to write foo.wav :(");*/

			}

			delete wav1;
			delete wav2;

		} else
			warning("But has no chitin.key");

		delete chitinKey;
	}

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

	atexit(SDL_Quit);

	return true;
}

void playWav(Common::SeekableReadStream &wavStream) {
	uint32 size = wavStream.size();
	byte *buf = new byte[size];

	wavStream.read(buf, size);

	SDL_RWops *rw = SDL_RWFromMem(buf, size);

	Mix_Chunk *wav = Mix_LoadWAV_RW(rw, true);
	if (!wav) {
		warning("Unable to load WAV file: %s", Mix_GetError());
		delete[] buf;
		return;
	}

	int channel = Mix_PlayChannel(-1, wav, 0);
	if(channel == -1) {
		warning("Unable to play WAV file: %s", Mix_GetError());
		delete[] buf;
		return;
	}

	warning("And smile :)");

	while (Mix_Playing(channel)) {
		sleep(1);
	}

	Mix_FreeChunk(wav);
	Mix_CloseAudio();

	delete[] buf;
}

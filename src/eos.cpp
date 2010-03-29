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
#include <SDL/SDL.h>

#include "common/stream.h"
#include "common/util.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/keyfile.h"
#include "aurora/biffile.h"
#include "aurora/resman.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	if (!Common::FilePath::isDirectory(argv[1])) {
		warning("No such directory \"%s\"", argv[1]);
		return 0;
	}

	// Some quick SDL initialization tests
	// TODO: Move SDL initialization elsewhere? But, the SDL.h include must remain either way.
	if (SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO) < 0)
		error("Failed to initialize SDL: %s", SDL_GetError());

	atexit(SDL_Quit);

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
				Common::SeekableReadStream *wavR = (wav1) ? wav1 : wav2;

				warning("Found a WAV. Writing it to foo.wav. Play it and smile :)");
				Common::DumpFile wav;
				if (wav.open("foo.wav")) {
					wav.writeStream(*wavR);
					wav.close();
				} else
					warning("Failed to write foo.wav :(");

			}

			delete wav1;
			delete wav2;

		} else
			warning("But has no chitin.key");

		delete chitinKey;
	}

	return 0;
}

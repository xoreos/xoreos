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
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/keyfile.h"
#include "aurora/biffile.h"

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	if (!Common::FilePath::isDirectory(argv[1])) {
		warning("No such directory \"%s\"", argv[1]);
		return 0;
	}

	Common::FileList files;
	files.addDirectory(argv[1], -1);

	Common::FileList keyFiles, bifFiles;
	files.getSubList(".*\\.key", keyFiles, true);
	files.getSubList(".*\\.bif", bifFiles, true);

	if (keyFiles.isEmpty() || bifFiles.isEmpty()) {
		warning("No KEY or BIF files found. Path most probably does not contain an Aurora game.");
		return 0;
	}

	warning("Opening \"%s\"", keyFiles.begin()->c_str());
	Common::SeekableReadStream *keyStream = keyFiles.openFile(*keyFiles.begin());
	if (keyStream) {
		Aurora::KeyFile key;

		bool success = key.load(*keyStream);

		const Aurora::KeyFile::BifList &bifs     = key.getBifs();
		const Aurora::KeyFile::ResourceList &res = key.getResources();

		warning("Success? %d - # bif: %d - # res: %d", success, (int) bifs.size(), (int) res.size());

	} else {
		warning("Fail :(");
	}

	delete keyStream;

	warning("Opening \"%s\"", bifFiles.begin()->c_str());
	Common::SeekableReadStream *bifStream = bifFiles.openFile(*bifFiles.begin());
	if (bifStream) {
		Aurora::BifFile bif;

		bool success = bif.load(*bifStream);

		const Aurora::BifFile::ResourceList &res = bif.getResources();

		warning("Success? %d - # res: %d", success, (int) res.size());

	} else {
		warning("Fail :(");
	}

	delete bifStream;

	return 0;
}

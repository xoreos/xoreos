/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file eos.cpp
 *  The project's main entry point.
 */

#include <cstdio>

#include <SDL.h>

#include "common/stream.h"
#include "common/util.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "aurora/resman.h"

#include "sound/sound.h"

bool initSDL();
void deinitSDL();

void dumpStream(Common::SeekableReadStream &stream, const std::string &fileName);

void doAuroraStuff(Aurora::ResourceManager &resMan);

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

	if (!SoundMan.initMixer()) {
		warning("initSDL(): Unable to initialize audio: %s", SoundMan.getMixerError());
		return false;
	}

	return true;
}

void deinitSDL() {
	SoundMan.deinitMixer();
	SDL_Quit();
}

void dumpStream(Common::SeekableReadStream &stream, const std::string &fileName) {
	uint32 pos = stream.pos();

	stream.seek(0);

	Common::DumpFile file;
	if (!file.open(fileName)) {
		stream.seek(pos);
		return;
	}

	file.writeStream(stream);
	file.close();

	stream.seek(pos);
}

static const char *mainKEYFiles[] = {".*/chitin.key", ".*/main.key"};
static const char *wavFiles[] = {"nm35aahhkd07134_", "p_hk47_tia", "as_pl_evanglstm1", "hello"};

void doAuroraStuff(Aurora::ResourceManager &resMan) {
	const Common::FileList &keyList = resMan.getKEYList();

	Common::SeekableReadStream *mainKEY = 0;
	for (int i = 0; i < ARRAYSIZE(mainKEYFiles); i++)
		if ((mainKEY = keyList.openFile(mainKEYFiles[i], true)))
			break;

	if (!mainKEY) {
		warning("But has no main KEY file");
		return ;
	}

	warning("And has a main KEY file");

	if (!resMan.loadKEY(*mainKEY)) {
		warning("But loading it failed");
		return;
	}

	delete mainKEY;

	Common::SeekableReadStream *wav = 0;
	for (int i = 0; i < ARRAYSIZE(wavFiles); i++)
		if ((wav = resMan.getSound(wavFiles[i])))
			break;

	if (wav) {
		warning("Found a wav. Trying to play it. Turn up your speakers");
		SoundMan.playSoundFile(wav);
	}
}

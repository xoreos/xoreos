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

Aurora::ResourceManager *initResMan(const std::string &baseDir);
bool initResKEYs(Aurora::ResourceManager &resMan);
Common::SeekableReadStream *openResKey(Aurora::ResourceManager &resMan, const std::string &keyName);
bool loadResKEY(Aurora::ResourceManager &resMan, const std::string &keyName);
bool loadResKEYTry(const std::string &msg, Aurora::ResourceManager &resMan, const std::string &keyName);

void doAuroraStuff(Aurora::ResourceManager &resMan);

int main(int argc, char **argv) {
	if (argc < 2) {
		std::printf("Usage: %s </path/to/aurora/game>\n", argv[0]);
		return 0;
	}

	std::string baseDir = Common::FilePath::makeAbsolute((const std::string &) argv[1]);

	if (!Common::FilePath::isDirectory(baseDir)) {
		warning("No such directory \"%s\"", baseDir.c_str());
		return 0;
	}

	if (!initSDL())
		error("Fatal");

	atexit(deinitSDL);

	Aurora::ResourceManager *resMan = initResMan(baseDir);
	if (!resMan)
		return 0;

	warning("Successfully initialized the resource manager");

	doAuroraStuff(*resMan);

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

Aurora::ResourceManager *initResMan(const std::string &baseDir) {
	Aurora::ResourceManager *resMan = new Aurora::ResourceManager;

	warning("Setting up the resource manager's base directory to %s", baseDir.c_str());
	if (!resMan->registerDataBaseDir(baseDir)) {
		delete resMan;
		return 0;
	}

	if (resMan->getGameID() == Aurora::kGameIDUnknown) {
		warning("Failed detecting the game ID");
		return 0;
	}

	warning("Detected game ID %d", resMan->getGameID());

	warning("Found %d KEYs", (int) resMan->getKEYList().size());

	if (!initResKEYs(*resMan)) {
		delete resMan;
		return 0;
	}

	warning("Loading secondary resources");
	if (!resMan->loadSecondaryResources()) {
		delete resMan;
		return 0;
	}

	return resMan;
}

bool initResKEYs(Aurora::ResourceManager &resMan) {
	if        (resMan.getGameID() == Aurora::kGameIDNWN) {
		warning("Loading the main KEY");
		if (!loadResKEY(resMan, ".*/chitin.key"))
			return false;

		if (!loadResKEYTry("Loading the patch KEY", resMan, ".*/patch.key"))
			return false;
		if (!loadResKEYTry("Loading the first extension's KEY", resMan, ".*/xp1.key"))
			return false;
		if (!loadResKEYTry("Loading the first extension's patch KEY", resMan, ".*/xp1patch.key"))
			return false;
		if (!loadResKEYTry("Loading the second extension's KEY", resMan, ".*/xp2.key"))
			return false;
		if (!loadResKEYTry("Loading the second extension's patch KEY", resMan, ".*/xp2patch.key"))
			return false;
		if (!loadResKEYTry("Loading the final update's KEY", resMan, ".*/xp3.key"))
			return false;
		if (!loadResKEYTry("Loading the final update's patch KEY", resMan, ".*/xp3patch.key"))
			return false;

	} else if (resMan.getGameID() == Aurora::kGameIDNWN2) {
		warning("Loading the main KEY");
		if (!loadResKEY(resMan, ".*/chitin.key"))
			return false;

	} else if (resMan.getGameID() == Aurora::kGameIDKotOR) {
		warning("Loading the main KEY");
		if (!loadResKEY(resMan, ".*/chitin.key"))
			return false;

	} else if (resMan.getGameID() == Aurora::kGameIDKotOR2) {
		warning("Loading the main KEY");
		if (!loadResKEY(resMan, ".*/chitin.key"))
			return false;

	} else if (resMan.getGameID() == Aurora::kGameIDTheWitcher) {
		warning("Loading the main KEY");
		if (!loadResKEY(resMan, ".*/main.key"))
			return false;

		warning("Loading the localized base KEY");
		if (!loadResKEY(resMan, ".*/localized.key"))
			return false;

		resMan.addBIFSourceDir("voices");

		warning("Loading the English language KEYs");
		if (!loadResKEY(resMan, ".*/lang_3.key"))
			return false;
		if (!loadResKEY(resMan, ".*/M1_3.key"))
			return false;
		if (!loadResKEY(resMan, ".*/M2_3.key"))
			return false;
	}

	return true;
}

bool loadResKEY(Aurora::ResourceManager &resMan, const std::string &keyName) {
	Common::SeekableReadStream *keyFile = resMan.getKEYList().openFile(keyName, true);
	if (!keyFile) {
		warning("Not found");
		return false;
	}

	if (!resMan.loadKEY(*keyFile)) {
		delete keyFile;
		return false;
	}

	delete keyFile;
	return true;
}

bool loadResKEYTry(const std::string &msg, Aurora::ResourceManager &resMan, const std::string &keyName) {
	Common::SeekableReadStream *keyFile = resMan.getKEYList().openFile(keyName, true);
	if (!keyFile)
		return true;

	warning("%s", msg.c_str());

	if (!resMan.loadKEY(*keyFile)) {
		delete keyFile;
		return false;
	}

	delete keyFile;
	return true;
}

static const char *wavFiles[] = {"nm35aahhkd07134_", "298hk50mun003", "as_pl_evanglstm1", "hello"};

void doAuroraStuff(Aurora::ResourceManager &resMan) {
	Common::SeekableReadStream *wav = 0;
	for (int i = 0; i < ARRAYSIZE(wavFiles); i++)
		if ((wav = resMan.getSound(wavFiles[i])))
			break;

	if (wav) {
		warning("Found a wav. Trying to play it. Turn up your speakers");
		SoundMan.playSoundFile(wav);
	}
}

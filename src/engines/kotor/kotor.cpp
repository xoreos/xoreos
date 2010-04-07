/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/kotor.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#include "engines/kotor/kotor.h"

#include "common/util.h"
#include "common/filelist.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"

namespace KotOR {

const KotOREngineProbe kKotOREngineProbe;

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

bool KotOREngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// If either swkotor.ini or swkotor.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor.(exe|ini)", true);
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine;
}


KotOREngine::KotOREngine() {
}

KotOREngine::~KotOREngine() {
}

bool KotOREngine::run(const std::string &directory) {
	_baseDirectory = directory;

	if (!init())
		return false;

	warning("Successfully initialized the engine");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("nm35aahhkd07134_");
	if (wav) {
		warning("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav);
	}

	while (!EventMan.quitRequested()) {
		if (!SoundMan.isPlaying(channel))
			EventMan.requestQuit();

		Events::Event event;
		while (EventMan.pollEvent(event));

		EventMan.delay(10);
	}

	return true;
}

bool KotOREngine::init() {
	if (!ResMan.registerDataBaseDir(_baseDirectory))
		return false;

	warning("Loading main KEY");
	if (!indexMandatoryKEY(".*/chitin.key"))
		return false;

	warning("Loading secondary resources");
	if (!ResMan.loadSecondaryResources())
		return false;

	return true;
}

} // End of namespace KotOR

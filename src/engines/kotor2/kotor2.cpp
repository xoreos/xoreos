/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/kotor2.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#include "engines/kotor2/kotor2.h"

#include "common/util.h"
#include "common/filelist.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"

namespace KotOR2 {

const KotOR2EngineProbe kKotOR2EngineProbe;

Aurora::GameID KotOR2EngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR2;
}

bool KotOR2EngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// If either swkotor2.ini or swkotor2.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor2.(exe|ini)", true);
}

Engines::Engine *KotOR2EngineProbe::createEngine() const {
	return new KotOR2Engine;
}


KotOR2Engine::KotOR2Engine() {
}

KotOR2Engine::~KotOR2Engine() {
}

bool KotOR2Engine::run(const std::string &directory) {
	_baseDirectory = directory;

	if (!init())
		return false;

	status("Successfully initialized the engine");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getSound("298hk50mun003");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
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

bool KotOR2Engine::init() {
	if (!ResMan.registerDataBaseDir(_baseDirectory))
		return false;

	status("Loading main KEY");
	if (!indexMandatoryKEY(".*/chitin.key"))
		return false;

	status("Loading secondary resources");
	if (!ResMan.loadSecondaryResources())
		return false;

	return true;
}

} // End of namespace KotOR2

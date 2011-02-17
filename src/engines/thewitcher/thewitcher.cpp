/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/thewitcher/thewitcher.cpp
 *  Engine class handling The Witcher
 */

#include "engines/thewitcher/thewitcher.h"
#include "engines/thewitcher/modelloader.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/stream.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/model.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

namespace TheWitcher {

const TheWitcherEngineProbe kTheWitcherEngineProbe;

const Common::UString TheWitcherEngineProbe::kGameName = "The Witcher";

Aurora::GameID TheWitcherEngineProbe::getGameID() const {
	return Aurora::kGameIDTheWitcher;
}

const Common::UString &TheWitcherEngineProbe::getGameName() const {
	return kGameName;
}

bool TheWitcherEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// There should be a system directory
	Common::UString systemDir = Common::FilePath::findSubDirectory(directory, "system", true);
	if (systemDir.empty())
		return false;

	// The system directory has to be readable
	Common::FileList systemFiles;
	if (!systemFiles.addDirectory(systemDir))
		return false;

	// If either witcher.ini or witcher.exe exists, this should be a valid path
	return systemFiles.contains(".*/witcher.(exe|ini)", true);
}

bool TheWitcherEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *TheWitcherEngineProbe::createEngine() const {
	return new TheWitcherEngine;
}


TheWitcherEngine::TheWitcherEngine() {
}

TheWitcherEngine::~TheWitcherEngine() {
}

void TheWitcherEngine::run(const Common::UString &target) {
	_baseDirectory = Common::FilePath::findSubDirectory(target, "data", true);
	if (_baseDirectory.empty())
		throw Common::Exception("No data directory");

	init();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	playVideo("publisher");
	playVideo("developer");
	playVideo("engine");
	playVideo("intro");
	playVideo("title");

	playSound("m1_axem00020005", Sound::kSoundTypeVoice);

	Graphics::Aurora::Model *model = loadModelObject("cm_naked3");

	model->show();

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete model;
}

void TheWitcherEngine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "voices");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules/!final");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "main.key", 0);

	status("Loading the localized base KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "localized.key", 10);

	status("Loading the English language KEYs");
	indexMandatoryArchive(Aurora::kArchiveKEY, "lang_3.key", 20);
	indexMandatoryArchive(Aurora::kArchiveKEY, "M1_3.key"  , 21);
	indexMandatoryArchive(Aurora::kArchiveKEY, "M2_3.key"  , 22);

	status("Indexing extra resources");
	indexOptionalDirectory("movies"   , 0, -1, 30);
	indexOptionalDirectory("music"    , 0, -1, 31);
	indexOptionalDirectory("sounds"   , 0, -1, 32);
	indexOptionalDirectory("cutscenes", 0, -1, 33);
	indexOptionalDirectory("dialogues", 0, -1, 34);
	indexOptionalDirectory("fx"       , 0, -1, 35);
	indexOptionalDirectory("meshes"   , 0, -1, 36);
	indexOptionalDirectory("quests"   , 0, -1, 37);
	indexOptionalDirectory("scripts"  , 0, -1, 38);
	indexOptionalDirectory("templates", 0, -1, 39);
	indexOptionalDirectory("textures" , 0, -1, 40);

	indexOptionalDirectory("", ".*\\.bik", 0, 41);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 50);

	registerModelLoader(new TheWitcherModelLoader);

	FontMan.setFormat(Graphics::Aurora::kFontFormatTTF);
}

} // End of namespace TheWitcher

} // End of namespace Engines

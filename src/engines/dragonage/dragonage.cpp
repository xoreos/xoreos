/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/dragonage/dragonage.cpp
 *  Engine class handling Dragon Age: Origins
 */

#include "engines/dragonage/dragonage.h"

#include "engines/util.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"

#include "graphics/aurora/cube.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

namespace DragonAge {

const DragonAgeEngineProbe kDragonAgeEngineProbe;

const Common::UString DragonAgeEngineProbe::kGameName = "Dragon Age: Origins";

Aurora::GameID DragonAgeEngineProbe::getGameID() const {
	return Aurora::kGameIDDragonAge;
}

const Common::UString &DragonAgeEngineProbe::getGameName() const {
	return kGameName;
}

bool DragonAgeEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains(".*/daoriginslauncher.exe", true))
		return true;

	return false;
}

bool DragonAgeEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *DragonAgeEngineProbe::createEngine() const {
	return new DragonAgeEngine;
}


DragonAgeEngine::DragonAgeEngine() {
}

DragonAgeEngine::~DragonAgeEngine() {
}

void DragonAgeEngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();

	status("Successfully initialized the engine");

	playVideo("dragon_age_ea_logo");
	playVideo("dragon_age_main");

	Graphics::Aurora::Cube *cube = 0;

	try {

		cube = new Graphics::Aurora::Cube("ach_abi_accomplish_rog");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
}

void DragonAgeEngine::init() {
	ResMan.setRIMsAreERFs(true);

	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data/abilities");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/env");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules/single player/data");

	status("Loading core resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "2da.erf"               ,  0);
	indexMandatoryArchive(Aurora::kArchiveERF, "anims.erf"             ,  1);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.gpu.rim"       ,  2);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.rim"           ,  3);
	indexMandatoryArchive(Aurora::kArchiveERF, "consolescripts.erf"    ,  4);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerareas.erf"     ,  5);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercreatures.erf" ,  6);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercutscenes.erf" ,  7);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerdialogs.erf"   ,  8);
	indexMandatoryArchive(Aurora::kArchiveERF, "designeritems.erf"     ,  9);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplaceables.erf", 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplots.erf"     , 11);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerscripts.rim"   , 12);
	indexMandatoryArchive(Aurora::kArchiveERF, "designertriggers.erf"  , 13);
	indexMandatoryArchive(Aurora::kArchiveERF, "face.erf"              , 14);
	indexMandatoryArchive(Aurora::kArchiveERF, "global.rim"            , 15);
	indexMandatoryArchive(Aurora::kArchiveERF, "globalvfx.rim"         , 16);
	indexMandatoryArchive(Aurora::kArchiveERF, "gui.erf"               , 17);
	indexMandatoryArchive(Aurora::kArchiveERF, "guiexport.erf"         , 18);
	indexMandatoryArchive(Aurora::kArchiveERF, "iterationtests.erf"    , 19);
	indexMandatoryArchive(Aurora::kArchiveERF, "lightprobedata.erf"    , 20);
	indexMandatoryArchive(Aurora::kArchiveERF, "materialobjects.erf"   , 21);
	indexMandatoryArchive(Aurora::kArchiveERF, "materials.erf"         , 22);
	indexMandatoryArchive(Aurora::kArchiveERF, "misc.erf"              , 23);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelhierarchies.erf"  , 24);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelmeshdata.erf"     , 25);
	indexMandatoryArchive(Aurora::kArchiveERF, "pathfindingpatches.erf", 26);
	indexMandatoryArchive(Aurora::kArchiveERF, "postprocesseffects.erf", 27);
	indexMandatoryArchive(Aurora::kArchiveERF, "resmetrics.erf"        , 28);
	indexMandatoryArchive(Aurora::kArchiveERF, "scripts.erf"           , 29);
	indexMandatoryArchive(Aurora::kArchiveERF, "shaders.erf"           , 30);
	indexMandatoryArchive(Aurora::kArchiveERF, "states.erf"            , 31);
	indexMandatoryArchive(Aurora::kArchiveERF, "subqueuefiles.erf"     , 32);
	indexMandatoryArchive(Aurora::kArchiveERF, "textures.erf"          , 33);
	indexMandatoryArchive(Aurora::kArchiveERF, "tints.erf"             , 34);

	status("Loading core ability resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "bearform.rim"    , 40);
	indexMandatoryArchive(Aurora::kArchiveERF, "burningform.rim" , 41);
	indexMandatoryArchive(Aurora::kArchiveERF, "golemform.rim"   , 42);
	indexMandatoryArchive(Aurora::kArchiveERF, "mouseform.rim"   , 43);
	indexMandatoryArchive(Aurora::kArchiveERF, "spiderform.rim"  , 44);
	indexMandatoryArchive(Aurora::kArchiveERF, "spiritform.rim"  , 45);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonbear.rim"  , 46);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonspider.rim", 47);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonwolf.rim"  , 48);

	status("Indexing extra core sound resources");
	indexMandatoryDirectory("packages/core/audio"          , 0, -1, 100);
	status("Indexing extra core movie resources");
	indexMandatoryDirectory("packages/core/data/movies"    , 0,  0, 101);
	status("Indexing extra core talktables");
	indexMandatoryDirectory("packages/core/data/talktables", 0,  0, 102);

	status("Indexing extra environments");
	indexMandatoryDirectory("packages/core/env", 0, 0, 110);

	status("Loading single-player campaign global resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "moduleglobal.rim", 50);

	status("Indexing extra single-player campaign movie resources");
	indexMandatoryDirectory("modules/single player/data"           , 0, -1, 120);
	status("Indexing extra single-player campaign sound resources");
	indexMandatoryDirectory("modules/single player/data/movies"    , 0,  0, 121);
	status("Indexing extra single-player campaign talktables");
	indexMandatoryDirectory("modules/single player/data/talktables", 0,  0, 122);
}

} // End of namespace DragonAge

} // End of namespace Engines

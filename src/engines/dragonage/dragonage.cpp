/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/dragonage/dragonage.cpp
 *  Engine class handling Dragon Age: Origins
 */

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "aurora/resman.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "engines/dragonage/dragonage.h"

namespace Engines {

namespace DragonAge {

const DragonAgeEngineProbe kDragonAgeEngineProbe;

const Common::UString DragonAgeEngineProbe::kGameName = "Dragon Age: Origins";

DragonAgeEngineProbe::DragonAgeEngineProbe() {
}

DragonAgeEngineProbe::~DragonAgeEngineProbe() {
}

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
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	/*
	playVideo("dragon_age_ea_logo");
	playVideo("dragon_age_main");
	if (EventMan.quitRequested())
		return;
	*/

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}
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
	indexMandatoryArchive(Aurora::kArchiveERF, "2da.erf"               ,  1);
	indexMandatoryArchive(Aurora::kArchiveERF, "anims.erf"             ,  2);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.gpu.rim"       ,  3);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.rim"           ,  4);
	indexMandatoryArchive(Aurora::kArchiveERF, "consolescripts.erf"    ,  5);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerareas.erf"     ,  6);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercreatures.erf" ,  7);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercutscenes.erf" ,  8);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerdialogs.erf"   ,  9);
	indexMandatoryArchive(Aurora::kArchiveERF, "designeritems.erf"     , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplaceables.erf", 11);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplots.erf"     , 12);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerscripts.rim"   , 13);
	indexMandatoryArchive(Aurora::kArchiveERF, "designertriggers.erf"  , 14);
	indexMandatoryArchive(Aurora::kArchiveERF, "face.erf"              , 15);
	indexMandatoryArchive(Aurora::kArchiveERF, "global.rim"            , 16);
	indexMandatoryArchive(Aurora::kArchiveERF, "globalvfx.rim"         , 17);
	indexMandatoryArchive(Aurora::kArchiveERF, "gui.erf"               , 18);
	indexMandatoryArchive(Aurora::kArchiveERF, "guiexport.erf"         , 19);
	indexMandatoryArchive(Aurora::kArchiveERF, "iterationtests.erf"    , 20);
	indexMandatoryArchive(Aurora::kArchiveERF, "lightprobedata.erf"    , 21);
	indexMandatoryArchive(Aurora::kArchiveERF, "materialobjects.erf"   , 22);
	indexMandatoryArchive(Aurora::kArchiveERF, "materials.erf"         , 23);
	indexMandatoryArchive(Aurora::kArchiveERF, "misc.erf"              , 24);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelhierarchies.erf"  , 25);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelmeshdata.erf"     , 26);
	indexMandatoryArchive(Aurora::kArchiveERF, "pathfindingpatches.erf", 27);
	indexMandatoryArchive(Aurora::kArchiveERF, "postprocesseffects.erf", 28);
	indexMandatoryArchive(Aurora::kArchiveERF, "resmetrics.erf"        , 29);
	indexMandatoryArchive(Aurora::kArchiveERF, "scripts.erf"           , 30);
	indexMandatoryArchive(Aurora::kArchiveERF, "shaders.erf"           , 31);
	indexMandatoryArchive(Aurora::kArchiveERF, "states.erf"            , 32);
	indexMandatoryArchive(Aurora::kArchiveERF, "subqueuefiles.erf"     , 33);
	indexMandatoryArchive(Aurora::kArchiveERF, "textures.erf"          , 34);
	indexMandatoryArchive(Aurora::kArchiveERF, "tints.erf"             , 35);

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
	status("Indexing extra core cursors");
	indexMandatoryDirectory("packages/core/data/cursors"   , 0,  0, 103);

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

void DragonAgeEngine::initCursors() {
}

} // End of namespace DragonAge

} // End of namespace Engines

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

/** @file engines/dragonage2/dragonage2.cpp
 *  Engine class handling Dragon Age II
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

#include "engines/dragonage2/dragonage2.h"

namespace Engines {

namespace DragonAge2 {

const DragonAge2EngineProbe kDragonAge2EngineProbe;

const Common::UString DragonAge2EngineProbe::kGameName = "Dragon Age II";

DragonAge2EngineProbe::DragonAge2EngineProbe() {
}

DragonAge2EngineProbe::~DragonAge2EngineProbe() {
}

Aurora::GameID DragonAge2EngineProbe::getGameID() const {
	return Aurora::kGameIDDragonAge2;
}

const Common::UString &DragonAge2EngineProbe::getGameName() const {
	return kGameName;
}

bool DragonAge2EngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains(".*/dragonage2launcher.exe", true))
		return true;

	return false;
}

bool DragonAge2EngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *DragonAge2EngineProbe::createEngine() const {
	return new DragonAge2Engine;
}


DragonAge2Engine::DragonAge2Engine() {
}

DragonAge2Engine::~DragonAge2Engine() {
}

void DragonAge2Engine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	/*
	playVideo("dragon_age_ea_logo");
	if (EventMan.quitRequested())
		return;
	*/

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}
}

void DragonAge2Engine::init() {
	ResMan.setRIMsAreERFs(true);

	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures/medium");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures/high");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/audio/sound/");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/patch");

	status("Loading core resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/2da.rim"                      ,  1);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/al_char_stage.rim"            ,  2);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/artfp.erf"                    ,  3);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/artfpwin32.erf"               ,  4);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/chargen.gpu.rim"              ,  5);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/chargen.rim"                  ,  6);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/designerresources.rim"        ,  7);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/designerscripts.rim"          ,  8);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/face.erf"                     ,  9);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/fonts.erf"                    , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/global-uncompressed.rim"      , 11);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/global.rim"                   , 12);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globalani-core.rim"           , 13);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globalchargen-core.rim"       , 14);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globalchargendds-core.gpu.rim", 15);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globaldds-core.gpu.rim"       , 16);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globalmao-core.rim"           , 17);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/globalvfx-core.rim"           , 18);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/gui.erf"                      , 19);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/guiexport.erf"                , 20);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/layouts.erf"                  , 21);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/lightprobedata.erf"           , 22);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/maoinit.erf"                  , 23);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/materialdefinitions.erf"      , 24);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/materialobjects.rim"          , 25);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/misc.erf"                     , 26);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/models.erf"                   , 27);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/pathfindingpatches.rim"       , 28);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/resmetrics.erf"               , 29);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/scripts.erf"                  , 30);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/shaders.erf"                  , 31);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/subqueuefiles.erf"            , 32);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/summonwardog.gpu.rim"         , 33);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/summonwardog.rim"             , 34);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/data/tints.rim"                    , 35);

	status("Loading core sound resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/audio/sound/wwisebanks_core.erf"  , 40);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/audio/sound/wwisestreams_core.erf", 41);

	status("Indexing extra core movie resources");
	indexMandatoryDirectory("packages/core/data/movies" , 0, 0, 101);
	status("Indexing extra core cursors");
	indexMandatoryDirectory("packages/core/data/cursors", 0, 0, 102);
}

void DragonAge2Engine::initCursors() {
}

} // End of namespace DragonAge2

} // End of namespace Engines

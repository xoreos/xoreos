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

#include "graphics/cursorman.h"
#include "graphics/cameraman.h"

#include "graphics/aurora/sceneman.h"
#include "graphics/aurora/fps.h"
#include "graphics/aurora/cube.h"

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

	CursorMan.hideCursor();

	playVideo("dragon_age_ea_logo");
	playVideo("dragon_age_main");

	CursorMan.set();
	CursorMan.showCursor();

	if (EventMan.quitRequested())
		return;

	Graphics::Aurora::FPS *fps = 0;
	if (ConfigMan.getBool("showfps", false))
		fps = new Graphics::Aurora::FPS;

	CameraMan.setPosition(0.0, 0.0, 5.0);
	CameraMan.lookAt(0.0, 0.0, 0.0);

	Graphics::Aurora::Cube *cube = SceneMan.createCube("ach_abi_accomplish_rog");

	cube->setPosition(0.0, 0.0, 0.0);
	cube->startRotate();
	cube->setVisible(true);

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	SceneMan.destroy(cube);

	delete fps;
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
	CursorMan.add("area_transition"            , "area_transition"    , "up"      );
	CursorMan.add("area_transition_pressed"    , "area_transition"    , "down"    );
	CursorMan.add("attack"                     , "attack"             , "up"      );
	CursorMan.add("attack_pressed"             , "attack"             , "down"    );
	CursorMan.add("chest"                      , "chest"              , "up"      );
	CursorMan.add("chest_pressed"              , "chest"              , "down"    );
	CursorMan.add("close"                      , "close"              , "up"      );
	CursorMan.add("close_pressed"              , "close"              , "down"    );
	CursorMan.add("converse"                   , "converse"           , "up"      );
	CursorMan.add("converse_pressed"           , "converse"           , "down"    );
	CursorMan.add("destroy"                    , "destroy"            , "up"      );
	CursorMan.add("destroy_pressed"            , "destroy"            , "down"    );
	CursorMan.add("dialog"                     , "dialog"             , "up"      );
	CursorMan.add("dialog_pressed"             , "dialog"             , "down"    );
	CursorMan.add("disabled_unlock"            , "disabled_unlock"    , "up"      );
	CursorMan.add("disabled_unlock_pressed"    , "disabled_unlock"    , "down"    );
	CursorMan.add("disarm"                     , "disarm"             , "up"      );
	CursorMan.add("disarm_pressed"             , "disarm"             , "down"    );
	CursorMan.add("door"                       , "door"               , "up"      );
	CursorMan.add("door_pressed"               , "door"               , "down"    );
	CursorMan.add("dragging"                   , "dragging"           , ""        );
	CursorMan.add("examine"                    , "examine"            , "up"      );
	CursorMan.add("examine_pressed"            , "examine"            , "down"    );
	CursorMan.add("flip_cover"                 , "flip_cover"         , "up"      );
	CursorMan.add("flip_cover_pressed"         , "flip_cover"         , "down"    );
	CursorMan.add("hand"                       , "hand"               , "up"      );
	CursorMan.add("hand2"                      , "hand"               , "down"    );
	CursorMan.add("hidden_cursor"              , "hidden_cursor"      , ""        );
	CursorMan.add("ibeam"                      , "ibeam"              , "up"      );
	CursorMan.add("ibeam_pressed"              , "ibeam"              , "down"    );
	CursorMan.add("invalid"                    , "invalid"            , "up"      );
	CursorMan.add("invalid_pressed"            , "invalid"            , "down"    );
	CursorMan.add("invalid_move"               , "invalid_move"       , ""        );
	CursorMan.add("open"                       , "open"               , "up"      );
	CursorMan.add("open_pressed"               , "open"               , "down"    );
	CursorMan.add("open_inventory"             , "open_inventory"     , "up"      );
	CursorMan.add("open_inventory_pressed"     , "open_inventory"     , "down"    );
	CursorMan.add("panning"                    , "panning"            , "down"    );
	CursorMan.add("panning1"                   , "panning"            , "up"      );
	CursorMan.add("panning1_h"                 , "panning_h"          , "up"      );
	CursorMan.add("panning1_v"                 , "panning_v"          , "up"      );
	CursorMan.add("panning_h"                  , "panning_h"          , "down"    );
	CursorMan.add("panning_v"                  , "panning_v"          , "down"    );
	CursorMan.add("party_member"               , "party_member"       , ""        );
	CursorMan.add("standard"                   , "standard"           , "up"      );
	CursorMan.add("standard_pressed"           , "standard"           , "down"    );
	CursorMan.add("standard_targeting"         , "standard_targeting" , "up"      );
	CursorMan.add("standard_targeting_pressed" , "standard_targeting" , "down"    );
	CursorMan.add("target"                     , "target"             , "up"      );
	CursorMan.add("target_pressed"             , "target"             , "down"    );
	CursorMan.add("target_inactive"            , "target"             , "inactive");
	CursorMan.add("target_circle"              , "target_circle"      , "up"      );
	CursorMan.add("target_circle_pressed"      , "target_circle"      , "down"    );
	CursorMan.add("target_circle_invalid"      , "target_circle"      , "invalid" );
	CursorMan.add("target_cone"                , "target_cone"        , "up"      );
	CursorMan.add("target_cone_pressed"        , "target_cone"        , "down"    );
	CursorMan.add("target_cone_invalid"        , "target_cone"        , "invalid" );
	CursorMan.add("target_creature"            , "target_creature"    , "up"      );
	CursorMan.add("target_creature_pressed"    , "target_creature"    , "down"    );
	CursorMan.add("target_creature_invalid"    , "target_creature"    , "invalid" );
	CursorMan.add("target_creaturelock"        , "target_creaturelock", "up"      );
	CursorMan.add("target_creaturelock_pressed", "target_creaturelock", "down"    );
	CursorMan.add("topple"                     , "topple"             , "up"      );
	CursorMan.add("topple_pressed"             , "topple"             , "down"    );
	CursorMan.add("trigger_trap"               , "trigger_trap"       , "up"      );
	CursorMan.add("trigger_trap_pressed"       , "trigger_trap"       , "down"    );
	CursorMan.add("unlock"                     , "unlock"             , "up"      );
	CursorMan.add("unlock_pressed"             , "unlock"             , "down"    );
	CursorMan.add("use"                        , "use"                , "up"      );
	CursorMan.add("use_pressed"                , "use"                , "down"    );

	CursorMan.setDefault("standard", "up");
}

} // End of namespace DragonAge

} // End of namespace Engines

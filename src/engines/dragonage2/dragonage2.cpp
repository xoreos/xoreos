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

#include "graphics/cursorman.h"

#include "graphics/aurora/fps.h"

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

	CursorMan.hideCursor();

	playVideo("dragon_age_ea_logo");

	CursorMan.set();
	CursorMan.showCursor();

	if (EventMan.quitRequested())
		return;

	Graphics::Aurora::FPS *fps = 0;
	if (ConfigMan.getBool("showfps", false))
		fps = new Graphics::Aurora::FPS;

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete fps;
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
	CursorMan.add("disabled_disarm"            , "disabled_disarm"    , "up"      );
	CursorMan.add("disabled_disarm_pressed"    , "disabled_disarm"    , "down"    );
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

} // End of namespace DragonAge2

} // End of namespace Engines

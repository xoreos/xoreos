/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Engine class handling Dragon Age II
 */

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/cube.h"
#include "src/graphics/aurora/fontman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage2/dragonage2.h"
#include "src/engines/dragonage2/console.h"

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

bool DragonAge2EngineProbe::probe(const Common::UString &UNUSED(directory),
                                  const Common::FileList &rootFiles) const {

	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains("/dragonage2launcher.exe", true))
		return true;

	return false;
}

bool DragonAge2EngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *DragonAge2EngineProbe::createEngine() const {
	return new DragonAge2Engine;
}


DragonAge2Engine::DragonAge2Engine() {
	_console = new Console(*this);
}

DragonAge2Engine::~DragonAge2Engine() {
}

Common::UString DragonAge2Engine::getLanguageString(Aurora::Language language) {
	switch (language) {
		case Aurora::kLanguageEnglish:
			return "en-us";
		case Aurora::kLanguageFrench:
			return "fr-fr";
		case Aurora::kLanguageGerman:
			return "de-de";
		case Aurora::kLanguageItalian:
			return "it-it";
		case Aurora::kLanguageSpanish:
			return "es-es";
		case Aurora::kLanguagePolish:
			return "pl-pl";
		case Aurora::kLanguageCzech:
			return "cz-cz";
		case Aurora::kLanguageHungarian:
			return "hu-hu";
		case Aurora::kLanguageRussian:
			return "ru-ru";
		case Aurora::kLanguageKorean:
			return "ko-ko";
		case Aurora::kLanguageJapanese:
			return "ja-ja";
		default:
			break;
	}

	return "";
}

bool DragonAge2Engine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                      Aurora::Platform UNUSED(platform),
                                      std::vector<Aurora::Language> &languages) const {
	try {
		Common::UString tlkDir =
			Common::FilePath::findSubDirectory(target, "packages/core/data/talktables", true);

		if (tlkDir.empty())
			return true;

		Common::FileList tlks;
		if (!tlks.addDirectory(tlkDir))
			return true;

		for (uint i = 0; i < Aurora::kLanguageMAX; i++) {
			Common::UString langStr = getLanguageString((Aurora::Language) i);
			if (langStr.empty())
				continue;

			if (!tlks.contains("core_" + langStr + ".tlk", true))
				continue;

			languages.push_back((Aurora::Language) i);
		}

	} catch (...) {
	}

	return true;
}

void DragonAge2Engine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playVideo("dragon_age_ea_logo");
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("xx_wood");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (_console->processEvent(event))
				continue;

			if ((event.key.keysym.sym == SDLK_d) && (event.key.keysym.mod & KMOD_CTRL)) {
				_console->show();
				continue;
			}
		}

		EventMan.delay(10);
	}

	delete cube;
}

void DragonAge2Engine::init() {
	LoadProgress progress(8);

	ResMan.setRIMsAreERFs(true);

	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_target);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures/medium");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures/high");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/audio/sound/");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/patch");

	progress.step("Loading core resource files");
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

	progress.step("Loading core sound resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/audio/sound/wwisebanks_core.erf"  , 40);
	indexMandatoryArchive(Aurora::kArchiveERF, "packages/core/audio/sound/wwisestreams_core.erf", 41);

	progress.step("Indexing extra core movie resources");
	indexMandatoryDirectory("packages/core/data/movies" , 0, 0, 101);
	progress.step("Indexing extra core cursors");
	indexMandatoryDirectory("packages/core/data/cursors", 0, 0, 102);

	progress.step("Loading game cursors");
	initCursors();

	progress.step("Successfully initialized the engine");
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

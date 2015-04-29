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
 *  Engine class handling Dragon Age: Origins
 */

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/cube.h"
#include "src/graphics/aurora/fontman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/language.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/console.h"

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

bool DragonAgeEngineProbe::probe(const Common::UString &UNUSED(directory),
                                 const Common::FileList &rootFiles) const {

	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains("/daoriginslauncher.exe", true))
		return true;

	return false;
}

bool DragonAgeEngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *DragonAgeEngineProbe::createEngine() const {
	return new DragonAgeEngine;
}


DragonAgeEngine::DragonAgeEngine() {
	_console = new Console(*this);
}

DragonAgeEngine::~DragonAgeEngine() {
}

Common::UString DragonAgeEngine::getLanguageString(Aurora::Language language) {
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
			return "cs-cz";
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

bool DragonAgeEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
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

bool DragonAgeEngine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool DragonAgeEngine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

void DragonAgeEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	main();

	deinit();
}

void DragonAgeEngine::init() {
	LoadProgress progress(19);

	if (evaluateLanguage(true, _language))
		status("Setting the language to %s", Aurora::getLanguageName(_language).c_str());
	else
		warning("Failed to detect this game's language");

	progress.step("Loading user game config");
	initConfig();

	progress.step("Declare string encodings");
	declareEncodings();

	initResources(progress);
	if (EventMan.quitRequested())
		return;

	progress.step("Loading game cursors");
	initCursors();
	if (EventMan.quitRequested())
		return;

	progress.step("Initializing internal game config");
	initGameConfig();

	progress.step("Successfully initialized the engine");
}

void DragonAgeEngine::declareEncodings() {
	static const LanguageEncoding kLanguageEncodings[] = {
		{ Aurora::kLanguageEnglish           , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageFrench            , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageGerman            , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageItalian           , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageSpanish           , Common::kEncodingUTF16LE },
		{ Aurora::kLanguagePolish            , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageCzech             , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageHungarian         , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageRussian           , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageKorean            , Common::kEncodingUTF16LE },
		{ Aurora::kLanguageJapanese          , Common::kEncodingUTF16LE }
	};

	Engines::declareEncodings(_game, kLanguageEncodings, ARRAYSIZE(kLanguageEncodings));
}

void DragonAgeEngine::initResources(LoadProgress &progress) {
	ResMan.setRIMsAreERFs(true);

	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_target);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/data/abilities");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/textures");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "packages/core/env");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules/single player/data");

	progress.step("Loading core resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "2da.erf"               , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "anims.erf"             , 11);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.gpu.rim"       , 12);
	indexMandatoryArchive(Aurora::kArchiveERF, "chargen.rim"           , 13);
	indexMandatoryArchive(Aurora::kArchiveERF, "consolescripts.erf"    , 14);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerareas.erf"     , 15);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercreatures.erf" , 16);
	indexMandatoryArchive(Aurora::kArchiveERF, "designercutscenes.erf" , 17);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerdialogs.erf"   , 18);
	indexMandatoryArchive(Aurora::kArchiveERF, "designeritems.erf"     , 19);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplaceables.erf", 20);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerplots.erf"     , 21);
	indexMandatoryArchive(Aurora::kArchiveERF, "designerscripts.rim"   , 22);
	indexMandatoryArchive(Aurora::kArchiveERF, "designertriggers.erf"  , 23);
	indexMandatoryArchive(Aurora::kArchiveERF, "face.erf"              , 24);
	indexMandatoryArchive(Aurora::kArchiveERF, "global.rim"            , 25);
	indexMandatoryArchive(Aurora::kArchiveERF, "globalvfx.rim"         , 26);
	indexMandatoryArchive(Aurora::kArchiveERF, "gui.erf"               , 27);
	indexMandatoryArchive(Aurora::kArchiveERF, "guiexport.erf"         , 28);
	indexMandatoryArchive(Aurora::kArchiveERF, "iterationtests.erf"    , 29);
	indexMandatoryArchive(Aurora::kArchiveERF, "lightprobedata.erf"    , 30);
	indexMandatoryArchive(Aurora::kArchiveERF, "materialobjects.erf"   , 31);
	indexMandatoryArchive(Aurora::kArchiveERF, "materials.erf"         , 32);
	indexMandatoryArchive(Aurora::kArchiveERF, "misc.erf"              , 33);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelhierarchies.erf"  , 34);
	indexMandatoryArchive(Aurora::kArchiveERF, "modelmeshdata.erf"     , 35);
	indexMandatoryArchive(Aurora::kArchiveERF, "pathfindingpatches.erf", 36);
	indexMandatoryArchive(Aurora::kArchiveERF, "postprocesseffects.erf", 37);
	indexMandatoryArchive(Aurora::kArchiveERF, "resmetrics.erf"        , 38);
	indexMandatoryArchive(Aurora::kArchiveERF, "scripts.erf"           , 39);
	indexMandatoryArchive(Aurora::kArchiveERF, "shaders.erf"           , 40);
	indexMandatoryArchive(Aurora::kArchiveERF, "states.erf"            , 41);
	indexMandatoryArchive(Aurora::kArchiveERF, "subqueuefiles.erf"     , 42);
	indexMandatoryArchive(Aurora::kArchiveERF, "textures.erf"          , 43);
	indexMandatoryArchive(Aurora::kArchiveERF, "tints.erf"             , 44);

	progress.step("Loading core ability resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "bearform.rim"    , 100);
	indexMandatoryArchive(Aurora::kArchiveERF, "burningform.rim" , 101);
	indexMandatoryArchive(Aurora::kArchiveERF, "golemform.rim"   , 102);
	indexMandatoryArchive(Aurora::kArchiveERF, "mouseform.rim"   , 103);
	indexMandatoryArchive(Aurora::kArchiveERF, "spiderform.rim"  , 104);
	indexMandatoryArchive(Aurora::kArchiveERF, "spiritform.rim"  , 105);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonbear.rim"  , 106);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonspider.rim", 107);
	indexMandatoryArchive(Aurora::kArchiveERF, "summonwolf.rim"  , 108);

	progress.step("Indexing extra core sound resources");
	indexMandatoryDirectory("packages/core/audio"          , 0, -1, 150);
	progress.step("Indexing extra core movie resources");
	indexMandatoryDirectory("packages/core/data/movies"    , 0,  0, 151);
	progress.step("Indexing extra core talktables");
	indexMandatoryDirectory("packages/core/data/talktables", 0,  0, 152);
	progress.step("Indexing extra core cursors");
	indexMandatoryDirectory("packages/core/data/cursors"   , 0,  0, 153);

	progress.step("Indexing extra environments");
	indexMandatoryDirectory("packages/core/env", 0, 0, 200);

	progress.step("Loading single-player campaign global resource files");
	indexMandatoryArchive(Aurora::kArchiveERF, "moduleglobal.rim", 250);

	progress.step("Indexing extra single-player campaign movie resources");
	indexMandatoryDirectory("modules/single player/data"           , 0, -1, 300);
	progress.step("Indexing extra single-player campaign sound resources");
	indexMandatoryDirectory("modules/single player/data/movies"    , 0,  0, 301);
	progress.step("Indexing extra single-player campaign talktables");
	indexMandatoryDirectory("modules/single player/data/talktables", 0,  0, 302);

	// TODO: DLC

	loadLanguageFiles(progress, _language);
}

void DragonAgeEngine::unloadLanguageFiles() {
	TalkMan.removeMainTable();
}

void DragonAgeEngine::loadLanguageFiles(LoadProgress &progress, Aurora::Language language) {
	progress.step(Common::UString::sprintf("Indexing language files (%s)",
				Aurora::getLanguageName(language).c_str()));

	loadLanguageFiles(language);
}

void DragonAgeEngine::loadLanguageFiles(Aurora::Language language) {
	unloadLanguageFiles();

	const Common::UString tlkM = "core_" + getLanguageString(language);
	const Common::UString tlkF = "core_" + getLanguageString(language) + "_f";

	TalkMan.addMainTable(tlkM, tlkF, Aurora::getLanguageID(_game, language));
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

void DragonAgeEngine::initConfig() {
}

void DragonAgeEngine::initGameConfig() {
}

void DragonAgeEngine::deinit() {
}

void DragonAgeEngine::playIntroVideos() {
	playVideo("dragon_age_ea_logo");
	playVideo("dragon_age_main");
}

void DragonAgeEngine::main() {
	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("ach_abi_accomplish_rog");

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

} // End of namespace DragonAge

} // End of namespace Engines

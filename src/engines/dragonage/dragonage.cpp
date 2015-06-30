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

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/dragonage/dragonage.h"
#include "src/engines/dragonage/modelloader.h"
#include "src/engines/dragonage/console.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"

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


DragonAgeEngine::DragonAgeEngine() : _language(Aurora::kLanguageInvalid),
	_campaigns(0) {

	_console = new Console(*this);
}

DragonAgeEngine::~DragonAgeEngine() {
	delete _campaigns;
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

		for (size_t i = 0; i < Aurora::kLanguageMAX; i++) {
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

Campaigns &DragonAgeEngine::getCampaigns() {
	assert(_campaigns);

	return *_campaigns;
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
	LoadProgress progress(15);

	progress.step("Declare languages");
	declareLanguages();

	if (evaluateLanguage(true, _language))
		status("Setting the language to %s", LangMan.getLanguageName(_language).c_str());
	else
		warning("Failed to detect this game's language");

	progress.step("Loading user game config");
	initConfig();

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

void DragonAgeEngine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageEnglish  ,  0, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageFrench   ,  1, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageGerman   ,  2, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageItalian  ,  3, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageSpanish  ,  4, Common::kEncodingUTF16LE },
		{ Aurora::kLanguagePolish   ,  5, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageCzech    ,  6, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageHungarian,  7, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageRussian  ,  8, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageKorean   ,  9, Common::kEncodingUTF16LE },
		{ Aurora::kLanguageJapanese , 10, Common::kEncodingUTF16LE }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void DragonAgeEngine::initResources(LoadProgress &progress) {
	ResMan.setRIMsAreERFs(true);
	ResMan.setHashAlgo(Common::kHashFNV64);

	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding core archive directories");
	indexMandatoryDirectory("/packages/core/data"           , 0,  0,  2);
	indexMandatoryDirectory("/packages/core/data/abilities" , 0,  0,  3);
	indexMandatoryDirectory("/packages/core/data/movies"    , 0,  0,  4);
	indexMandatoryDirectory("/packages/core/data/talktables", 0,  0,  5);
	indexMandatoryDirectory("/packages/core/data/cursors"   , 0,  0,  6);
	indexMandatoryDirectory("/packages/core/textures"       , 0, -1,  7);
	indexMandatoryDirectory("/packages/core/audio"          , 0, -1,  8);
	indexMandatoryDirectory("/packages/core/env"            , 0, -1,  9);
	indexMandatoryDirectory("/packages/core/patch/"         , 0,  0, 10);

	progress.step("Loading core resource files");
	indexMandatoryArchive("/packages/core/data/2da.erf",                20);
	indexMandatoryArchive("/packages/core/data/anims.erf",              21);
	indexMandatoryArchive("/packages/core/data/chargen.gpu.rim",        22);
	indexMandatoryArchive("/packages/core/data/chargen.rim",            23);
	indexMandatoryArchive("/packages/core/data/consolescripts.erf",     24);
	indexMandatoryArchive("/packages/core/data/designerareas.erf",      25);
	indexMandatoryArchive("/packages/core/data/designercreatures.erf",  26);
	indexMandatoryArchive("/packages/core/data/designercutscenes.erf",  27);
	indexMandatoryArchive("/packages/core/data/designerdialogs.erf",    28);
	indexMandatoryArchive("/packages/core/data/designeritems.erf",      29);
	indexMandatoryArchive("/packages/core/data/designerplaceables.erf", 30);
	indexMandatoryArchive("/packages/core/data/designerplots.erf",      31);
	indexMandatoryArchive("/packages/core/data/designerscripts.rim",    32);
	indexMandatoryArchive("/packages/core/data/designertriggers.erf",   33);
	indexMandatoryArchive("/packages/core/data/face.erf",               34);
	indexMandatoryArchive("/packages/core/data/global.rim",             35);
	indexMandatoryArchive("/packages/core/data/globalvfx.rim",          36);
	indexMandatoryArchive("/packages/core/data/gui.erf",                37);
	indexMandatoryArchive("/packages/core/data/guiexport.erf",          38);
	indexMandatoryArchive("/packages/core/data/iterationtests.erf",     39);
	indexMandatoryArchive("/packages/core/data/lightprobedata.erf",     40);
	indexMandatoryArchive("/packages/core/data/materialobjects.erf",    41);
	indexMandatoryArchive("/packages/core/data/materials.erf",          42);
	indexMandatoryArchive("/packages/core/data/misc.erf",               43);
	indexMandatoryArchive("/packages/core/data/modelhierarchies.erf",   44);
	indexMandatoryArchive("/packages/core/data/modelmeshdata.erf",      45);
	indexMandatoryArchive("/packages/core/data/pathfindingpatches.erf", 46);
	indexMandatoryArchive("/packages/core/data/postprocesseffects.erf", 47);
	indexMandatoryArchive("/packages/core/data/resmetrics.erf",         48);
	indexMandatoryArchive("/packages/core/data/scripts.erf",            49);
	indexMandatoryArchive("/packages/core/data/shaders.erf",            50);
	indexMandatoryArchive("/packages/core/data/states.erf",             51);
	indexMandatoryArchive("/packages/core/data/subqueuefiles.erf",      52);
	indexMandatoryArchive("/packages/core/data/textures.erf",           53);
	indexMandatoryArchive("/packages/core/data/tints.erf",              54);

	progress.step("Loading core ability resource files");
	indexMandatoryArchive("/packages/core/data/abilities/bearform.rim",     100);
	indexMandatoryArchive("/packages/core/data/abilities/burningform.rim",  101);
	indexMandatoryArchive("/packages/core/data/abilities/golemform.rim",    102);
	indexMandatoryArchive("/packages/core/data/abilities/mouseform.rim",    103);
	indexMandatoryArchive("/packages/core/data/abilities/spiderform.rim",   104);
	indexMandatoryArchive("/packages/core/data/abilities/spiritform.rim",   105);
	indexMandatoryArchive("/packages/core/data/abilities/summonbear.rim",   106);
	indexMandatoryArchive("/packages/core/data/abilities/summonspider.rim", 107);
	indexMandatoryArchive("/packages/core/data/abilities/summonwolf.rim",   108);

	progress.step("Loading core patch files");
	loadPatches("/packages/core/patch", 150);

	progress.step("Adding single-player campaign archive directories");
	indexMandatoryDirectory("/modules/single player/data/"          , 0,  0, 200);
	indexMandatoryDirectory("/modules/single player/data/movies"    , 0,  0, 201);
	indexMandatoryDirectory("/modules/single player/data/talktables", 0,  0, 202);
	indexMandatoryDirectory("/modules/single player/audio"          , 0, -1, 203);
	indexMandatoryDirectory("/modules/single player/patch"          , 0,  0, 204);

	progress.step("Loading single-player campaign resource files");
	indexMandatoryArchive("/modules/single player/data/designerareas.erf",        220);
	indexMandatoryArchive("/modules/single player/data/designercreatures.erf",    221);
	indexMandatoryArchive("/modules/single player/data/designercutscenes.erf",    222);
	indexMandatoryArchive("/modules/single player/data/designerdialogs.erf",      223);
	indexMandatoryArchive("/modules/single player/data/designeritems.erf",        224);
	indexMandatoryArchive("/modules/single player/data/designermaps.erf",         225);
	indexMandatoryArchive("/modules/single player/data/designermerchants.erf",    226);
	indexMandatoryArchive("/modules/single player/data/designerplaceables.erf",   227);
	indexMandatoryArchive("/modules/single player/data/designerplots.erf",        228);
	indexMandatoryArchive("/modules/single player/data/designerscripts.erf",      229);
	indexMandatoryArchive("/modules/single player/data/designertriggers.erf",     230);
	indexMandatoryArchive("/modules/single player/data/extradesignerscripts.rim", 231);
	indexMandatoryArchive("/modules/single player/data/moduleglobal.rim",         232);

	progress.step("Loading single-player campaign patch files");
	loadPatches("/modules/single player/patch", 250);

	// TODO: DLC

	loadLanguageFiles(progress, _language);

	progress.step("Registering file formats");
	registerModelLoader(new DragonAgeModelLoader);
}

void DragonAgeEngine::loadPatches(const Common::UString &dir, uint32 priority) {
	Common::FileList patches(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir, true), 0);

	patches.sort(true);
	patches.relativize(ResMan.getDataBase());

	for (Common::FileList::const_iterator p = patches.begin(); p != patches.end(); ++p)
		if (Common::FilePath::getExtension(*p).equalsIgnoreCase(".erf"))
			indexMandatoryArchive("/" + *p, priority++);
}

void DragonAgeEngine::unloadLanguageFiles() {
	for (std::list<Common::ChangeID>::iterator t = _languageTLK.begin(); t != _languageTLK.end(); ++t)
		TalkMan.removeTable(*t);

	_languageTLK.clear();
}

void DragonAgeEngine::loadLanguageFiles(LoadProgress &progress, Aurora::Language language) {
	progress.step(Common::UString::format("Indexing language files (%s)",
				LangMan.getLanguageName(language).c_str()));

	loadLanguageFiles(language);
}

void DragonAgeEngine::loadTalkTable(const Common::UString &tlk, Aurora::Language language, uint32 priority) {
	Common::UString tlkM = tlk + getLanguageString(language);
	Common::UString tlkF = tlk + getLanguageString(language) + "_f";

	_languageTLK.push_back(Common::ChangeID());
	TalkMan.addTable(tlkM, tlkF, false, priority, &_languageTLK.back());
}

void DragonAgeEngine::loadLanguageFiles(Aurora::Language language) {
	unloadLanguageFiles();
	if (EventMan.quitRequested())
		return;

	LangMan.setCurrentLanguage(language);

	loadTalkTable("core_"        , language, 0);
	loadTalkTable("singleplayer_", language, 1);
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
	_campaigns = new Campaigns(*_console);

	const Campaign *singlePlayer = _campaigns->findCampaign("Single Player");
	if (!singlePlayer)
		throw Common::Exception("Can't find the default single player campaign");

	_campaigns->load(*singlePlayer);
	_campaigns->run();

	delete _campaigns;
	_campaigns = 0;
}

} // End of namespace DragonAge

} // End of namespace Engines

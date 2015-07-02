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
	LoadProgress progress(11);

	progress.step("Declare languages");
	declareLanguages();

	if (evaluateLanguage(true, _language))
		status("Setting the language to %s", LangMan.getLanguageName(_language).c_str());
	else
		warning("Failed to detect this game's language");

	LangMan.setCurrentLanguage(_language);

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
		{ Aurora::kLanguageEnglish  ,  0, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench   ,  1, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
		{ Aurora::kLanguageRussian  ,  2, Common::kEncodingUTF16LE, Common::kEncodingCP1251 },
		{ Aurora::kLanguageItalian  ,  3, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman   ,  4, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
		{ Aurora::kLanguagePolish   ,  5, Common::kEncodingUTF16LE, Common::kEncodingCP1250 },
		{ Aurora::kLanguageSpanish  ,  6, Common::kEncodingUTF16LE, Common::kEncodingCP1252 },
		{ Aurora::kLanguageCzech    ,  7, Common::kEncodingUTF16LE, Common::kEncodingCP1250 },
		{ Aurora::kLanguageHungarian,  8, Common::kEncodingUTF16LE, Common::kEncodingCP1250 },
		{ Aurora::kLanguageKorean   ,  9, Common::kEncodingUTF16LE, Common::kEncodingCP949  },
		{ Aurora::kLanguageJapanese , 10, Common::kEncodingUTF16LE, Common::kEncodingCP932  }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void DragonAgeEngine::initResources(LoadProgress &progress) {
	ResMan.setRIMsAreERFs(true);
	ResMan.setHashAlgo(Common::kHashFNV64);

	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding core archive directories");

	progress.step("Indexing core resources files");
	loadResources("/packages/core", 0, _resources, _languageTLK);

	progress.step("Indexing extra core resources files");
	indexMandatoryArchive("/packages/core/data/designerscripts.rim",        450, _resources);
	indexMandatoryArchive("/packages/core/data/globalvfx.rim",              451, _resources);
	indexMandatoryArchive("/packages/core/data/chargen.rim",                452, _resources);
	indexMandatoryArchive("/packages/core/data/chargen.gpu.rim",            453, _resources);
	indexMandatoryArchive("/packages/core/data/global.rim",                 454, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/spiritform.rim",   455, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/summonwolf.rim",   456, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/mouseform.rim",    457, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/summonspider.rim", 458, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/summonbear.rim",   459, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/spiderform.rim",   460, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/golemform.rim",    461, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/bearform.rim",     462, _resources);
	indexMandatoryArchive("/packages/core/data/abilities/burningform.rim",  463, _resources);

	progress.step("Indexing single-player campaign resources files");
	loadResources("/modules/single player", 500, _resources, _languageTLK);

	progress.step("Registering file formats");
	registerModelLoader(new DragonAgeModelLoader);
}

void DragonAgeEngine::loadResources(const Common::UString &dir, uint32 priority,
                                    ChangeList &res, ChangeList &tlk) {

	indexOptionalDirectory(dir + "/data"           , 0,  0, priority + 10, res);
	indexOptionalDirectory(dir + "/data/abilities" , 0,  0, priority + 11, res);
	indexOptionalDirectory(dir + "/data/movies"    , 0,  0, priority + 12, res);
	indexOptionalDirectory(dir + "/data/talktables", 0,  0, priority + 13, res);
	indexOptionalDirectory(dir + "/data/cursors"   , 0,  0, priority + 14, res);
	indexOptionalDirectory(dir + "/textures"       , 0, -1, priority + 15, res);
	indexOptionalDirectory(dir + "/audio"          , 0, -1, priority + 16, res);
	indexOptionalDirectory(dir + "/env"            , 0, -1, priority + 17, res);
	indexOptionalDirectory(dir + "/patch"          , 0,  0, priority + 18, res);

	loadResourceDir(dir + "/data"          , priority + 100, res);
	loadResourceDir(dir + "/data/abilities", priority + 200, res);

	loadResourceDir(dir + "/patch", 0x40000000 | priority, res);

	indexOptionalDirectory(dir + "/override", 0, -1, 0x40000000 | (priority + 499), res);

	loadTalkTables(dir + "/data/talktables", priority, tlk);
}

void DragonAgeEngine::loadTexturePack(const Common::UString &dir, uint32 priority,
                                      ChangeList &res, TextureQuality quality) {

	static const char *kTextureQualityName[kTextureQualityMAX] = { "high", "medium" };

	if (((uint)quality) >= kTextureQualityMAX)
		throw Common::Exception("Invalid texture quality level");

	loadResourceDir(dir + "/textures/" + kTextureQualityName[quality], priority + 300, res);
}

void DragonAgeEngine::loadTalkTables(const Common::UString &dir, uint32 priority, ChangeList &changes) {
	if (EventMan.quitRequested())
		return;

	const Common::UString tlkDir = Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir, true);

	Common::FileList files(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir, true), 0);

	files.sort(true);
	files.relativize(tlkDir);

	const Common::UString languageTLK = getLanguageString(_language) + ".tlk";
	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f) {
		if (!f->toLower().endsWith(languageTLK))
			continue;

		Common::UString tlk = *f;
		tlk.truncate(tlk.size() - languageTLK.size());

		loadTalkTable(tlk, _language, priority++, changes);
	}
}

void DragonAgeEngine::loadResourceDir(const Common::UString &dir, uint32 priority, ChangeList &changes) {
	if (EventMan.quitRequested())
		return;

	Common::FileList files(Common::FilePath::findSubDirectory(ResMan.getDataBase(), dir, true), 0);

	files.sort(true);
	files.relativize(ResMan.getDataBase());

	for (Common::FileList::const_iterator f = files.begin(); f != files.end(); ++f)
		if (Common::FilePath::getExtension(*f).equalsIgnoreCase(".erf"))
			indexMandatoryArchive("/" + *f, priority++, changes);
}

void DragonAgeEngine::unloadTalkTables(ChangeList &changes) {
	for (std::list<Common::ChangeID>::iterator t = changes.begin(); t != changes.end(); ++t)
		TalkMan.removeTable(*t);

	changes.clear();
}

void DragonAgeEngine::loadTalkTable(const Common::UString &tlk, Aurora::Language language,
                                    uint32 priority, ChangeList &changes) {

	Common::UString tlkM = tlk + getLanguageString(language);
	Common::UString tlkF = tlk + getLanguageString(language) + "_f";

	changes.push_back(Common::ChangeID());
	TalkMan.addTable(tlkM, tlkF, false, priority, &changes.back());
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
	unloadTalkTables(_languageTLK);
	deindexResources(_resources);
}

void DragonAgeEngine::playIntroVideos() {
	playVideo("dragon_age_ea_logo");
	playVideo("dragon_age_main");
}

void DragonAgeEngine::main() {
	_campaigns = new Campaigns(*_console, *this);

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

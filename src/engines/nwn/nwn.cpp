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
 *  Engine class handling Neverwinter Nights.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/util.h"
#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/talktable_tlk.h"

#include "src/events/events.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/tokenman.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/version.h"
#include "src/engines/nwn/modelloader.h"
#include "src/engines/nwn/console.h"
#include "src/engines/nwn/game.h"

namespace Engines {

namespace NWN {

NWNEngine::NWNEngine() : _language(Aurora::kLanguageInvalid),
	_hasXP1(false), _hasXP2(false), _hasXP3(false) {

	_console = std::make_unique<Console>(*this);
}

NWNEngine::~NWNEngine() {
}

bool NWNEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                Aurora::Platform UNUSED(platform),
                                std::vector<Aurora::Language> &languages) const {
	try {
		Common::FileList files;
		if (!files.addDirectory(target))
			return true;

		Common::UString tlk = files.findFirst("dialog.tlk", true);
		if (tlk.empty())
			return true;

		uint32_t languageID = Aurora::TalkTable_TLK::getLanguageID(tlk);
		if (languageID == Aurora::kLanguageInvalid)
			return true;

		Aurora::Language language = LangMan.getLanguage(languageID);
		if (language == Aurora::kLanguageInvalid)
			return true;

		languages.push_back(language);

	} catch (...) {
	}

	return true;
}

bool NWNEngine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool NWNEngine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

Game &NWNEngine::getGame() {
	assert(_game);

	return *_game;
}

void NWNEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	_game = std::make_unique<Game>(*this, *_console, *_version);
	_game->run();

	deinit();
}

void NWNEngine::init() {
	LoadProgress progress(20);

	progress.step("Declare languages");
	declareLanguages();

	progress.step("Detecting game version");
	detectVersion();

	if (evaluateLanguage(true, _language))
		status("Setting the language to %s", LangMan.getLanguageName(_language).c_str());
	else
		warning("Failed to detect this game's language");

	LangMan.setCurrentLanguage(_language);

	progress.step("Loading user game config");
	initConfig();
	checkConfig();

	if (EventMan.quitRequested())
		return;

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

void NWNEngine::detectVersion() {
	_version = std::make_unique<Version>(_platform);

	if (_version->detect(_target)) {
		status("This is Neverwinter Nights %s v%s",
		       _version->getPlatformName().c_str(), _version->getVersionString().c_str());

		if        (_version->isTooOld()) {
			warning("Your version of Neverwinter Nights is too old");
			warning("Please update to v%s for optimal support", _version->getOptimumVersionString().c_str());
		} else if (_version->isTooNew()) {
			warning("Your version of Neverwinter Nights is too new!?");
			warning("Please contact us with detailed information about your version");
		}

	} else {
		warning("Failed to detect the patch version of your Neverwinter Nights installation");
	}
}

void NWNEngine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageEnglish           ,   0, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench            ,   1, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman            ,   2, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageItalian           ,   3, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageSpanish           ,   4, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguagePolish            ,   5, Common::kEncodingCP1250, Common::kEncodingCP1250 },
		{ Aurora::kLanguageKorean            , 128, Common::kEncodingCP949 , Common::kEncodingCP949  },
		{ Aurora::kLanguageChineseTraditional, 129, Common::kEncodingCP950 , Common::kEncodingCP950  },
		{ Aurora::kLanguageChineseSimplified , 130, Common::kEncodingCP936 , Common::kEncodingCP936  },
		{ Aurora::kLanguageJapanese          , 131, Common::kEncodingCP932 , Common::kEncodingCP932  }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void NWNEngine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding extra archive directories");
	indexMandatoryDirectory("data"        , 0, 0, 2);
	indexMandatoryDirectory("nwm"         , 0, 0, 3);
	indexMandatoryDirectory("modules"     , 0, 0, 4);
	indexMandatoryDirectory("hak"         , 0, 0, 5);
	indexMandatoryDirectory("texturepacks", 0, 0, 6);

	progress.step("Loading main KEY");
	indexMandatoryArchive("chitin.key", 10);

	progress.step("Loading expansions and patch KEYs");

	// Base game patch
	indexOptionalArchive("patch.key", 11);

	// Expansion 1: Shadows of Undrentide (SoU)
	_hasXP1 = ResMan.hasArchive("xp1.key");
	indexOptionalArchive("xp1.key"     , 12);
	indexOptionalArchive("xp1patch.key", 13);

	// Expansion 2: Hordes of the Underdark (HotU)
	_hasXP2 = ResMan.hasArchive("xp2.key");
	indexOptionalArchive("xp2.key"     , 14);
	indexOptionalArchive("xp2patch.key", 15);

	// Expansion 3: Kingmaker (resources also included in the final 1.69 patch)
	_hasXP3 = ResMan.hasArchive("xp3.key");
	indexOptionalArchive("xp3.key"     , 16);
	indexOptionalArchive("xp3patch.key", 17);

	progress.step("Loading GUI textures");
	indexMandatoryArchive("gui_32bit.erf", 50);
	indexOptionalArchive ("xp1_gui.erf"  , 51);
	indexOptionalArchive ("xp2_gui.erf"  , 52);

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("ambient"   , 0, 0, 100);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("music"     , 0, 0, 101);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"    , 0, 0, 102);
	progress.step("Indexing extra image resources");
	indexOptionalDirectory ("portraits" , 0, 0, 103);
	progress.step("Indexing extra talktables");
	indexOptionalDirectory ("tlk"       , 0, 0, 105);
	progress.step("Indexing databases");
	indexOptionalDirectory ("database"  , 0, 0, 106);

	// Texture packs at 400-403, in module.cpp

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 500);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	progress.step("Registering file formats");
	registerModelLoader(new NWNModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);

	// Blacklist the DDS version of the galahad14 font, because in versions of NWN coming
	// with a Cyrillic one, the DDS file is still Latin.
	ResMan.blacklist("fnt_galahad14", Aurora::kFileTypeDDS);

	declareBogusTextures();
}

void NWNEngine::declareBogusTextures() {
	static const char * const kBogusTextures[] = {
		"belt_g",
		"FB1_g",
		"head_g",
		"Lbicep_g",
		"lbicep_g",
		"lfoot_g",
		"lforearm_g",
		"lhand_g",
		"lshin_g",
		"Lshoulder_g",
		"lshoulder_g",
		"lthigh_g",
		"Material",
		"neck_g",
		"pelvis_g",
		"pmh0_head001g",
		"Rbicep_g",
		"rbicep_g",
		"rfoot_g",
		"rforearm_g",
		"rhand_g",
		"rshin_g",
		"Rshoulder_g",
		"rshoulder_g",
		"rthigh_g",
		"TF3_g",
		"torso_g"
	};

	for (size_t i = 0; i < ARRAYSIZE(kBogusTextures); i++)
		TextureMan.addBogusTexture(kBogusTextures[i]);
}

void NWNEngine::initCursors() {
	CursorMan.add("gui_mp_defaultd" , "default", "down");
	CursorMan.add("gui_mp_defaultu" , "default", "up"  );

	CursorMan.add("gui_mp_actiond"  , "action"   , "down");
	CursorMan.add("gui_mp_actionu"  , "action"   , "up"  );
	CursorMan.add("gui_mp_attackd"  , "attack"   , "down");
	CursorMan.add("gui_mp_attacku"  , "attack"   , "up"  );
	CursorMan.add("gui_mp_created"  , "create"   , "down");
	CursorMan.add("gui_mp_createu"  , "create"   , "up"  );
	CursorMan.add("gui_mp_disarmd"  , "disarm"   , "down");
	CursorMan.add("gui_mp_disarmu"  , "disarm"   , "up"  );
	CursorMan.add("gui_mp_doord"    , "door"     , "down");
	CursorMan.add("gui_mp_dooru"    , "door"     , "up"  );
	CursorMan.add("gui_mp_examined" , "examine"  , "down");
	CursorMan.add("gui_mp_examineu" , "examine"  , "up"  );
	CursorMan.add("gui_mp_followd"  , "follow"   , "down");
	CursorMan.add("gui_mp_followu"  , "follow"   , "up"  );
	CursorMan.add("gui_mp_heald"    , "heal"     , "down");
	CursorMan.add("gui_mp_healu"    , "heal"     , "up"  );
	CursorMan.add("gui_mp_killd"    , "kill"     , "down");
	CursorMan.add("gui_mp_killu"    , "kill"     , "up"  );
	CursorMan.add("gui_mp_lockd"    , "lock"     , "down");
	CursorMan.add("gui_mp_locku"    , "lock"     , "up"  );
	CursorMan.add("gui_mp_magicd"   , "magic"    , "down");
	CursorMan.add("gui_mp_magicu"   , "magic"    , "up"  );
	CursorMan.add("gui_mp_pickupd"  , "pickup"   , "down");
	CursorMan.add("gui_mp_pickupu"  , "pickup"   , "up"  );
	CursorMan.add("gui_mp_pushpind" , "pushpin"  , "down");
	CursorMan.add("gui_mp_pushpinu" , "pushpin"  , "up"  );
	CursorMan.add("gui_mp_talkd"    , "talk"     , "down");
	CursorMan.add("gui_mp_talku"    , "talk"     , "up"  );
	CursorMan.add("gui_mp_transd"   , "trans"    , "down");
	CursorMan.add("gui_mp_transu"   , "trans"    , "up"  );
	CursorMan.add("gui_mp_used"     , "use"      , "down");
	CursorMan.add("gui_mp_useu"     , "use"      , "up"  );
	CursorMan.add("gui_mp_walkd"    , "walk"     , "down");
	CursorMan.add("gui_mp_walku"    , "walk"     , "up"  );

	CursorMan.add("gui_mp_noactiond", "noaction" , "down");
	CursorMan.add("gui_mp_noactionu", "noaction" , "up"  );
	CursorMan.add("gui_mp_noatckd"  , "noattack" , "down");
	CursorMan.add("gui_mp_noatcku"  , "noattack" , "up"  );
	CursorMan.add("gui_mp_nocreatd" , "nocreate" , "down");
	CursorMan.add("gui_mp_nocreatu" , "nocreate" , "up"  );
	CursorMan.add("gui_mp_nodisarmd", "nodisarm" , "down");
	CursorMan.add("gui_mp_nodisarmu", "nodisarm" , "up"  );
	CursorMan.add("gui_mp_noexamd"  , "noexamine", "down");
	CursorMan.add("gui_mp_noexamu"  , "noexamine", "up"  );
	CursorMan.add("gui_mp_noheald"  , "noheal"   , "down");
	CursorMan.add("gui_mp_nohealu"  , "noheal"   , "up"  );
	CursorMan.add("gui_mp_nokilld"  , "nokill"   , "down");
	CursorMan.add("gui_mp_nokillu"  , "nokill"   , "up"  );
	CursorMan.add("gui_mp_nolockd"  , "nolock"   , "down");
	CursorMan.add("gui_mp_nolocku"  , "nolock"   , "up"  );
	CursorMan.add("gui_mp_nomagicd" , "nomagic"  , "down");
	CursorMan.add("gui_mp_nomagicu" , "nomagic"  , "up"  );
	CursorMan.add("gui_mp_notalkd"  , "notalk"   , "down");
	CursorMan.add("gui_mp_notalku"  , "notalk"   , "up"  );
	CursorMan.add("gui_mp_noused"   , "nouse"    , "down");
	CursorMan.add("gui_mp_nouseu"   , "nouse"    , "up"  );
	CursorMan.add("gui_mp_nowalkd"  , "nowalk"   , "down");
	CursorMan.add("gui_mp_nowalku"  , "nowalk"   , "up"  );

	CursorMan.add("gui_mp_arwalk00", "arrowwalk", "N");
	CursorMan.add("gui_mp_arwalk01", "arrowwalk", "NNE");
	CursorMan.add("gui_mp_arwalk02", "arrowwalk", "NE");
	CursorMan.add("gui_mp_arwalk03", "arrowwalk", "ENE");
	CursorMan.add("gui_mp_arwalk04", "arrowwalk", "E");
	CursorMan.add("gui_mp_arwalk05", "arrowwalk", "ESE");
	CursorMan.add("gui_mp_arwalk06", "arrowwalk", "SE");
	CursorMan.add("gui_mp_arwalk07", "arrowwalk", "SSE");
	CursorMan.add("gui_mp_arwalk08", "arrowwalk", "S");
	CursorMan.add("gui_mp_arwalk09", "arrowwalk", "SSW");
	CursorMan.add("gui_mp_arwalk10", "arrowwalk", "SW");
	CursorMan.add("gui_mp_arwalk11", "arrowwalk", "WSW");
	CursorMan.add("gui_mp_arwalk12", "arrowwalk", "W");
	CursorMan.add("gui_mp_arwalk13", "arrowwalk", "WNW");
	CursorMan.add("gui_mp_arwalk14", "arrowwalk", "NW");
	CursorMan.add("gui_mp_arwalk15", "arrowwalk", "NNW");

	CursorMan.add("gui_mp_arrun00", "arrowrun", "N");
	CursorMan.add("gui_mp_arrun01", "arrowrun", "NNE");
	CursorMan.add("gui_mp_arrun02", "arrowrun", "NE");
	CursorMan.add("gui_mp_arrun03", "arrowrun", "ENE");
	CursorMan.add("gui_mp_arrun04", "arrowrun", "E");
	CursorMan.add("gui_mp_arrun05", "arrowrun", "ESE");
	CursorMan.add("gui_mp_arrun06", "arrowrun", "SE");
	CursorMan.add("gui_mp_arrun07", "arrowrun", "SSE");
	CursorMan.add("gui_mp_arrun08", "arrowrun", "S");
	CursorMan.add("gui_mp_arrun09", "arrowrun", "SSW");
	CursorMan.add("gui_mp_arrun10", "arrowrun", "SW");
	CursorMan.add("gui_mp_arrun11", "arrowrun", "WSW");
	CursorMan.add("gui_mp_arrun12", "arrowrun", "W");
	CursorMan.add("gui_mp_arrun13", "arrowrun", "WNW");
	CursorMan.add("gui_mp_arrun14", "arrowrun", "NW");
	CursorMan.add("gui_mp_arrun15", "arrowrun", "NNW");

	CursorMan.setDefault("default", "up");
}

void NWNEngine::initConfig() {
	ConfigMan.setInt(Common::kConfigRealmDefault, "menufogcount" ,   4);
	ConfigMan.setInt(Common::kConfigRealmDefault, "texturepack"  ,   1);
	ConfigMan.setInt(Common::kConfigRealmDefault, "difficulty"   ,   0);
	ConfigMan.setInt(Common::kConfigRealmDefault, "feedbackmode" ,   2);
	ConfigMan.setInt(Common::kConfigRealmDefault, "tooltipdelay" , 100);

	ConfigMan.setBool(Common::kConfigRealmDefault, "largefonts"       , false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "mouseoverfeedback", true);
}

void NWNEngine::initGameConfig() {
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP1", _hasXP1);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP2", _hasXP2);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP3", _hasXP3);

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_extraModuleDir",
		Common::FilePath::findSubDirectory(_target, "modules", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_campaignDir",
		Common::FilePath::findSubDirectory(_target, "nwm", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_localPCDir",
		Common::FilePath::findSubDirectory(_target, "localvault", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_serverPCDir",
		Common::FilePath::findSubDirectory(_target, "servervault", true));

	TokenMan.set("<StartCheck>"    , "<cFF0000FF>");
	TokenMan.set("<StartAction>"   , "<c00FF00FF>");
	TokenMan.set("<StartHighlight>", "<c0000FFFF>");
	TokenMan.set("</Start>"        , "</c>");

	// TODO: <PlayerName>
}

void NWNEngine::checkConfig() {
	checkConfigInt("menufogcount" ,   0,    5);
	checkConfigInt("texturepack"  ,   0,    3);
	checkConfigInt("difficulty"   ,   0,    3);
	checkConfigInt("feedbackmode" ,   0,    2);
	checkConfigInt("tooltipdelay" , 100, 2700);
}

void NWNEngine::deinit() {
	unregisterModelLoader();

	_version.reset();
	_game.reset();
}

void NWNEngine::playIntroVideos() {
	playVideo("atarilogo");
	playVideo("biowarelogo");
	playVideo("wotclogo");
	playVideo("fge_logo_black");
	playVideo("nwnintro");
}

} // End of namespace NWN

} // End of namespace Engines

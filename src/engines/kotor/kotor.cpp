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
 *  Engine class handling Star Wars: Knights of the Old Republic.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/talktable_tlk.h"

#include "src/events/events.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/kotor/kotor.h"
#include "src/engines/kotor/version.h"
#include "src/engines/kotor/modelloader.h"
#include "src/engines/kotor/console.h"
#include "src/engines/kotor/game.h"

namespace Engines {

namespace KotOR {

KotOREngine::KotOREngine() : _language(Aurora::kLanguageInvalid), _hasLiveKey(false) {

	_console = std::make_unique<Console>(*this);
}

KotOREngine::~KotOREngine() {
}

bool KotOREngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
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

bool KotOREngine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool KotOREngine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

void KotOREngine::run() {
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

void KotOREngine::init() {
	LoadProgress progress(19);

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

void KotOREngine::detectVersion() {
	_version = std::make_unique<Version>(_platform);

	if (_version->detect(_target)) {
		status("This is Star Wars: Knights of the Old Republic %s v%s",
		       _version->getPlatformName().c_str(), _version->getVersionString().c_str());

		if        (_version->isTooOld()) {
			warning("Your version of Star Wars: Knights of the Old Republic is too old");
			warning("Please update to v%s for optimal support", _version->getOptimumVersionString().c_str());
		} else if (_version->isTooNew()) {
			warning("Your version of Star Wars: Knights of the Old Republic is too new!?");
			warning("Please contact us with detailed information about your version");
		}

	} else {
		warning("Failed to detect the patch version of your Star Wars: Knights of the Old Republic installation");
	}
}

void KotOREngine::declareLanguages() {
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

void KotOREngine::initResources(LoadProgress &progress) {
	// In the Xbox version of KotOR, TXB textures are actually TPCs
	if (_platform == Aurora::kPlatformXbox)
		ResMan.addTypeAlias(Aurora::kFileTypeTXB, Aurora::kFileTypeTPC);

	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding extra archive directories");
	const Common::UString dataDir = (_platform == Aurora::kPlatformXbox) ? "dataxbox" : "data";
	const Common::UString rimsDir = (_platform == Aurora::kPlatformXbox) ? "rimsxbox" : "rims";

	indexMandatoryDirectory( dataDir , 0, 0, 2);
	indexMandatoryDirectory("lips"   , 0, 0, 3);
	indexMandatoryDirectory("modules", 0, 0, 4);
	indexMandatoryDirectory( rimsDir , 0, 0, 5);

	if (_platform != Aurora::kPlatformXbox)
		indexMandatoryDirectory("texturepacks", 0, 0, 6);

	progress.step("Loading main KEY");
	indexMandatoryArchive("chitin.key", 10);

	if (indexOptionalArchive("live1.key", 11))
		_hasLiveKey = true;

	progress.step("Loading global auxiliary resources");
	indexMandatoryArchive("mainmenu.rim"    , 50);
	indexMandatoryArchive("mainmenudx.rim"  , 51);
	indexMandatoryArchive("legal.rim"       , 52);
	indexMandatoryArchive("legaldx.rim"     , 53);
	indexMandatoryArchive("global.rim"      , 54);
	indexMandatoryArchive("subglobaldx.rim" , 55);
	indexMandatoryArchive("miniglobaldx.rim", 56);
	indexMandatoryArchive("globaldx.rim"    , 57);
	indexMandatoryArchive("chargen.rim"     , 58);
	indexMandatoryArchive("chargendx.rim"   , 59);

	if (_platform == Aurora::kPlatformXbox) {
		// The Xbox version has most of its textures in "textures.bif"
		// Some, however, reside in "players.erf"
		progress.step("Loading Xbox textures");
		indexMandatoryArchive("players.erf", 60);
	} else {
		// The Windows/Mac versions have the GUI textures here
		progress.step("Loading GUI textures");
		indexMandatoryArchive("swpc_tex_gui.erf", 60);
	}

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 100);
	progress.step("Indexing extra voice resources");
	indexMandatoryDirectory("streamwaves" , 0, -1, 101);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 102);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 103);

	if (_platform == Aurora::kPlatformWindows) {
		progress.step("Indexing Windows-specific resources");
		initCursorsRemap();
		indexMandatoryArchive("swkotor.exe", 154);
	} else if (_platform == Aurora::kPlatformMacOSX) {
		progress.step("Indexing Mac-specific resources");
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources",         0, -1, 154);
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources/Cursors", 0, -1, 155);
	} else if (_platform == Aurora::kPlatformXbox) {
		progress.step("Indexing Xbox-specific resources");
		indexMandatoryDirectory("errortex"  , 0, -1, 154);
		indexMandatoryDirectory("localvault", 0, -1, 155);
		indexMandatoryDirectory("media"     , 0, -1, 156);

		// For the DLC, we need to index the "sound" directory as well
		if (_hasLiveKey)
			indexMandatoryDirectory("sound", 0, -1, 157);
	}

	// Texture packs at 400, in module.cpp

	progress.step("Indexing override files");
	indexOptionalArchive("patch.erf", 499);
	indexOptionalDirectory("override", 0, 0, 500);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	if (_hasLiveKey)
		TalkMan.addTable("live1", "live1f", true, 0);

	progress.step("Registering file formats");
	registerModelLoader(new KotORModelLoader(_platform == Aurora::kPlatformXbox));
	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);

	if (ResMan.hasResource("fnt_d16x16b", Aurora::kResourceImage))
		FontMan.addAlias("fnt_d16x16", "fnt_d16x16b");
}

void KotOREngine::initCursorsRemap() {
	std::vector<Common::UString> cursorRemap;

	cursorRemap.push_back("gui_mp_defaultu");
	cursorRemap.push_back("gui_mp_defaultd");
	cursorRemap.push_back("gui_mp_walku");
	cursorRemap.push_back("gui_mp_walkd");
	cursorRemap.push_back("gui_mp_invalidu");
	cursorRemap.push_back("gui_mp_invalidd");
	cursorRemap.push_back("gui_mp_bashu");
	cursorRemap.push_back("gui_mp_bashd");
	cursorRemap.push_back("gui_mp_bashup");
	cursorRemap.push_back("gui_mp_bashdp");
	cursorRemap.push_back("gui_mp_talku");
	cursorRemap.push_back("gui_mp_talkd");
	cursorRemap.push_back("gui_mp_notalku");
	cursorRemap.push_back("gui_mp_notalkd");
	cursorRemap.push_back("gui_mp_followu");
	cursorRemap.push_back("gui_mp_followd");
	cursorRemap.push_back("gui_mp_examineu");
	cursorRemap.push_back("gui_mp_examined");
	cursorRemap.push_back("gui_mp_noexamu");
	cursorRemap.push_back("gui_mp_noexamd");
	cursorRemap.push_back("gui_mp_transu");
	cursorRemap.push_back("gui_mp_transd");
	cursorRemap.push_back("gui_mp_dooru");
	cursorRemap.push_back("gui_mp_doord");
	cursorRemap.push_back("gui_mp_useu");
	cursorRemap.push_back("gui_mp_used");
	cursorRemap.push_back("gui_mp_useup");
	cursorRemap.push_back("gui_mp_usedp");
	cursorRemap.push_back("gui_mp_magicu");
	cursorRemap.push_back("gui_mp_magicd");
	cursorRemap.push_back("gui_mp_nomagicu");
	cursorRemap.push_back("gui_mp_nomagicd");
	cursorRemap.push_back("gui_mp_dismineu");
	cursorRemap.push_back("gui_mp_dismined");
	cursorRemap.push_back("gui_mp_dismineup");
	cursorRemap.push_back("gui_mp_disminedp");
	cursorRemap.push_back("gui_mp_recmineu");
	cursorRemap.push_back("gui_mp_recmined");
	cursorRemap.push_back("gui_mp_recmineup");
	cursorRemap.push_back("gui_mp_recminedp");
	cursorRemap.push_back("gui_mp_locku");
	cursorRemap.push_back("gui_mp_lockd");
	cursorRemap.push_back("gui_mp_doorup");
	cursorRemap.push_back("gui_mp_doordp");
	cursorRemap.push_back("gui_mp_selectu");
	cursorRemap.push_back("gui_mp_selectd");
	cursorRemap.push_back("gui_mp_createu");
	cursorRemap.push_back("gui_mp_created");
	cursorRemap.push_back("gui_mp_nocreatu");
	cursorRemap.push_back("gui_mp_nocreatd");
	cursorRemap.push_back("gui_mp_killu");
	cursorRemap.push_back("gui_mp_killd");
	cursorRemap.push_back("gui_mp_nokillu");
	cursorRemap.push_back("gui_mp_nokilld");
	cursorRemap.push_back("gui_mp_healu");
	cursorRemap.push_back("gui_mp_heald");
	cursorRemap.push_back("gui_mp_nohealu");
	cursorRemap.push_back("gui_mp_noheald");
	cursorRemap.push_back("gui_mp_arrun00");
	cursorRemap.push_back("gui_mp_arrun01");
	cursorRemap.push_back("gui_mp_arrun02");
	cursorRemap.push_back("gui_mp_arrun03");
	cursorRemap.push_back("gui_mp_arrun04");
	cursorRemap.push_back("gui_mp_arrun05");
	cursorRemap.push_back("gui_mp_arrun06");
	cursorRemap.push_back("gui_mp_arrun07");
	cursorRemap.push_back("gui_mp_arrun08");
	cursorRemap.push_back("gui_mp_arrun09");
	cursorRemap.push_back("gui_mp_arrun10");
	cursorRemap.push_back("gui_mp_arrun11");
	cursorRemap.push_back("gui_mp_arrun12");
	cursorRemap.push_back("gui_mp_arrun13");
	cursorRemap.push_back("gui_mp_arrun14");
	cursorRemap.push_back("gui_mp_arrun15");
	cursorRemap.push_back("gui_mp_arwalk00");
	cursorRemap.push_back("gui_mp_arwalk01");
	cursorRemap.push_back("gui_mp_arwalk02");
	cursorRemap.push_back("gui_mp_arwalk03");
	cursorRemap.push_back("gui_mp_arwalk04");
	cursorRemap.push_back("gui_mp_arwalk05");
	cursorRemap.push_back("gui_mp_arwalk06");
	cursorRemap.push_back("gui_mp_arwalk07");
	cursorRemap.push_back("gui_mp_arwalk08");
	cursorRemap.push_back("gui_mp_arwalk09");
	cursorRemap.push_back("gui_mp_arwalk10");
	cursorRemap.push_back("gui_mp_arwalk11");
	cursorRemap.push_back("gui_mp_arwalk12");
	cursorRemap.push_back("gui_mp_arwalk13");
	cursorRemap.push_back("gui_mp_arwalk14");
	cursorRemap.push_back("gui_mp_arwalk15");
	cursorRemap.push_back("gui_mp_pickupu");
	cursorRemap.push_back("gui_mp_pickupd");

	ResMan.setCursorRemap(cursorRemap);
}

void KotOREngine::initCursors() {
	if (_platform == Aurora::kPlatformXbox)
		return;

	CursorMan.add("gui_mp_defaultd" , "default"  , "down");
	CursorMan.add("gui_mp_defaultu" , "default"  , "up"  );

	CursorMan.add("gui_mp_bashd"    , "bash"     , "down");
	CursorMan.add("gui_mp_bashu"    , "bash"     , "up"  );
	CursorMan.add("gui_mp_bashdp"   , "bash+"    , "down");
	CursorMan.add("gui_mp_bashup"   , "bash+"    , "up"  );
	CursorMan.add("gui_mp_dismined" , "dismine"  , "down");
	CursorMan.add("gui_mp_dismineu" , "dismine"  , "up"  );
	CursorMan.add("gui_mp_disminedp", "dismine+" , "down");
	CursorMan.add("gui_mp_dismineup", "dismine+" , "up"  );
	CursorMan.add("gui_mp_doord"    , "door"     , "down");
	CursorMan.add("gui_mp_dooru"    , "door"     , "up"  );
	CursorMan.add("gui_mp_doordp"   , "door+"    , "down");
	CursorMan.add("gui_mp_doorup"   , "door+"    , "up"  );
	CursorMan.add("gui_mp_invalidd" , "invalid"  , "down");
	CursorMan.add("gui_mp_invalidu" , "invalid"  , "up"  );
	CursorMan.add("gui_mp_killd"    , "kill"     , "down");
	CursorMan.add("gui_mp_killu"    , "kill"     , "up"  );
	CursorMan.add("gui_mp_recmined" , "recmine"  , "down");
	CursorMan.add("gui_mp_recmineu" , "recmine"  , "up"  );
	CursorMan.add("gui_mp_recminedp", "recmine+" , "down");
	CursorMan.add("gui_mp_recmineup", "recmine+" , "up"  );
	CursorMan.add("gui_mp_selectd"  , "select"   , "down");
	CursorMan.add("gui_mp_selectu"  , "select"   , "up"  );
	CursorMan.add("gui_mp_talkd"    , "talk"     , "down");
	CursorMan.add("gui_mp_talku"    , "talk"     , "up"  );
	CursorMan.add("gui_mp_used"     , "use"      , "down");
	CursorMan.add("gui_mp_useu"     , "use"      , "up"  );
	CursorMan.add("gui_mp_usedp"    , "use+"     , "down");
	CursorMan.add("gui_mp_useup"    , "use+"     , "up"  );

	CursorMan.setDefault("default", "up");
}

void KotOREngine::initConfig() {
	// Gameplay

	ConfigMan.setInt(Common::kConfigRealmDefault, "difficulty", 1);
	ConfigMan.setBool(Common::kConfigRealmDefault, "autolevelup", false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "mousemove", false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "autosave", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "reverseminigameyaxis", false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "combatmovement", true);

	// Gameplay -> Mouse settings

	ConfigMan.setBool(Common::kConfigRealmDefault, "reversemousebuttons", false);

	// Gameplay -> Key mapping

	// Feedback

	// Auto-pause

	ConfigMan.setBool(Common::kConfigRealmDefault, "endofcombatround", false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "enemysighted", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "minesighted", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "partymemberdown", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "actionmenuused", false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "newtargetselected", true);

	// Graphics

	ConfigMan.setBool(Common::kConfigRealmDefault, "shadows", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "grass", true);

	// Graphics -> Advanced options

	ConfigMan.setInt(Common::kConfigRealmDefault, "texturepack", 2);
	ConfigMan.setInt(Common::kConfigRealmDefault, "antialiasing", 0);
	ConfigMan.setInt(Common::kConfigRealmDefault, "anisotropy", 0);
	ConfigMan.setBool(Common::kConfigRealmDefault, "framebuffereffects", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "softshadows", true);
	ConfigMan.setBool(Common::kConfigRealmDefault, "vsync", false);

	// Sound

	// Sound -> Advanced options

	ConfigMan.setInt(Common::kConfigRealmDefault, "eax", 0);
	ConfigMan.setBool(Common::kConfigRealmDefault, "forcesoftware", false);

	// xoreos-specific options

	// Should we disable hiding of far rooms when the fly cam is enabled?
	ConfigMan.setBool(Common::kConfigRealmDefault, "flycamallrooms", true);
}

void KotOREngine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "KOTOR_moduleDir",
		Common::FilePath::findSubDirectory(_target, "modules", true));
}

void KotOREngine::checkConfig() {
	checkConfigInt("difficulty", 0, 2);

	checkConfigInt("texturepack", 0, 2);
	checkConfigInt("antialiasing", 0, 3);
	checkConfigInt("anisotropy", 0, 4);

	checkConfigInt("eax", 0, 3);
}

void KotOREngine::deinit() {
	unregisterModelLoader();

	_version.reset();
	_game.reset();
}

void KotOREngine::playIntroVideos() {
	switch (_platform) {
		case Aurora::kPlatformXbox:
			playVideo("sizzle"); // Logos and a preview of the game
			break;

		default:
		case Aurora::kPlatformWindows:
			playVideo("leclogo"); // LucasArts
			playVideo("biologo"); // BioWare
			playVideo("legal");   // Legal billboard
			break;

		case Aurora::kPlatformMacOSX:
			playVideo("leclogo");              // LucasArts
			playVideo("biologo");              // BioWare
			playVideo("Aspyr_BlueDust_intro"); // Aspyr
			playVideo("legal");                // Legal billboard
			break;

		case Aurora::kPlatformAndroid:
		case Aurora::kPlatformIOS:
			playVideo("leclogo"); // LucasArts
			playVideo("biologo"); // BioWare
			playVideo("aspyr");   // Aspyr
			playVideo("legal");   // Legal billboard
			break;
	}
}

bool KotOREngine::hasYavin4Module() const {
	return _platform != Aurora::kPlatformXbox || _hasLiveKey;
}

} // End of namespace KotOR

} // End of namespace Engines

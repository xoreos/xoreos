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
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords.
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

#include "src/engines/kotor2/kotor2.h"
#include "src/engines/kotor2/modelloader.h"
#include "src/engines/kotor2/console.h"
#include "src/engines/kotor2/game.h"

namespace Engines {

namespace KotOR2 {

KotOR2Engine::KotOR2Engine() : _language(Aurora::kLanguageInvalid) {
	_console = std::make_unique<Console>(*this);
}

KotOR2Engine::~KotOR2Engine() {
}

bool KotOR2Engine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                   Aurora::Platform platform,
                                   std::vector<Aurora::Language> &languages) const {
	try {
		Common::UString baseDir = target;
		if      (platform == Aurora::kPlatformLinux)
			baseDir += "/steamassets";
		else if (platform == Aurora::kPlatformMacOSX)
			baseDir += "/GameData";

		Common::FileList files;
		if (!files.addDirectory(baseDir))
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

bool KotOR2Engine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool KotOR2Engine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

void KotOR2Engine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	_game = std::make_unique<Game>(*this, *_console, _platform);
	_game->run();

	deinit();
}

void KotOR2Engine::init() {
	LoadProgress progress(17);

	progress.step("Declare languages");
	declareLanguages();

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

void KotOR2Engine::declareLanguages() {
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

void KotOR2Engine::initResources(LoadProgress &progress) {
	// In the Xbox version of KotOR2, TXB textures are actually TPCs
	if (_platform == Aurora::kPlatformXbox)
		ResMan.addTypeAlias(Aurora::kFileTypeTXB, Aurora::kFileTypeTPC);

	progress.step("Setting base directory");

	Common::UString baseDir = _target;
	if      (_platform == Aurora::kPlatformLinux)
		baseDir += "/steamassets";
	else if (_platform == Aurora::kPlatformMacOSX)
		baseDir += "/GameData";

	ResMan.registerDataBase(baseDir);

	progress.step("Adding extra archive directories");
	const Common::UString dataDir = (_platform == Aurora::kPlatformXbox) ? "dataxbox" : "data";
	const Common::UString rimsDir = (_platform == Aurora::kPlatformXbox) ? "rimsxbox" : "rims";

	indexMandatoryDirectory( dataDir , 0, 0, 2);
	indexMandatoryDirectory("lips"   , 0, 0, 3);
	indexMandatoryDirectory("modules", 0, 0, 4);

	indexOptionalDirectory(rimsDir, 0, 0, 5);

	if (_platform != Aurora::kPlatformXbox)
		indexMandatoryDirectory("texturepacks", 0, 0, 6);

	if (_platform == Aurora::kPlatformXbox)
		indexMandatoryDirectory("supermodels" , 0, 0, 7);

	progress.step("Loading main KEY");
	indexMandatoryArchive("chitin.key", 10);

	progress.step("Loading high-res texture packs");
	if (_platform != Aurora::kPlatformXbox)
		indexMandatoryArchive("swpc_tex_gui.erf", 50);

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 100);
	progress.step("Indexing extra voice resources");
	indexMandatoryDirectory("streamvoice" , 0, -1, 101);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 102);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 103);

	progress.step("Indexing platform-specific resources");
	if (_platform == Aurora::kPlatformWindows) {
		initCursorsRemap();
		indexMandatoryArchive("swkotor2.exe", 104);
	} else if (_platform == Aurora::kPlatformLinux) {
		indexMandatoryDirectory("../resources/cursors/", 0, 0, 104);
	} else if (_platform == Aurora::kPlatformMacOSX) {
		indexMandatoryDirectory("../Resources/Cursors/", 0, 0, 104);
	}

	// Texture packs at 400, in module.cpp

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 500);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	progress.step("Registering file formats");
	registerModelLoader(new KotOR2ModelLoader(_platform == Aurora::kPlatformXbox));
	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);
}

void KotOR2Engine::initCursorsRemap() {
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

void KotOR2Engine::initCursors() {
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

void KotOR2Engine::initConfig() {
	ConfigMan.setInt(Common::kConfigRealmDefault, "texturepack", 2);

	// Should we disable hiding of far rooms when the fly cam is enabled?
	ConfigMan.setBool(Common::kConfigRealmDefault, "flycamallrooms", true);
}

void KotOR2Engine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "KOTOR2_moduleDir",
		Common::FilePath::findSubDirectory(ResMan.getDataBase(), "modules", true));
}

void KotOR2Engine::checkConfig() {
	checkConfigInt("texturepack", 0, 2);
}

void KotOR2Engine::deinit() {
	unregisterModelLoader();

	_game.reset();
}

void KotOR2Engine::playIntroVideos() {
	playVideo("leclogo");
	playVideo("obsidianent");
	playVideo("aspyr");
	playVideo("legal");
}

} // End of namespace KotOR2

} // End of namespace Engines

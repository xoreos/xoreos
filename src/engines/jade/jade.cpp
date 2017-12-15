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
 *  Engine class handling Jade Empire
 */

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/configman.h"

#include "src/aurora/util.h"
#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/talktable_tlk.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/fontman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/jade/gui/main/main.h"

#include "src/engines/jade/jade.h"
#include "src/engines/jade/modelloader.h"
#include "src/engines/jade/console.h"
#include "src/engines/jade/game.h"

namespace Engines {

namespace Jade {

JadeEngine::JadeEngine() : _language(Aurora::kLanguageInvalid) {
	_console.reset(new Console(*this));
}

JadeEngine::~JadeEngine() {
}

bool JadeEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                 Aurora::Platform UNUSED(platform),
                                 std::vector<Aurora::Language> &languages) const {
	try {
		Common::FileList files;
		if (!files.addDirectory(target))
			return true;

		Common::UString tlk = files.findFirst("dialog.tlk", true);
		if (tlk.empty())
			return true;

		uint32 languageID = Aurora::TalkTable_TLK::getLanguageID(tlk);
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

bool JadeEngine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool JadeEngine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

Game &JadeEngine::getGame() {
	assert(_game);

	return *_game;
}

void JadeEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	_game.reset(new Game(*this, *_console, _platform));
	_game->run();

	deinit();
}

void JadeEngine::init() {
	LoadProgress progress(16);

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

	if (_platform == Aurora::kPlatformWindows) {
		progress.step("Loading game cursors");
		initCursors();
		if (EventMan.quitRequested())
			return;
	}

	progress.step("Initializing internal game config");
	initGameConfig();

	progress.step("Successfully initialized the engine");

	GfxMan.setGUIScale(Graphics::GraphicsManager::kScalingWindowSize);
	GfxMan.setGUISize(640, 480);
}

void JadeEngine::declareLanguages() {
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

void JadeEngine::initResources(LoadProgress &progress) {
	// Some new file types with the same function as old ones re-use the type ID
	ResMan.addTypeAlias(Aurora::kFileTypeBTC, Aurora::kFileTypeCRE);
	ResMan.addTypeAlias(Aurora::kFileTypeBTP, Aurora::kFileTypePLA);
	ResMan.addTypeAlias(Aurora::kFileTypeGIT, Aurora::kFileTypeSAV);
	ResMan.addTypeAlias(Aurora::kFileTypeBTT, Aurora::kFileTypeTRG);

	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding extra archive directories");
	indexMandatoryDirectory("data", 0, -1, 2);

	progress.step("Loading main KEY");
	indexMandatoryArchive("chitin.key", 10);

	progress.step("Loading global auxiliary resources");
	indexMandatoryArchive("loadscreens.mod"   , 50);
	indexMandatoryArchive("players.mod"       , 51);
	indexMandatoryArchive("global-a.rim"      , 52);
	indexMandatoryArchive("ingamemenu-a.rim"  , 53);
	indexMandatoryArchive("globalunload-a.rim", 54);
	indexMandatoryArchive("minigame-a.rim"    , 55);
	indexMandatoryArchive("miniglobal-a.rim"  , 56);
	indexMandatoryArchive("mmenu-a.rim"       , 57);

	progress.step("Indexing extra font resources");
	indexMandatoryDirectory("fonts"   , 0, -1, 100);
	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("sound"   , 0, -1, 101);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"  , 0, -1, 102);

	if (_platform == Aurora::kPlatformWindows) {
		progress.step("Indexing extra shader resources");
		indexMandatoryDirectory("shaderpc", 0, -1, 103);
	}

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 150);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	progress.step("Registering file formats");
	registerModelLoader(new JadeModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatABC);
	FontMan.addAlias("sava"       , "eurogui");
	FontMan.addAlias("cerigo"     , "eurodialog");
	FontMan.addAlias("fnt_gui"    , "eurogui");
	FontMan.addAlias("fnt_console", "eurogui");
}

void JadeEngine::initCursors() {
	CursorMan.add("ui_cursor32", "default");

	CursorMan.setDefault("default");
}

void JadeEngine::initConfig() {
}

void JadeEngine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "JADE_moduleDir",
		Common::FilePath::findSubDirectory(_target, "data", true));
}

void JadeEngine::deinit() {
	_game.reset();
}

void JadeEngine::playIntroVideos() {
	playVideo("black");
	playVideo("publisher");
	playVideo("bwlogo");
	playVideo("black");
	playVideo("graymatr");
}

} // End of namespace Jade

} // End of namespace Engines

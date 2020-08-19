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
 *  Engine class handling The Witcher.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/readstream.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"

#include "src/aurora/lua/scriptman.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/modelloader.h"
#include "src/engines/witcher/console.h"
#include "src/engines/witcher/game.h"

namespace Engines {

namespace Witcher {

WitcherEngine::WitcherEngine() :
	_languageText(Aurora::kLanguageInvalid), _languageVoice(Aurora::kLanguageInvalid) {

	_console = std::make_unique<Console>(*this);
}

WitcherEngine::~WitcherEngine() {
}

bool WitcherEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                    Aurora::Platform UNUSED(platform),
                                    std::vector<Aurora::Language> &languagesText,
                                    std::vector<Aurora::Language> &languagesVoice) const {
	try {
		Common::UString dataDir = Common::FilePath::findSubDirectory(target, "data", true);
		if (dataDir.empty())
			return true;

		Common::FileList files;
		if (!files.addDirectory(dataDir))
			return true;

		for (size_t i = 0; i < Aurora::kLanguageMAX; i++) {
			const uint32_t langID = LangMan.getLanguageID((Aurora::Language) i);

			const Common::UString voiceKey = Common::UString::format("lang_%d.key"  , langID);
			const Common::UString textKey  = Common::UString::format("dialog_%d.tlk", langID);

			if (files.contains(voiceKey, true))
				languagesVoice.push_back((Aurora::Language) i);
			if (files.contains(textKey, true))
				languagesText.push_back((Aurora::Language) i);
		}

	} catch (...) {
	}

	return true;
}

bool WitcherEngine::getLanguage(Aurora::Language &languageText, Aurora::Language &languageVoice) const {
	languageText  = _languageText;
	languageVoice = _languageVoice;

	return true;
}

bool WitcherEngine::changeLanguage() {
	Aurora::Language languageText, languageVoice;
	if (!evaluateLanguage(false, languageText, languageVoice))
		return false;

	if ((_languageText == languageText) && (_languageVoice == languageVoice))
		return true;

	try {

		loadLanguageFiles(languageText, languageVoice);

		if (_game)
			_game->refreshLocalized();

		_languageText  = languageText;
		_languageVoice = languageVoice;

	} catch (...) {

		// Roll back
		loadLanguageFiles(_languageText, _languageVoice);
		return false;

	}

	return true;
}

Game &WitcherEngine::getGame() {
	assert(_game);

	return *_game;
}

void WitcherEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	_game = std::make_unique<Game>(*this, *_console);
	_game->run();

	deinit();
}

void WitcherEngine::init() {
	LoadProgress progress(15);

	progress.step("Declare languages");
	declareLanguages();

	if (evaluateLanguage(true, _languageText, _languageVoice))
		status("Setting the language to %s text + %s voices",
				LangMan.getLanguageName(_languageText).c_str(),
				LangMan.getLanguageName(_languageVoice).c_str());
	else
		throw Common::Exception("Failed to detect this game's language");

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

	progress.step("Initializing Lua subsystem");
	initLua();

	progress.step("Successfully initialized the engine");
}

void WitcherEngine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageDebug             ,  0, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageEnglish           ,  3, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguagePolish            ,  5, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageGerman            , 10, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageFrench            , 11, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageSpanish           , 12, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageItalian           , 13, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageRussian           , 14, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageCzech             , 15, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageHungarian         , 16, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageKorean            , 20, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageChineseTraditional, 21, Common::kEncodingUTF8, Common::kEncodingUTF8 },
		{ Aurora::kLanguageChineseSimplified , 22, Common::kEncodingUTF8, Common::kEncodingUTF8 }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void WitcherEngine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding extra archive directories");
	indexMandatoryDirectory("system"        , 0,  0, 2);
	indexMandatoryDirectory("system/scripts", 0,  0, 3);
	indexMandatoryDirectory("data"          , 0,  0, 4);
	indexMandatoryDirectory("data/modules"  , 0, -1, 5);

	// Contains BIFs with voices for the two premium modules
	indexOptionalDirectory("data/voices", 0, 0, 6);

	progress.step("Loading main KEY");
	indexMandatoryArchive("main.key", 10);

	progress.step("Loading the localized base KEY");
	indexMandatoryArchive("localized.key", 50);

	// Language files at 100-102

	progress.step("Indexing extra resources");
	indexOptionalDirectory("data/movies"   , 0, -1, 150);
	indexOptionalDirectory("data/music"    , 0, -1, 151);
	indexOptionalDirectory("data/sounds"   , 0, -1, 152);
	indexOptionalDirectory("data/cutscenes", 0, -1, 153);
	indexOptionalDirectory("data/dialogues", 0, -1, 154);
	indexOptionalDirectory("data/fx"       , 0, -1, 155);
	indexOptionalDirectory("data/meshes"   , 0, -1, 156);
	indexOptionalDirectory("data/quests"   , 0, -1, 157);
	indexOptionalDirectory("data/scripts"  , 0, -1, 158);
	indexOptionalDirectory("data/templates", 0, -1, 159);
	indexOptionalDirectory("data/textures" , 0, -1, 160);

	progress.step("Indexing Windows-specific resources");
	indexMandatoryArchive("witcher.exe", 250);

	progress.step("Indexing override files");
	indexOptionalDirectory("data/override", 0, 0, 500);

	loadLanguageFiles(progress, _languageText, _languageVoice);

	progress.step("Registering file formats");
	registerModelLoader(new WitcherModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTTF);
}

void WitcherEngine::initCursors() {
	CursorMan.add("cursor0" , "default"  , "up"  );
	CursorMan.add("cursor1" , "default"  , "down");

	CursorMan.setDefault("default", "up");
}

void WitcherEngine::initConfig() {
}

void WitcherEngine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "WITCHER_moduleDir",
		Common::FilePath::findSubDirectory(_target, "data/modules", true));
}

void WitcherEngine::initLua() {
	LuaScriptMan.init();
}

void WitcherEngine::unloadLanguageFiles() {
	TalkMan.removeTable(_languageTLK);

	std::list<Common::ChangeID>::iterator res;
	for (res = _languageResources.begin(); res != _languageResources.end(); ++res)
		deindexResources(*res);

	_languageResources.clear();
}

void WitcherEngine::loadLanguageFiles(LoadProgress &progress,
		Aurora::Language langText, Aurora::Language langVoice) {

	progress.step(Common::UString::format("Indexing language files (%s text + %s voices)",
				LangMan.getLanguageName(langText).c_str(), LangMan.getLanguageName(langVoice).c_str()));

	loadLanguageFiles(langText, langVoice);
}

void WitcherEngine::loadLanguageFiles(Aurora::Language langText, Aurora::Language langVoice) {
	unloadLanguageFiles();
	LangMan.setCurrentLanguage(langText, langVoice);

	Common::UString archive;

	_languageResources.push_back(Common::ChangeID());
	archive = Common::UString::format("lang_%d.key", LangMan.getLanguageID(langVoice));
	indexMandatoryArchive(archive, 100, &_languageResources.back());

	// Voices for the first premium module (The Price of Neutrality)
	_languageResources.push_back(Common::ChangeID());
	archive = Common::UString::format("M1_%d.key", LangMan.getLanguageID(langVoice));
	indexOptionalArchive(archive, 101, &_languageResources.back());

	// Voices for the second premium module (Side Effects)
	_languageResources.push_back(Common::ChangeID());
	archive = Common::UString::format("M2_%d.key", LangMan.getLanguageID(langVoice));
	indexOptionalArchive(archive, 102, &_languageResources.back());

	archive = Common::UString::format("dialog_%d", LangMan.getLanguageID(langText));
	TalkMan.addTable(archive, "", false, 0, &_languageTLK);
}

void WitcherEngine::deinit() {
	_game.reset();

	LuaScriptMan.deinit();
	::Aurora::Lua::ScriptManager::destroy();
}

void WitcherEngine::playIntroVideos() {
	playVideo("publisher");
	playVideo("developer");
	playVideo("engine");
	playVideo("intro");
	playVideo("title");
}

} // End of namespace Witcher

} // End of namespace Engines

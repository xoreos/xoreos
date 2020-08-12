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
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/readstream.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/ndsrom.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/sonic/sonic.h"
#include "src/engines/sonic/types.h"
#include "src/engines/sonic/files.h"
#include "src/engines/sonic/modelloader.h"
#include "src/engines/sonic/console.h"
#include "src/engines/sonic/game.h"

namespace Engines {

namespace Sonic {

SonicEngine::SonicEngine() : _language(Aurora::kLanguageInvalid) {
	_console = std::make_unique<Console>(*this);
}

SonicEngine::~SonicEngine() {
}

Common::UString SonicEngine::getLanguageHERF(Aurora::Language language) {
	switch (language) {
		case Aurora::kLanguageEnglish:
			return "test_e";
		case Aurora::kLanguageFrench:
			return "test_f";
		case Aurora::kLanguageGerman:
			return "test_g";
		case Aurora::kLanguageItalian:
			return "test_i";
		case Aurora::kLanguageSpanish:
			return "test_s";
		case Aurora::kLanguageJapanese:
			return "test_j";
		default:
			break;
	}

	return "";
}

Common::UString SonicEngine::getLanguageTLK(Aurora::Language language) {
	switch (language) {
		case Aurora::kLanguageEnglish:
			return "strings";
		case Aurora::kLanguageFrench:
			return "strings_fr-fr";
		case Aurora::kLanguageGerman:
			return "strings_de-de";
		case Aurora::kLanguageItalian:
			return "strings_it-it";
		case Aurora::kLanguageSpanish:
			return "strings_es-es";
		case Aurora::kLanguageJapanese:
			return "strings_ja-jp";
		default:
			break;
	}

	return "";
}

bool SonicEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                  Aurora::Platform UNUSED(platform),
                                  std::vector<Aurora::Language> &languages) const {
	try {
		Aurora::NDSFile nds(target);

		for (size_t i = 0; i < Aurora::kLanguageMAX; i++) {
			Common::UString herf = getLanguageHERF((Aurora::Language) i);
			Common::UString tlk  = getLanguageTLK ((Aurora::Language) i);
			if (herf.empty() || tlk.empty())
				continue;

			if (!nds.hasResource(herf + ".herf") || !nds.hasResource(tlk + ".tlk"))
				continue;

			languages.push_back((Aurora::Language) i);
		}

	} catch (...) {
	}

	return true;
}

bool SonicEngine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool SonicEngine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language))
		return false;

	if (_language == language)
		return true;

	try {

		loadLanguageFiles(language);
		_language = language;

	} catch (...) {

		// Roll back
		loadLanguageFiles(_language);
		return false;

	}

	return true;
}

Game &SonicEngine::getGame() {
	assert(_game);

	return *_game;
}

void SonicEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();
	CursorMan.showCursor();

	_game = std::make_unique<Game>(*this, *_console);
	_game->run();

	deinit();
}

void SonicEngine::init() {
	GfxMan.setGUIScale(Graphics::GraphicsManager::kScalingWindowSize);
	GfxMan.setGUISize(kScreenWidth, 2 * kScreenHeight);

	// Force to the screen size of the Nintendo DS (2 screens of 256x192)
	WindowMan.setWindowSize(kScreenWidth, 2 * kScreenHeight);

	LoadProgress progress(8);

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

	progress.step("Initializing internal game config");
	initGameConfig();

	progress.step("Successfully initialized the engine");
}

void SonicEngine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageEnglish , Aurora::kLanguageInvalid, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench  , Aurora::kLanguageInvalid, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman  , Aurora::kLanguageInvalid, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageItalian , Aurora::kLanguageInvalid, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageSpanish , Aurora::kLanguageInvalid, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageJapanese, Aurora::kLanguageInvalid, Common::kEncodingUTF8  , Common::kEncodingUTF8   }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void SonicEngine::initResources(LoadProgress &progress) {
	ResMan.setHasSmall(true);
	ResMan.setHashAlgo(Common::kHashDJB2);

	progress.step("Indexing the ROM file");
	ResMan.registerDataBase(_target);

	progress.step("Indexing the main HERF file");
	indexMandatoryArchive("test.herf", 10);

	loadLanguageFiles(progress, _language);

	progress.step("Registering files and formats");
	declareResources();
	registerModelLoader(new SonicModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatNFTR);

	GfxMan.setOrthogonal(0.0f, 1000.0f);
	GfxMan.setCullFace(false);
}

void SonicEngine::declareResources() {
	for (size_t i = 0; i < ARRAYSIZE(kFiles); i++)
		ResMan.declareResource(kFiles[i]);
}

void SonicEngine::unloadLanguageFiles() {
	TalkMan.removeTable(_languageTLK);

	deindexResources(_languageHERF);
}

void SonicEngine::loadLanguageFiles(LoadProgress &progress, Aurora::Language language) {
	progress.step(Common::String::format("Indexing language files (%s)",
				LangMan.getLanguageName(language).c_str()));

	loadLanguageFiles(language);
}

void SonicEngine::loadLanguageFiles(Aurora::Language language) {
	unloadLanguageFiles();
	LangMan.setCurrentLanguage(language);

	Common::UString herf = getLanguageHERF(language) + ".herf";

	indexMandatoryArchive(herf, 50, &_languageHERF);

	Common::UString tlk = getLanguageTLK(language);

	TalkMan.addTable(tlk, "", false, 0, &_languageTLK);
}

void SonicEngine::initConfig() {
}

void SonicEngine::initGameConfig() {
}

void SonicEngine::deinit() {
	_game.reset();

	unloadLanguageFiles();
}

} // End of namespace Sonic

} // End of namespace Engines

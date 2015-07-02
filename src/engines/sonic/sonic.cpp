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
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/readstream.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/ndsrom.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/sonic/sonic.h"
#include "src/engines/sonic/types.h"
#include "src/engines/sonic/files.h"
#include "src/engines/sonic/modelloader.h"
#include "src/engines/sonic/console.h"
#include "src/engines/sonic/util.h"
#include "src/engines/sonic/module.h"

namespace Engines {

namespace Sonic {

const SonicEngineProbe kSonicEngineProbe;

const Common::UString SonicEngineProbe::kGameName = "Sonic Chronicles: The Dark Brotherhood";

SonicEngineProbe::SonicEngineProbe() {
}

SonicEngineProbe::~SonicEngineProbe() {
}

Aurora::GameID SonicEngineProbe::getGameID() const {
	return Aurora::kGameIDSonic;
}

const Common::UString &SonicEngineProbe::getGameName() const {
	return kGameName;
}

bool SonicEngineProbe::probe(const Common::UString &UNUSED(directory),
                             const Common::FileList &UNUSED(rootFiles)) const {

	return false;
}

bool SonicEngineProbe::probe(Common::SeekableReadStream &stream) const {
	Common::UString title, code, maker;
	if (!Aurora::NDSFile::isNDS(stream, title, code, maker))
		return false;

	return title == "SONICCHRON";
}

Engines::Engine *SonicEngineProbe::createEngine() const {
	return new SonicEngine;
}


SonicEngine::SonicEngine() : _language(Aurora::kLanguageInvalid), _module(0) {
	_console = new Console(*this);
}

SonicEngine::~SonicEngine() {
	delete _module;
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

Module *SonicEngine::getModule() {
	return _module;
}

void SonicEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();
	CursorMan.showCursor();

	main();

	deinit();
}

void SonicEngine::init() {
	// Force to the screen size of the Nintendo DS (2 screens of 256x192)
	GfxMan.setScreenSize(kScreenWidth, 2 * kScreenHeight);

	LoadProgress progress(9);

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

	progress.step("Load essential fonts");

	_guiFont = loadFont("guifont", "jillcan13sh", false);

	if (_language == Aurora::kLanguageJapanese)
		_quoteFont = loadFont("quotefont", "jillcan12sh", false);
	else
		_quoteFont = loadFont("quotefont", "jillcan12rg", true);
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
	progress.step(Common::UString::format("Indexing language files (%s)",
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
	_guiFont.clear();

	unloadLanguageFiles();
}

bool SonicEngine::waitClick() {
	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (event.type == Events::kEventMouseUp)
				return true;
		}

		EventMan.delay(10);
	}

	return !EventMan.quitRequested();
}

bool SonicEngine::showLicenseSplash() {
	Graphics::Aurora::GUIQuad top("nintendosplash"  , 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("actimaginesplash", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	GfxMan.lockFrame();
	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();

	return true;
}

bool SonicEngine::showTitle() {
	Graphics::Aurora::GUIQuad top("introscr_top", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("introscr_bot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();

	return true;
}

bool SonicEngine::showMainMenu() {
	Graphics::Aurora::GUIQuad top("introscr_top" , 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("startupscrbot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY   , 0.0f);
	bot.setPosition(kBottomScreenX, kBottomScreenY, 0.0f);

	top.show();
	bot.show();


	Graphics::Aurora::TextureHandle buttonTexture = loadNCGR("main_pnl_off", "gui_main_pnl", 8, 2,
			"00", "01", "02", "03", "04", (const char *) 0, (const char *) 0, (const char *) 0,
			"10", "11", "12", "13", "14", "15", "16", "17");

	Graphics::Aurora::GUIQuad button(buttonTexture, 0.0f, 0.0f, 240.0f, 48.0f, 0.0f, 1.0f, 1.0f, 0.0f);

	button.setPosition(-120.0f, -120.0f, -1.0f);
	button.show();


	Graphics::Aurora::Text buttonText(_guiFont, TalkMan.getString(15860));

	float buttonX, buttonY, buttonZ;
	button.getPosition(buttonX, buttonY, buttonZ);

	const float buttonTextX = buttonX + ((button.getWidth()  - buttonText.getWidth())  / 2.0f);
	const float buttonTextY = buttonY + ((button.getHeight() - buttonText.getHeight()) / 2.0f);

	buttonText.setPosition(buttonTextX, buttonTextY, -2.0f);
	buttonText.show();


	Graphics::Aurora::Text helpText(_guiFont, TalkMan.getString(18707));

	const float helpTextX = kBottomScreenX + ((kScreenWidth - helpText.getWidth()) / 2.0f);
	const float helpTextY = kBottomScreenY;

	helpText.setPosition(helpTextX, helpTextY, -2.0f);
	helpText.show();


	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	helpText.hide();
	buttonText.hide();
	button.hide();
	top.hide();
	bot.hide();

	return true;
}

bool SonicEngine::showQuote() {
	const float length = (_language == Aurora::kLanguageJapanese) ? 236.0f : 256.0f;
	const float align  = (_language == Aurora::kLanguageJapanese) ?   0.0f :   0.5f;

	Common::UString quote = TalkMan.getString(21712);
	_guiFont.getFont().split(quote, length, 0.0f, false);

	Graphics::Aurora::Text quoteText(_quoteFont, quote, 1.0f, 1.0f, 1.0f, 1.0f, align);

	const float quoteTextX = kTopScreenX + ((kScreenWidth  - quoteText.getWidth())  / 2.0f);
	const float quoteTextY = kTopScreenY + ((kScreenHeight - quoteText.getHeight()) / 2.0f);

	quoteText.setPosition(quoteTextX, quoteTextY, 0.0f);
	quoteText.show();

	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	quoteText.hide();

	return true;
}

bool SonicEngine::showChapter1() {
	Graphics::Aurora::GUIQuad top("chap1scr_top", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);
	Graphics::Aurora::GUIQuad bot("chap1scr_bot", 0.0f, 0.0f, kScreenWidth, kScreenHeight, 0.0f, 1.0f, 1.0f, 0.0f);

	top.setPosition(kTopScreenX   , kTopScreenY);
	bot.setPosition(kBottomScreenX, kBottomScreenY);

	top.show();
	bot.show();
	GfxMan.unlockFrame();

	if (!waitClick())
		return false;

	GfxMan.lockFrame();
	top.hide();
	bot.hide();
	GfxMan.unlockFrame();

	return true;
}

void SonicEngine::main() {
	if (!showLicenseSplash())
		return;
	if (!showTitle())
		return;
	if (!showMainMenu())
		return;
	if (!showQuote())
		return;
	if (!showChapter1())
		return;

	_module = new Module(*_console);

	_module->run();

	delete _module;
	_module = 0;
}

} // End of namespace Sonic

} // End of namespace Engines

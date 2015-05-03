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
#include "src/common/stream.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"
#include "src/aurora/ndsrom.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/cube.h"
#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/language.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"

#include "src/engines/sonic/sonic.h"
#include "src/engines/sonic/console.h"

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
	return Aurora::NDSFile::isNDS(stream);
}

Engines::Engine *SonicEngineProbe::createEngine() const {
	return new SonicEngine;
}


SonicEngine::SonicEngine() : _language(Aurora::kLanguageInvalid) {
	_console = new Console(*this);
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

		for (uint i = 0; i < Aurora::kLanguageMAX; i++) {
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

void SonicEngine::run() {
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

void SonicEngine::init() {
	LoadProgress progress(7);

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

	progress.step("Initializing internal game config");
	initGameConfig();

	progress.step("Successfully initialized the engine");
}

void SonicEngine::declareEncodings() {
	static const LanguageEncoding kLanguageEncodings[] = {
		{ Aurora::kLanguageEnglish           , Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench            , Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman            , Common::kEncodingCP1252 },
		{ Aurora::kLanguageItalian           , Common::kEncodingCP1252 },
		{ Aurora::kLanguageSpanish           , Common::kEncodingCP1252 }
	};

	Engines::declareEncodings(_game, kLanguageEncodings, ARRAYSIZE(kLanguageEncodings));
}

void SonicEngine::initResources(LoadProgress &progress) {
	ResMan.setHasSmall(true);
	ResMan.setHashAlgo(Common::kHashDJB2);

	progress.step("Indexing the ROM file");
	indexMandatoryArchive(Aurora::kArchiveNDS, _target, 1);

	progress.step("Indexing the main HERF file");
	indexMandatoryArchive(Aurora::kArchiveHERF, "test.herf", 10);

	loadLanguageFiles(progress, _language);

	declareResources();
}

void SonicEngine::declareResources() {
	ResMan.declareResource("nintendosplash.tga");
}

void SonicEngine::unloadLanguageFiles() {
	TalkMan.removeTable(_languageTLK);

	deindexResources(_languageHERF);
}

void SonicEngine::loadLanguageFiles(LoadProgress &progress, Aurora::Language language) {
	progress.step(Common::UString::sprintf("Indexing language files (%s)",
				Aurora::getLanguageName(language).c_str()));

	loadLanguageFiles(language);
}

void SonicEngine::loadLanguageFiles(Aurora::Language language) {
	unloadLanguageFiles();
	declareTalkLanguage(_game, language);

	Common::UString herf = getLanguageHERF(language) + ".herf";

	indexMandatoryArchive(Aurora::kArchiveHERF, herf, 50, &_languageHERF);

	Common::UString tlk = getLanguageTLK(language);

	TalkMan.addTable(tlk, "", false, 0, &_languageTLK);
}

void SonicEngine::initConfig() {
}

void SonicEngine::initGameConfig() {
}

void SonicEngine::deinit() {
}

void SonicEngine::playIntroVideos() {
	// Play the two logo videos
	playVideo("bioware");
	playVideo("sega");

	// TODO: We need to support playing two videos at once. The two logo videos
	// are both on the bottom screen, but (most) other videos have a top screen
	// and bottom screen video.
}

void SonicEngine::main() {
	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("nintendosplash");

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

} // End of namespace Sonic

} // End of namespace Engines

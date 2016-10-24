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
 *  Generic engine interface.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/graphics/aurora/fps.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/engine.h"

#include "src/engines/aurora/console.h"

namespace Engines {

Engine::Engine() : _game(Aurora::kGameIDUnknown), _platform(Aurora::kPlatformUnknown) {
}

Engine::~Engine() {
}

bool Engine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &UNUSED(target),
                             Aurora::Platform UNUSED(platform),
                             std::vector<Aurora::Language> &UNUSED(languages)) const {
	return false;
}

bool Engine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &UNUSED(target),
                             Aurora::Platform UNUSED(platform),
                             std::vector<Aurora::Language> &UNUSED(languagesText),
                             std::vector<Aurora::Language> &UNUSED(languagesVoice)) const {
	return false;
}

bool Engine::detectLanguages(std::vector<Aurora::Language> &languages) const {
	return detectLanguages(_game, _target, _platform, languages);
}

bool Engine::detectLanguages(std::vector<Aurora::Language> &languagesText,
                             std::vector<Aurora::Language> &languagesVoice) const {

	return detectLanguages(_game, _target, _platform, languagesText, languagesVoice);
}

bool Engine::getLanguage(Aurora::Language &UNUSED(language)) const {
	return false;
}

bool Engine::getLanguage(Aurora::Language &UNUSED(languageText),
                         Aurora::Language &UNUSED(languageVoice)) const {
	return false;
}

bool Engine::changeLanguage() {
	return false;
}

void Engine::start(Aurora::GameID game, const Common::UString &target, Aurora::Platform platform) {
	showFPS();

	_game     = game;
	_platform = platform;
	_target   = target;

	run();
}

void Engine::showFPS() {
	bool show = ConfigMan.getBool("showfps", false);

	if        ( show && !_fps) {

		_fps.reset(new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13)));
		_fps->show();

	} else if (!show &&  _fps) {

		_fps.reset();

	}
}

static bool hasLanguage(const std::vector<Aurora::Language> &langs, Aurora::Language lang) {
	return std::find(langs.begin(), langs.end(), lang) != langs.end();
}

static void fiddleLangChinese(const std::vector<Aurora::Language> &langs, Aurora::Language &lang) {
	// If we're given a generic Chinese language, look if we have traditional or simplified Chinese
	if (lang == Aurora::kLanguageChinese) {
		if      (hasLanguage(langs, Aurora::kLanguageChineseTraditional))
			lang = Aurora::kLanguageChineseTraditional;
		else if (hasLanguage(langs, Aurora::kLanguageChineseSimplified))
			lang = Aurora::kLanguageChineseSimplified;

		if (lang != Aurora::kLanguageChinese)
			status("Substituting %s for %s",
					LangMan.getLanguageName(lang).c_str(),
					LangMan.getLanguageName(Aurora::kLanguageChinese).c_str());
	}

	// Substitute simplified for traditional Chinese if necessary
	if (lang == Aurora::kLanguageChineseTraditional) {
		if (!hasLanguage(langs, Aurora::kLanguageChineseTraditional) &&
		     hasLanguage(langs, Aurora::kLanguageChineseSimplified))
			lang = Aurora::kLanguageChineseSimplified;

		if (lang != Aurora::kLanguageChineseTraditional)
			status("Substituting %s for %s",
					LangMan.getLanguageName(Aurora::kLanguageChineseSimplified).c_str(),
					LangMan.getLanguageName(Aurora::kLanguageChineseTraditional).c_str());
	}

	// Substitute traditional for simplified Chinese if necessary
	if (lang == Aurora::kLanguageChineseSimplified) {
		if (!hasLanguage(langs, Aurora::kLanguageChineseSimplified) &&
		     hasLanguage(langs, Aurora::kLanguageChineseTraditional))
			lang = Aurora::kLanguageChineseTraditional;

		if (lang != Aurora::kLanguageChineseSimplified)
			status("Substituting %s for %s",
					LangMan.getLanguageName(Aurora::kLanguageChineseTraditional).c_str(),
					LangMan.getLanguageName(Aurora::kLanguageChineseSimplified).c_str());
	}
}

static bool resolveLangInvalid(const std::vector<Aurora::Language> &langs, Aurora::Language &lang,
                               const Common::UString &conf, const Common::UString &specifier,
                               bool find) {

	if (lang != Aurora::kLanguageInvalid)
		return true;

	if (!find || langs.empty())
		return false;

	if (!conf.empty())
		warning("Failed to parse \"%s\" into a language", conf.c_str());

	lang = langs.front();
	status("Using the first available language (%s)%s",
			LangMan.getLanguageName(lang).c_str(), specifier.c_str());

	return true;
}

static bool resolveLangUnavailable(const std::vector<Aurora::Language> &langs, Aurora::Language &lang,
                                   const Common::UString &specifier, bool find) {

	if (hasLanguage(langs, lang))
		return true;

	if (!find || langs.empty())
		return false;

	Aurora::Language oldLang = lang;

	lang = langs.front();
	warning("This game version does not come with %s language files%s",
			LangMan.getLanguageName(oldLang).c_str(), specifier.c_str());

	status("Using the first available language (%s)%s",
			LangMan.getLanguageName(lang).c_str(), specifier.c_str());

	return true;
}

bool Engine::evaluateLanguage(bool find, Aurora::Language &language) const {
	language = Aurora::kLanguageInvalid;

	std::vector<Aurora::Language> langs;
	bool detected = detectLanguages(langs);
	assert(detected);

	if (langs.empty())
		return true;

	Common::UString confLang      = ConfigMan.getString("lang");
	Common::UString confLangText  = ConfigMan.getString("langtext");
	Common::UString confLangVoice = ConfigMan.getString("langvoice");

	if (confLangText.empty())
		confLangText = confLang;
	if (confLangVoice.empty())
		confLangVoice = confLang;

	Aurora::Language lang      = Aurora::kLanguageInvalid;
	Aurora::Language langText  = LangMan.parseLanguage(confLangText);
	Aurora::Language langVoice = LangMan.parseLanguage(confLangVoice);

	if (langText != langVoice) {
		if (confLangText.empty())
			langText = langVoice;
		if (confLangVoice.empty())
			langVoice = langText;

		if (langText != langVoice) {
			warning("Game does not support different languages for voice and text");

			langText = langs.front();
			status("Using the first available language (%s)", LangMan.getLanguageName(langText).c_str());
		}
	}

	confLang = confLangText;
	lang     = langText;

	fiddleLangChinese(langs, lang);

	if (!resolveLangInvalid(langs, lang, confLang, "", find))
		return false;
	if (!resolveLangUnavailable(langs, lang, "", find))
		return false;

	language = lang;

	return true;
}

bool Engine::evaluateLanguage(bool find, Aurora::Language &languageText,
                                         Aurora::Language &languageVoice) const {

	languageText = languageVoice = Aurora::kLanguageInvalid;

	std::vector<Aurora::Language> langsText, langsVoice;
	bool detected = detectLanguages(langsText, langsVoice);
	assert(detected);

	if (langsText.empty() || langsVoice.empty())
		return true;

	Common::UString confLang      = ConfigMan.getString("lang");
	Common::UString confLangText  = ConfigMan.getString("langtext");
	Common::UString confLangVoice = ConfigMan.getString("langvoice");

	if (confLangText.empty())
		confLangText = confLang;
	if (confLangVoice.empty())
		confLangVoice = confLang;

	Aurora::Language langText  = LangMan.parseLanguage(confLangText);
	Aurora::Language langVoice = LangMan.parseLanguage(confLangVoice);

	fiddleLangChinese(langsText , langText);
	fiddleLangChinese(langsVoice, langVoice);

	if (!resolveLangInvalid(langsText , langText , confLangText , " for text"  , find))
		return false;
	if (!resolveLangInvalid(langsVoice, langVoice, confLangVoice, " for voices", find))
		return false;

	if (!resolveLangUnavailable(langsText , langText , " for text"  , find))
		return false;
	if (!resolveLangUnavailable(langsVoice, langVoice, " for voices", find))
		return false;

	languageText  = langText;
	languageVoice = langVoice;

	return true;
}

} // End of namespace Engines

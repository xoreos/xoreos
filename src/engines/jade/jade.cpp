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

#include "src/graphics/camera.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/render/renderman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/jade/gui/main/main.h"

#include "src/engines/jade/jade.h"
#include "src/engines/jade/modelloader.h"
#include "src/engines/jade/console.h"
#include "src/engines/jade/game.h"

namespace Engines {

namespace Jade {

JadeEngine::JadeEngine() : _language(Aurora::kLanguageInvalid) {
	_console = std::make_unique<Console>(*this);
}

JadeEngine::~JadeEngine() {
}

/** Find all language TLKs in this Jade Empire installation. */
static void findTLKs(const Common::UString &target, Common::FileList &tlks) {
	Common::FileList files;
	if (!files.addDirectory(target))
		return;

	files.addDirectory(Common::FilePath::findSubDirectory(target, "localizations", true), 1);
	files.addDirectory(Common::FilePath::findSubDirectory(target, "localized", true), 1);

	const std::vector<Aurora::Language> gameLanguages = LangMan.getLanguages();
	for (std::vector<Aurora::Language>::const_iterator l = gameLanguages.begin();
	     l != gameLanguages.end(); ++l)
		files.addDirectory(Common::FilePath::findSubDirectory(target, LangMan.getLanguageName(*l)), -1);

	files.addDirectory(Common::FilePath::findSubDirectory(target, "Chinese", true), -1);

	files.getSubList("/dialog.tlk", true, tlks);
}

/** Add the language to the vector if it's a valid language and not yet in the vector. */
static void addUniqueLanguageIfValid(std::vector<Aurora::Language> &languages, Aurora::Language language) {
	if ((language == Aurora::kLanguageInvalid) ||
	    (std::find(languages.begin(), languages.end(), language) != languages.end()))
		return;

	languages.push_back(language);
}

/** Return the language of a TLK. */
static Aurora::Language getTLKLanguage(const Common::UString &tlk) {
	const uint32_t languageID = Aurora::TalkTable_TLK::getLanguageID(tlk);
	if (languageID == Aurora::kLanguageInvalid)
		return Aurora::kLanguageInvalid;

	return LangMan.getLanguage(languageID);
}

/** Return the subdirectory of the game path where the TLK for a specific language is in. */
static Common::UString getTLKDirectory(const Common::UString &target, Aurora::Language language) {
	Common::FileList tlks;
	findTLKs(target, tlks);

	for (Common::FileList::const_iterator tlk = tlks.begin(); tlk != tlks.end(); ++tlk)
		if (getTLKLanguage(*tlk) == language)
			return Common::FilePath::getDirectory(Common::FilePath::relativize(target, *tlk));

	return "";
}

bool JadeEngine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                 Aurora::Platform UNUSED(platform),
                                 std::vector<Aurora::Language> &languages) const {
	try {
		Common::FileList tlks;
		findTLKs(target, tlks);

		for (Common::FileList::const_iterator tlk = tlks.begin(); tlk != tlks.end(); ++tlk)
			addUniqueLanguageIfValid(languages, getTLKLanguage(*tlk));

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

	_game = std::make_unique<Game>(*this, *_console, _platform);
	_game->run();

	deinit();
}

void JadeEngine::init() {
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
	/**
	 * Jade Empire is proving to be a difficult beast to figure out what is opaque, and what is
	 * truly transparent. For this reason, doing things normally results in many rendering
	 * artefacts. Just sort everything by depth and be done with it.
	 */
	RenderMan.setSortingHint(Graphics::Render::RenderManager::SORT_HINT_ALLDEPTH);
}

void JadeEngine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageEnglish          ,   0, Common::kEncodingUTF8, Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench           ,   1, Common::kEncodingUTF8, Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman           ,   2, Common::kEncodingUTF8, Common::kEncodingCP1252 },
		{ Aurora::kLanguageItalian          ,   3, Common::kEncodingUTF8, Common::kEncodingCP1252 },
		{ Aurora::kLanguageSpanish          ,   4, Common::kEncodingUTF8, Common::kEncodingCP1252 },
		{ Aurora::kLanguagePolish           ,   5, Common::kEncodingUTF8, Common::kEncodingCP1250 },
		{ Aurora::kLanguageCzech            ,   6, Common::kEncodingUTF8, Common::kEncodingCP1250 },
		{ Aurora::kLanguageHungarian        ,   7, Common::kEncodingUTF8, Common::kEncodingCP1250 },
		{ Aurora::kLanguageChineseSimplified, 130, Common::kEncodingUTF8, Common::kEncodingCP936  },
		{ Aurora::kLanguageRussian          , 132, Common::kEncodingUTF8, Common::kEncodingCP1251 }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

/** Figure out which localization in an Android installation contains the voices. */
static Common::UString findAndroidVOLanguageDir(const Common::UString &target) {
	/* Look into all subdirectories of the directory localized (which are all named after
	 * a language). In each, see if there's a sound/vo subdirectory. If there is, return
	 * the subdirectory (i.e the language name). */

	const Common::UString loc = Common::FilePath::findSubDirectory(target, "localized", true);
	if (loc.empty())
		return "";

	std::list<Common::UString> langs;
	Common::FilePath::getSubDirectories(loc, langs);

	for (std::list<Common::UString>::const_iterator l = langs.begin(); l != langs.end(); ++l)
		if (!Common::FilePath::findSubDirectory(*l, "sound/vo", true).empty())
			return Common::FilePath::relativize(loc, *l);

	return "";
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
	indexMandatoryDirectory("movies"  , 0, -1, 103);

	if (_platform == Aurora::kPlatformWindows) {
		progress.step("Indexing extra shader resources");
		indexMandatoryDirectory("shaderpc", 0, -1, 104);
	}

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 150);

	progress.step("Indexing extra language files");

	/* If we're running an Android version, see which language the voices are in.
	 * This should only ever be one language, the native language of the installation.
	 *
	 * If this is the language we want to play the game in, we're indexing the
	 * resources below. If it isn't, index them with a lower priority, as a fallback. */
	const Common::UString androidVO = findAndroidVOLanguageDir(_target);
	const Aurora::Language androidVOLang = LangMan.parseLanguage(androidVO);
	if ((androidVOLang != Aurora::kLanguageInvalid) && (androidVOLang != LangMan.getCurrentLanguageText())) {
		const Common::UString locDir = Common::UString("localized/") + androidVO;

		indexOptionalDirectory(locDir + "/data",  0, -1,   3);
		indexOptionalDirectory(locDir + "/sound", 0, -1, 102);
	}

	const Common::UString langDir = getTLKDirectory(_target, LangMan.getCurrentLanguageText());
	if (!langDir.empty()) {
		indexMandatoryDirectory(langDir, 0, 0, 160);

		indexOptionalDirectory(langDir + "/data"  , 0, -1, 161);
		indexOptionalDirectory(langDir + "/fonts" , 0, -1, 162);
		indexOptionalDirectory(langDir + "/sound" , 0, -1, 163);
		indexOptionalDirectory(langDir + "/movies", 0, -1, 164);

		indexOptionalDirectory(langDir + "/override", 0,  0, 170);
	}

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	progress.step("Registering file formats");
	registerModelLoader(new JadeModelLoader);
	TextureMan.setDeswizzleSBM(_platform == Aurora::kPlatformXbox);
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
	switch (_platform) {
		case Aurora::kPlatformXbox:
			playVideo("bwlogo");    // BioWare
			playVideo("gsl_short"); // Microsoft
			break;

		default:
		case Aurora::kPlatformWindows:
			playVideo("black");     // One second of black
			playVideo("publisher"); // 2K Games (original DVD version) or Electronic Arts (GOG version)
			playVideo("black");     // One second of black
			playVideo("bwlogo");    // BioWare
			playVideo("black");     // One second of black
			playVideo("graymatr");  // Gray Matter
			break;

		case Aurora::kPlatformAndroid:
		case Aurora::kPlatformIOS:
			// TODO: These are Bink 2 videos, they don't actually play in xoreos

			playVideo("black");     // One second of black
			playVideo("publisher"); // EA
			playVideo("black");     // One second of black
			playVideo("bwlogo");    // BioWare
			playVideo("black");     // One second of black
			playVideo("aspyr");     // Aspyr
			break;
	}
}

} // End of namespace Jade

} // End of namespace Engines

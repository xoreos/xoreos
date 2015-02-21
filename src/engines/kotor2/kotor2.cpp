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
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/stream.h"
#include "src/common/configman.h"

#include "src/aurora/util.h"
#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/fps.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"


#include "src/engines/kotor2/kotor2.h"
#include "src/engines/kotor2/modelloader.h"
#include "src/engines/kotor2/console.h"
#include "src/engines/kotor2/module.h"

#include "src/engines/kotor2/gui/main/main.h"

namespace Engines {

namespace KotOR2 {

const KotOR2EngineProbeWin  kKotOR2EngineProbeWin;
const KotOR2EngineProbeXbox kKotOR2EngineProbeXbox;

const Common::UString KotOR2EngineProbe::kGameName = "Star Wars: Knights of the Old Republic II - The Sith Lords";

KotOR2EngineProbe::KotOR2EngineProbe() {
}

KotOR2EngineProbe::~KotOR2EngineProbe() {
}

Aurora::GameID KotOR2EngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR2;
}

const Common::UString &KotOR2EngineProbe::getGameName() const {
	return kGameName;
}

bool KotOR2EngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *KotOR2EngineProbe::createEngine() const {
	return new KotOR2Engine;
}


KotOR2EngineProbeWin::KotOR2EngineProbeWin() {
}

KotOR2EngineProbeWin::~KotOR2EngineProbeWin() {
}

bool KotOR2EngineProbeWin::probe(const Common::UString &UNUSED(directory),
                                 const Common::FileList &rootFiles) const {

	// If "swkotor2.exe" exists, this should be a valid path for the Windows port
	return rootFiles.contains("/swkotor2.exe", true);
}


KotOR2EngineProbeXbox::KotOR2EngineProbeXbox() {
}

KotOR2EngineProbeXbox::~KotOR2EngineProbeXbox() {
}

bool KotOR2EngineProbeXbox::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "dataxbox" directory exists and "weapons.erf" exists, this should be a valid path for the Xbox port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");
	return !appDirectory.empty() && rootFiles.contains("/weapons.erf", true);
}


KotOR2Engine::KotOR2Engine() : _fps(0) {
}

KotOR2Engine::~KotOR2Engine() {
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

	if (ConfigMan.getBool("showfps", false)) {
		_fps = new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13));
		_fps->show();
	}

	mainMenuLoop();

	deinit();
}

void KotOR2Engine::init() {
	LoadProgress progress(17);

	progress.step("Loading user game config");
	initConfig();
	checkConfig();

	if (EventMan.quitRequested())
		return;

	progress.step("Declare string encodings");
	declareEncodings();

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

void KotOR2Engine::declareEncodings() {
	uint32 langEnglish = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageEnglish);
	TalkMan.registerEncoding(langEnglish, Common::kEncodingCP1252);

	uint32 langFrench = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageFrench);
	TalkMan.registerEncoding(langFrench, Common::kEncodingCP1252);

	uint32 langGerman = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageGerman);
	TalkMan.registerEncoding(langGerman, Common::kEncodingCP1252);

	uint32 langItalian = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageItalian);
	TalkMan.registerEncoding(langItalian, Common::kEncodingCP1252);

	uint32 langSpanish = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageSpanish);
	TalkMan.registerEncoding(langSpanish, Common::kEncodingCP1252);

	uint32 langPolish = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguagePolish);
	TalkMan.registerEncoding(langPolish, Common::kEncodingCP1250);

	uint32 langKorean = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageKorean);
	TalkMan.registerEncoding(langKorean, Common::kEncodingCP949);

	uint32 langChineseT = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageChineseTraditional);
	TalkMan.registerEncoding(langChineseT, Common::kEncodingCP950);

	uint32 langChineseS = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageChineseSimplified);
	TalkMan.registerEncoding(langChineseS, Common::kEncodingCP936);

	uint32 langJapanese = Aurora::getLanguageID(Aurora::kGameIDKotOR2, Aurora::kLanguageJapanese);
	TalkMan.registerEncoding(langJapanese, Common::kEncodingCP932);
}

void KotOR2Engine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_target);

	// In the Xbox version of KotOR, TXB textures are actually TPCs
	if (_platform == Aurora::kPlatformXbox)
		ResMan.addTypeAlias(Aurora::kFileTypeTXB, Aurora::kFileTypeTPC);

	indexMandatoryDirectory("", 0, 0, 1);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, (_platform == Aurora::kPlatformXbox) ? "dataxbox" : "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "lips");

	if (_platform != Aurora::kPlatformXbox)
		ResMan.addArchiveDir(Aurora::kArchiveERF, "texturepacks");

	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, (_platform == Aurora::kPlatformXbox) ? "rimsxbox" : "rims");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "modules");

	if (_platform == Aurora::kPlatformXbox)
		ResMan.addArchiveDir(Aurora::kArchiveERF, "SuperModels");

	progress.step("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 1);

	progress.step("Loading high-res texture packs");
	if (_platform != Aurora::kPlatformXbox) {
		indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_gui.erf", 10);
		indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_tpa.erf", 11);
	}

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 20);
	progress.step("Indexing extra voice resources");
	indexMandatoryDirectory("streamvoice" , 0, -1, 21);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 22);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 23);

	progress.step("Indexing platform-specific resources");
	if (_platform == Aurora::kPlatformWindows) {
		initCursorsRemap();
		indexMandatoryArchive(Aurora::kArchiveEXE, "swkotor2.exe", 24);
	}

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 30);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addMainTable("dialog");

	progress.step("Registering file formats");
	registerModelLoader(new KotOR2ModelLoader);
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
}

void KotOR2Engine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "KOTOR_moduleDir",
		Common::FilePath::findSubDirectory(_target, "modules", true));
}

void KotOR2Engine::checkConfig() {
	checkConfigInt("texturepack", 0, 2, 2);
}

void KotOR2Engine::deinit() {
	delete _fps;
}

void KotOR2Engine::playIntroVideos() {
	playVideo("leclogo");
	playVideo("obsidianent");
	playVideo("legal");
}

void KotOR2Engine::playMenuMusic() {
	if (SoundMan.isPlaying(_menuMusic))
		return;

	_menuMusic = playSound("mus_sion", Sound::kSoundTypeMusic, true);
}

void KotOR2Engine::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void KotOR2Engine::mainMenuLoop() {
	playMenuMusic();

	Console console;
	Module module(console);

	console.setModule(&module);

	while (!EventMan.quitRequested()) {
		GUI *mainMenu = new MainMenu(module);

		EventMan.flushEvents();

		mainMenu->show();
		mainMenu->run();
		mainMenu->hide();

		delete mainMenu;

		if (EventMan.quitRequested())
			break;

		stopMenuMusic();

		module.run();
		if (EventMan.quitRequested())
			break;

		playMenuMusic();
		console.hide();
		module.clear();
	}

	console.setModule();

	stopMenuMusic();
}

} // End of namespace KotOR2

} // End of namespace Engines

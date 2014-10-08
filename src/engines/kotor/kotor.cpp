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

/** @file engines/kotor/kotor.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/stream.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "sound/sound.h"

#include "events/events.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"

#include "engines/aurora/util.h"
#include "engines/aurora/loadprogress.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/model.h"

#include "engines/kotor/kotor.h"
#include "engines/kotor/modelloader.h"
#include "engines/kotor/module.h"
#include "engines/kotor/console.h"

#include "engines/kotor/gui/main/main.h"

namespace Engines {

namespace KotOR {

const KotOREngineProbeWin  kKotOREngineProbeWin;
const KotOREngineProbeMac  kKotOREngineProbeMac;
const KotOREngineProbeXbox kKotOREngineProbeXbox;

const Common::UString KotOREngineProbe::kGameName = "Star Wars: Knights of the Old Republic";

KotOREngineProbe::KotOREngineProbe() {
}

KotOREngineProbe::~KotOREngineProbe() {
}

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const Common::UString &KotOREngineProbe::getGameName() const {
	return kGameName;
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine(getPlatform());
}


KotOREngineProbeWin::KotOREngineProbeWin() {
}

KotOREngineProbeWin::~KotOREngineProbeWin() {
}

bool KotOREngineProbeWin::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If swkotor.exe exists, this should be a valid path for the Windows port
	return rootFiles.containsGlob(".*/swkotor.exe", true);
}


KotOREngineProbeMac::KotOREngineProbeMac() {
}

KotOREngineProbeMac::~KotOREngineProbeMac() {
}

bool KotOREngineProbeMac::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "Knights of the Old Republic.app" directory exists, this should be a valid path for the Mac OS X port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app", true);
	return !appDirectory.empty();
}


KotOREngineProbeXbox::KotOREngineProbeXbox() {
}

KotOREngineProbeXbox::~KotOREngineProbeXbox() {
}

bool KotOREngineProbeXbox::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "dataxbox" directory exists and "players.erf" exists, this should be a valid path for the Xbox port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");
	return !appDirectory.empty() && rootFiles.containsGlob(".*/players.erf", true);
}


KotOREngine::KotOREngine(Aurora::Platform platform) : _platform(platform),
	_fps(0), _hasLiveKey(false) {
}

KotOREngine::~KotOREngine() {
}

void KotOREngine::run(const Common::UString &target) {
	_baseDirectory = target;

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

void KotOREngine::init() {
	LoadProgress progress(17);

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

void KotOREngine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

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

	progress.step("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 1);

	if (indexOptionalArchive(Aurora::kArchiveKEY, "live1.key", 2))
		_hasLiveKey = true;

	progress.step("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenu.rim"    , 10);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenudx.rim"  , 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legal.rim"       , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legaldx.rim"     , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global.rim"      , 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "subglobaldx.rim" , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "miniglobaldx.rim", 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globaldx.rim"    , 17);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargen.rim"     , 18);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargendx.rim"   , 19);

	if (_platform == Aurora::kPlatformXbox) {
		// The Xbox version has most of its textures in "textures.bif"
		// Some, however, reside in "players.erf"
		progress.step("Loading Xbox textures");
		indexMandatoryArchive(Aurora::kArchiveERF, "players.erf", 20);
	} else {
		// The Windows/Mac versions have the GUI textures here
		progress.step("Loading GUI textures");
		indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_gui.erf", 20);
	}

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 30);
	progress.step("Indexing extra voice resources");
	indexMandatoryDirectory("streamwaves" , 0, -1, 31);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 32);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 33);

	if (_platform == Aurora::kPlatformWindows) {
		progress.step("Indexing Windows-specific resources");
		initCursorsRemap();
		indexMandatoryArchive(Aurora::kArchiveEXE, "swkotor.exe", 34);
	} else if (_platform == Aurora::kPlatformMacOSX) {
		progress.step("Indexing Mac-specific resources");
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources",         0, -1, 34);
		indexMandatoryDirectory("Knights of the Old Republic.app/Contents/Resources/Cursors", 0, -1, 35);
	} else if (_platform == Aurora::kPlatformXbox) {
		progress.step("Indexing Xbox-specific resources");
		indexMandatoryDirectory("errortex"  , 0, -1, 34);
		indexMandatoryDirectory("localvault", 0, -1, 35);
		indexMandatoryDirectory("media"     , 0, -1, 36);

		// For the DLC, we need to index the "sound" directory as well
		if (_hasLiveKey)
			indexMandatoryDirectory("sound", 0, -1, 37);
	}

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 40);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addMainTable("dialog");

	if (_hasLiveKey)
		TalkMan.addAltTable("live1");

	progress.step("Registering file formats");
	registerModelLoader(new KotORModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTexture);
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
	ConfigMan.setInt(Common::kConfigRealmDefault, "texturepack", 2);
}

void KotOREngine::initGameConfig() {
}

void KotOREngine::checkConfig() {
	checkConfigInt("texturepack", 0, 2, 2);
}

void KotOREngine::deinit() {
	delete _fps;
}

void KotOREngine::playIntroVideos() {
	if (_platform == Aurora::kPlatformXbox) {
		playVideo("logo");
		// TODO: What the hell is (sizzle|sizzle2).xmv?
	} else {
		playVideo("leclogo");
		playVideo("biologo");

		// On Mac OS X, play the Aspyr logo
		if (_platform == Aurora::kPlatformMacOSX)
			playVideo("Aspyr_BlueDust_intro");

		playVideo("legal");
	}
}

void KotOREngine::playMenuMusic() {
	if (SoundMan.isPlaying(_menuMusic))
		return;

	_menuMusic = playSound("mus_theme_cult", Sound::kSoundTypeMusic, true);
}

void KotOREngine::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void KotOREngine::mainMenuLoop() {
	playMenuMusic();

	Console console;
	Module module(console);

	console.setModule(&module);

	while (!EventMan.quitRequested()) {
		GUI *mainMenu = new MainMenu(module, _platform == Aurora::kPlatformXbox);

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

bool KotOREngine::hasYavin4Module() const {
	return _platform != Aurora::kPlatformXbox || _hasLiveKey;
}

} // End of namespace KotOR

} // End of namespace Engines

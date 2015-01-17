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

/** @file engines/jade/jade.cpp
 *  Engine class handling Jade Empire
 */

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/cube.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/loadprogress.h"
#include "engines/aurora/resources.h"

#include "engines/jade/gui/main/main.h"

#include "engines/jade/jade.h"

namespace Engines {

namespace Jade {

const JadeEngineProbe kJadeEngineProbe;

const Common::UString JadeEngineProbe::kGameName = "Jade Empire";

JadeEngineProbe::JadeEngineProbe() {
}

JadeEngineProbe::~JadeEngineProbe() {
}

Aurora::GameID JadeEngineProbe::getGameID() const {
	return Aurora::kGameIDJade;
}

const Common::UString &JadeEngineProbe::getGameName() const {
	return kGameName;
}

bool JadeEngineProbe::probe(const Common::UString &UNUSED(directory),
                            const Common::FileList &rootFiles) const {

	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains("/JadeEmpire.exe", true))
		return true;

	return false;
}

bool JadeEngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *JadeEngineProbe::createEngine() const {
	return new JadeEngine;
}


JadeEngine::JadeEngine() {
}

JadeEngine::~JadeEngine() {
}

void JadeEngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playVideo("black");
	playVideo("publisher");
	playVideo("bwlogo");
	playVideo("graymatr");
	playVideo("attract");
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();


	MainMenu *mainMenu = new MainMenu();

	mainMenu->show();
	mainMenu->run();

	delete mainMenu;

	if (EventMan.quitRequested())
		return;


	playSound("musicbank00046", Sound::kSoundTypeMusic, true);

	bool showFPS = ConfigMan.getBool("showfps", false);

	Graphics::Aurora::FPS *fps = 0;
	if (showFPS) {
		fps = new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13));
		fps->show();
	}

	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("ui_ph_silk");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
	delete fps;
}

void JadeEngine::init() {
	LoadProgress progress(13);

	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	// In Jade Empire, CRE files are actually BTC
	ResMan.addTypeAlias(Aurora::kFileTypeCRE, Aurora::kFileTypeBTC);

	indexMandatoryDirectory("", 0, 0, 1);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data/bips");

	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data", true);

	progress.step("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 1);

	progress.step("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveERF, "loadscreens.mod"   , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "players.mod"       , 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global-a.rim"      , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "ingamemenu-a.rim"  , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globalunload-a.rim", 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "minigame-a.rim"    , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "miniglobal-a.rim"  , 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mmenu-a.rim"       , 17);

	progress.step("Indexing extra font resources");
	indexMandatoryDirectory("fonts"   , 0, -1, 20);
	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("sound"   , 0, -1, 21);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"  , 0, -1, 22);
	progress.step("Indexing extra shader resources");
	indexMandatoryDirectory("shaderpc", 0, -1, 23);

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 30);

	if (EventMan.quitRequested())
		return;

	progress.step("Loading main talk table");
	TalkMan.addMainTable("dialog");

	progress.step("Registering file formats");
	FontMan.setFormat(Graphics::Aurora::kFontFormatABC);
	FontMan.addAlias("sava"   , "asian");
	FontMan.addAlias("cerigo" , "asian");
	FontMan.addAlias("fnt_gui", "asian");

	progress.step("Loading game cursors");
	initCursors();

	progress.step("Successfully initialized the engine");
}

void JadeEngine::initCursors() {
	CursorMan.add("ui_cursor32", "default");

	CursorMan.setDefault("default");
}

} // End of namespace Jade

} // End of namespace Engines

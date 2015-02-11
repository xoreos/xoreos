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

#include "graphics/camera.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/model.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/fps.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/loadprogress.h"
#include "engines/aurora/resources.h"
#include "engines/aurora/model.h"
#include "engines/aurora/camera.h"

#include "engines/jade/gui/main/main.h"

#include "engines/jade/jade.h"
#include "engines/jade/modelloader.h"
#include "engines/jade/console.h"
#include "engines/jade/module.h"

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

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	if (ConfigMan.getBool("showfps", false)) {
		_fps = new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13));
		_fps->show();
	}

	main();

	deinit();
}

void JadeEngine::init() {
	LoadProgress progress(15);

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

	progress.step("Successfully initialized the engine");
}

void JadeEngine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	// Some new file types with the same function as old ones re-use the type ID
	ResMan.addTypeAlias(Aurora::kFileTypeBTC, Aurora::kFileTypeCRE);
	ResMan.addTypeAlias(Aurora::kFileTypeBTP, Aurora::kFileTypePLA);
	ResMan.addTypeAlias(Aurora::kFileTypeBTT, Aurora::kFileTypeTRG);

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
	registerModelLoader(new JadeModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatABC);
	FontMan.addAlias("sava"   , "asian");
	FontMan.addAlias("cerigo" , "asian");
	FontMan.addAlias("fnt_gui", "asian");
}

void JadeEngine::initCursors() {
	CursorMan.add("ui_cursor32", "default");

	CursorMan.setDefault("default");
}

void JadeEngine::initConfig() {
}

void JadeEngine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "JADE_moduleDir",
		Common::FilePath::findSubDirectory(_baseDirectory, "data", true));
}

void JadeEngine::deinit() {
	delete _fps;
}

void JadeEngine::playIntroVideos() {
	playVideo("black");
	playVideo("publisher");
	playVideo("bwlogo");
	playVideo("graymatr");
	playVideo("attract");
}

void JadeEngine::main() {
	Console console;
	Module module(console);

	console.setModule(&module);

	module.load("j01_town");
	module.run();
}

} // End of namespace Jade

} // End of namespace Engines

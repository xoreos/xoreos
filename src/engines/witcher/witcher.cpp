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
 *  Engine class handling The Witcher
 */

#include "src/common/util.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/stream.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"
#include "src/aurora/error.h"

#include "src/graphics/camera.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/fps.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"
#include "src/engines/aurora/camera.h"

#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/modelloader.h"

namespace Engines {

namespace Witcher {

const WitcherEngineProbe kWitcherEngineProbe;

const Common::UString WitcherEngineProbe::kGameName = "The Witcher";

WitcherEngineProbe::WitcherEngineProbe() {
}

WitcherEngineProbe::~WitcherEngineProbe() {
}

Aurora::GameID WitcherEngineProbe::getGameID() const {
	return Aurora::kGameIDWitcher;
}

const Common::UString &WitcherEngineProbe::getGameName() const {
	return kGameName;
}

bool WitcherEngineProbe::probe(const Common::UString &directory,
                               const Common::FileList &UNUSED(rootFiles)) const {

	// There should be a system directory
	Common::UString systemDir = Common::FilePath::findSubDirectory(directory, "system", true);
	if (systemDir.empty())
		return false;

	// The system directory has to be readable
	Common::FileList systemFiles;
	if (!systemFiles.addDirectory(systemDir))
		return false;

	// If either witcher.ini or witcher.exe exists, this should be a valid path
	return systemFiles.containsGlob(".*/witcher.(exe|ini)", true);
}

bool WitcherEngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *WitcherEngineProbe::createEngine() const {
	return new WitcherEngine;
}


WitcherEngine::WitcherEngine() {
}

WitcherEngine::~WitcherEngine() {
}

void WitcherEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playVideo("publisher");
	playVideo("developer");
	playVideo("engine");
	playVideo("intro");
	playVideo("title");
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	bool showFPS = ConfigMan.getBool("showfps", false);

	Graphics::Aurora::FPS *fps = 0;
	if (showFPS) {
		fps = new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13));
		fps->show();
	}

	playSound("m1_axem00020005", Sound::kSoundTypeVoice);

	CameraMan.setPosition(0.0, 1.0, 0.0);

	Graphics::Aurora::Model *model = loadModelObject("cm_naked3");

	model->setRotation(0.0, 0.0, 180.0);
	model->setPosition(0.0, 2.0, 0.0);
	model->show();

	EventMan.enableKeyRepeat();

	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event))
			handleCameraInput(event);

		CameraMan.update();
		EventMan.delay(10);
	}

	EventMan.enableKeyRepeat(0);

	delete model;
	delete fps;
}

void WitcherEngine::init() {
	LoadProgress progress(11);

	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_target);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveEXE, "system");
	ResMan.addArchiveDir(Aurora::kArchiveKEY, "data");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data/voices");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data/modules/!final");

	progress.step("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "main.key", 1);

	progress.step("Loading the localized base KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "localized.key", 10);

	progress.step("Loading the English language KEYs");
	indexMandatoryArchive(Aurora::kArchiveKEY, "lang_3.key", 20);
	indexMandatoryArchive(Aurora::kArchiveKEY, "M1_3.key"  , 21);
	indexMandatoryArchive(Aurora::kArchiveKEY, "M2_3.key"  , 22);

	progress.step("Indexing extra resources");
	indexOptionalDirectory("data/movies"   , 0, -1, 30);
	indexOptionalDirectory("data/music"    , 0, -1, 31);
	indexOptionalDirectory("data/sounds"   , 0, -1, 32);
	indexOptionalDirectory("data/cutscenes", 0, -1, 33);
	indexOptionalDirectory("data/dialogues", 0, -1, 34);
	indexOptionalDirectory("data/fx"       , 0, -1, 35);
	indexOptionalDirectory("data/meshes"   , 0, -1, 36);
	indexOptionalDirectory("data/quests"   , 0, -1, 37);
	indexOptionalDirectory("data/scripts"  , 0, -1, 38);
	indexOptionalDirectory("data/templates", 0, -1, 39);
	indexOptionalDirectory("data/textures" , 0, -1, 40);

	indexOptionalDirectory("data", ".*\\.bik", 0, 41);

	progress.step("Indexing Windows-specific resources");
	indexMandatoryArchive(Aurora::kArchiveEXE, "witcher.exe", 42);

	progress.step("Indexing override files");
	indexOptionalDirectory("data/override", 0, 0, 50);

	progress.step("Loading game cursors");
	initCursors();

	progress.step("Registering file formats");
	registerModelLoader(new WitcherModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTTF);

	progress.step("Successfully initialized the engine");
}

void WitcherEngine::initCursors() {
	CursorMan.add("cursor0" , "default"  , "up"  );
	CursorMan.add("cursor1" , "default"  , "down");

	CursorMan.setDefault("default", "up");
}

} // End of namespace Witcher

} // End of namespace Engines

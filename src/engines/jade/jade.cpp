/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

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

bool JadeEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains(".*/JadeEmpire.exe", true))
		return true;

	return false;
}

bool JadeEngineProbe::probe(Common::SeekableReadStream &stream) const {
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
	initCursors();

	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	/*
	playVideo("black");
	playVideo("publisher");
	playVideo("bwlogo");
	playVideo("graymatr");
	playVideo("attract");
	if (EventMan.quitRequested())
		return;
	*/

	playSound("musicbank00046", Sound::kSoundTypeMusic, true);

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}
}

void JadeEngine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);
	indexMandatoryDirectory("", 0, 0, 1);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "data/bips");

	ResMan.addArchiveDir(Aurora::kArchiveRIM, "data", true);

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 1);

	status("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveERF, "loadscreens.mod"   , 10);
	indexMandatoryArchive(Aurora::kArchiveERF, "players.mod"       , 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global-a.rim"      , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "ingamemenu-a.rim"  , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globalunload-a.rim", 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "minigame-a.rim"    , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "miniglobal-a.rim"  , 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mmenu-a.rim"       , 17);

	status("Indexing extra font resources");
	indexMandatoryDirectory("fonts"   , 0, -1, 20);
	status("Indexing extra sound resources");
	indexMandatoryDirectory("sound"   , 0, -1, 21);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"  , 0, -1, 22);
	status("Indexing extra shader resources");
	indexMandatoryDirectory("shaderpc", 0, -1, 23);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 30);

	if (EventMan.quitRequested())
		return;

	status("Loading main talk table");
	TalkMan.addMainTable("dialog");
}

void JadeEngine::initCursors() {
}

} // End of namespace Jade

} // End of namespace Engines

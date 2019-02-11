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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords (debug) console.
 */

#include <algorithm>

#include <boost/bind.hpp>

#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/configman.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/kotorbase/room.h"

#include "src/engines/kotor2/console.h"
#include "src/engines/kotor2/kotor2.h"
#include "src/engines/kotor2/game.h"
#include "src/engines/kotor2/module.h"
#include "src/engines/kotor2/area.h"
#include "src/engines/kotor2/creature.h"

namespace Engines {

namespace KotOR2 {

Console::Console(KotOR2Engine &engine) :
	::Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
	_engine(&engine), _maxSizeMusic(0) {

	registerCommand("exitmodule"          , boost::bind(&Console::cmdExitModule          , this, _1),
			"Usage: exitmodule\nExit the module, returning to the main menu");
	registerCommand("listmodules"         , boost::bind(&Console::cmdListModules         , this, _1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule"          , boost::bind(&Console::cmdLoadModule          , this, _1),
			"Usage: loadmodule <module>\nLoad and enter the specified module");
	registerCommand("listmusic"           , boost::bind(&Console::cmdListMusic           , this, _1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"           , boost::bind(&Console::cmdStopMusic           , this, _1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"           , boost::bind(&Console::cmdPlayMusic           , this, _1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("flycam"              , boost::bind(&Console::cmdFlyCam              , this, _1),
			"Usage: flycam\nToggle free roam camera mode");
	registerCommand("showwalkmesh"        , boost::bind(&Console::cmdShowWalkmesh        , this, _1),
			"Usage: showwalkmesh\nToggle walkmesh display");
	registerCommand("showtriggers"        , boost::bind(&Console::cmdShowTriggers        , this, _1),
			"Usage: showtriggers\nToggle triggers display");
	registerCommand("getpcroom"           , boost::bind(&Console::cmdGetPCRoom           , this, _1),
			"Usage: getpcroom\nGet a room PC is in");
	registerCommand("listroomsvisiblefrom", boost::bind(&Console::cmdListRoomsVisibleFrom, this, _1),
			"Usage: listroomsvisiblefrom <room>\nList rooms that are visible from the specified room");
	registerCommand("playanim"            , boost::bind(&Console::cmdPlayAnim            , this, _1),
			"Usage: playanim <base> [<head>]\nPlay the specified animations on the active object");
}

Console::~Console() {
}

void Console::updateCaches() {
	::Engines::Console::updateCaches();

	updateModules();
	updateMusic();
}

void Console::updateModules() {
	Game::getModules(_modules);

	setArguments("loadmodule", _modules);
}

void Console::updateMusic() {
	_music.clear();
	_maxSizeMusic = 0;

	const Common::FileList music(Common::FilePath::findSubDirectory(ResMan.getDataBase(), "streammusic", true));

	for (Common::FileList::const_iterator m = music.begin(); m != music.end(); ++m) {
		if (!Common::FilePath::getExtension(*m).equalsIgnoreCase(".wav"))
			continue;

		_music.push_back(Common::FilePath::getStem(*m));

		_maxSizeMusic = MAX(_maxSizeMusic, _music.back().size());
	}

	std::sort(_music.begin(), _music.end(), Common::UString::iless());
	setArguments("playmusic", _music);
}

void Console::cmdExitModule(const CommandLine &UNUSED(cl)) {
	hide();
	_engine->getGame().getModule().exit();
}

void Console::cmdListModules(const CommandLine &UNUSED(cl)) {
	updateModules();

	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m)
		printf("%s (\"%s\")", m->c_str(), Module::getName(*m).c_str());
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	for (std::vector<Common::UString>::iterator m = _modules.begin(); m != _modules.end(); ++m) {
		if (m->equalsIgnoreCase(cl.args)) {
			hide();
			_engine->getGame().getModule().load(cl.args);
			return;
		}
	}

	printf("No such module \"%s\"", cl.args.c_str());
}

void Console::cmdListMusic(const CommandLine &UNUSED(cl)) {
	updateMusic();
	printList(_music, _maxSizeMusic);
}

void Console::cmdStopMusic(const CommandLine &UNUSED(cl)) {
	_engine->getGame().stopMusic();
}

void Console::cmdPlayMusic(const CommandLine &cl) {
	_engine->getGame().playMusic(cl.args);
}

void Console::cmdFlyCam(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleFreeRoamCamera();
}

void Console::cmdShowWalkmesh(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleWalkmesh();
}

void Console::cmdShowTriggers(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleTriggers();
}

void Console::cmdGetPCRoom(const CommandLine &UNUSED(cl)) {
	const KotOR::Room *room = _engine->getGame().getModule().getPC()->getRoom();
	printf("%s", room->getResRef().c_str());
}

void Console::cmdListRoomsVisibleFrom(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	const std::vector<Common::UString> &rooms = _engine->getGame().getModule().getCurrentArea()->getRoomsVisibleFrom(cl.args);
	for (std::vector<Common::UString>::const_iterator r = rooms.begin();
			r != rooms.end(); ++r) {
		printf("%s", r->c_str());
	}
}

void Console::cmdPlayAnim(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}
	std::vector<Common::UString> anims;
	Common::UString::split(cl.args, ' ', anims);
	size_t animCount = anims.size();
	_engine->getGame().getModule().playAnimationOnActiveObject(anims[0],
			animCount >= 2 ? anims[1] : "");
}

} // End of namespace KotOR2

} // End of namespace Engines

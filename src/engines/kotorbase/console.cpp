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
 *  Base debug console for KotOR games.
 */

#include <functional>

#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/strutil.h"

#include "src/aurora/resman.h"

#include "src/engines/kotorbase/console.h"
#include "src/engines/kotorbase/engine.h"
#include "src/engines/kotorbase/game.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/area.h"
#include "src/engines/kotorbase/room.h"
#include "src/engines/kotorbase/creature.h"

namespace Engines {

namespace KotORBase {

Console::Console(KotOREngine &engine) :
		Engines::Console(engine, Graphics::Aurora::kSystemFontMono, 13),
		_engine(&engine),
		_maxSizeMusic(0) {

	registerCommand("exitmodule"          , std::bind(&Console::cmdExitModule          , this, std::placeholders::_1),
			"Usage: exitmodule\nExit the module, returning to the main menu");
	registerCommand("listmodules"         , std::bind(&Console::cmdListModules         , this, std::placeholders::_1),
			"Usage: listmodules\nList all modules");
	registerCommand("loadmodule"          , std::bind(&Console::cmdLoadModule          , this, std::placeholders::_1),
			"Usage: loadmodule <module>\nLoad and enter the specified module");
	registerCommand("listmusic"           , std::bind(&Console::cmdListMusic           , this, std::placeholders::_1),
			"Usage: listmusic\nList all available music resources");
	registerCommand("stopmusic"           , std::bind(&Console::cmdStopMusic           , this, std::placeholders::_1),
			"Usage: stopmusic\nStop the currently playing music resource");
	registerCommand("playmusic"           , std::bind(&Console::cmdPlayMusic           , this, std::placeholders::_1),
			"Usage: playmusic [<music>]\nPlay the specified music resource. "
			"If none was specified, play the default area music.");
	registerCommand("flycam"              , std::bind(&Console::cmdFlyCam              , this, std::placeholders::_1),
			"Usage: flycam\nToggle free roam camera mode");
	registerCommand("showwalkmesh"        , std::bind(&Console::cmdShowWalkmesh        , this, std::placeholders::_1),
			"Usage: showwalkmesh\nToggle walkmesh display");
	registerCommand("showtriggers"        , std::bind(&Console::cmdShowTriggers        , this, std::placeholders::_1),
			"Usage: showtriggers\nToggle triggers display");
	registerCommand("getpcroom"           , std::bind(&Console::cmdGetPCRoom           , this, std::placeholders::_1),
			"Usage: getpcroom\nGet a room PC is in");
	registerCommand("listroomsvisiblefrom", std::bind(&Console::cmdListRoomsVisibleFrom, this, std::placeholders::_1),
			"Usage: listroomsvisiblefrom <room>\nList rooms that are visible from the specified room");
	registerCommand("playanim"            , std::bind(&Console::cmdPlayAnim            , this, std::placeholders::_1),
			"Usage: playanim <base> [<head>]\nPlay the specified animations on the active object");
	registerCommand("additem"             , std::bind(&Console::cmdAddItem             , this, std::placeholders::_1),
			"Usage: additem <item> [<count>]\nAdd the specified item to the active object");
	registerCommand("getactiveobject"     , std::bind(&Console::cmdGetActiveObject     , this, std::placeholders::_1),
			"Usage: getactiveobject\nGet a tag of the active object");
	registerCommand("actionmovetoobject"  , std::bind(&Console::cmdActionMoveToObject  , this, std::placeholders::_1),
			"Usage: actionmovetoobject <target> [<range>]\nMake the active creature move to a specified object");
	registerCommand("describe"            , std::bind(&Console::cmdDescribe            , this, std::placeholders::_1),
			"Usage: describe\nDescribe the active object or the party leader");
}

void Console::updateCaches() {
	Engines::Console::updateCaches();

	updateModules();
	updateMusic();
}

void Console::updateModules() {
	_modules = _engine->getGame().getModules();

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
		printf("%s (\"%s\")", m->c_str(), KotORBase::Module::getName(*m, getModuleDirOptionName()).c_str());
}

void Console::cmdLoadModule(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	if (_engine->getGame().hasModule(cl.args)) {
		hide();
		_engine->getGame().getModule().load(cl.args);
		return;
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
	_engine->getGame().getModule().toggleFlyCamera();
}

void Console::cmdShowWalkmesh(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleWalkmesh();
}

void Console::cmdShowTriggers(const CommandLine &UNUSED(cl)) {
	_engine->getGame().getModule().toggleTriggers();
}

void Console::cmdGetPCRoom(const CommandLine &UNUSED(cl)) {
	const Room *room = _engine->getGame().getModule().getPC()->getRoom();
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

void Console::cmdAddItem(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	std::vector<Common::UString> args;
	Common::UString::split(cl.args, ' ', args);
	int count = 1;
	if (args.size() >= 2)
		Common::parseString(args[1], count);

	_engine->getGame().getModule().addItemToActiveObject(args[0], count);
}

void Console::cmdGetActiveObject(const CommandLine &UNUSED(cl)) {
	Area *area = _engine->getGame().getModule().getCurrentArea();
	Object *object = area->getActiveObject();

	if (!object) {
		print("No object active");
		return;
	}

	printf("Active object: %s", object->getTag().c_str());
}

void Console::cmdActionMoveToObject(const CommandLine &cl) {
	if (cl.args.empty()) {
		printCommandHelp(cl.cmd);
		return;
	}

	std::vector<Common::UString> args;
	Common::UString::split(cl.args, ' ', args);
	float range = 1.0f;

	if (args.size() > 1)
		Common::parseString(args[1], range);

	Area *area = _engine->getGame().getModule().getCurrentArea();

	Creature *creature = ObjectContainer::toCreature(area->getActiveObject());
	if (!creature) {
		print("Active object is not a creature");
		return;
	}

	Object *object = area->getObjectByTag(args[0]);
	if (!object) {
		printf("Object with tag \"%s\" not found", args[0].c_str());
		return;
	}

	float x, y, z;
	object->getPosition(x, y, z);

	Action action(kActionMoveToPoint);
	action.location = glm::vec3(x, y, z);
	action.range = range;

	creature->clearActions();
	creature->addAction(action);
}

void Console::cmdDescribe(const CommandLine &UNUSED(cl)) {
	Object *object = _engine->getGame().getModule().getCurrentArea()->getActiveObject();
	if (!object) {
		object = _engine->getGame().getModule().getPartyLeader();
		if (!object)
			return;
	}
	Creature *creature = ObjectContainer::toCreature(object);
	if (creature) {
		printf("name=\"%s\" tag=\"%s\" temp=\"%s\" hp=%u maxhp=%u str=%u dex=%u con=%u int=%u wis=%u cha=%u",
		       creature->getName().c_str(),
		       creature->getTag().c_str(),
		       creature->getTemplateResRef().c_str(),
		       creature->getCurrentHitPoints(),
		       creature->getMaxHitPoints(),
		       creature->getAbilityScore(kAbilityStrength),
		       creature->getAbilityScore(kAbilityDexterity),
		       creature->getAbilityScore(kAbilityConstitution),
		       creature->getAbilityScore(kAbilityIntelligence),
		       creature->getAbilityScore(kAbilityWisdom),
		       creature->getAbilityScore(kAbilityCharisma));
	} else {
		printf("name=\"%s\" tag=\"%s\" temp=\"%s\"",
		       object->getName().c_str(),
		       object->getTag().c_str(),
		       object->getTemplateResRef().c_str());
	}
}

} // End of namespace KotORBase

} // End of namespace Engines

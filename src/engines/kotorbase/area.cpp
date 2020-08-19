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
 *  The context holding an area in KotOR games.
 */

#include <memory>
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/maths.h"

#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/localpathfinding.h"

#include "src/engines/kotorbase/room.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/placeable.h"
#include "src/engines/kotorbase/door.h"
#include "src/engines/kotorbase/waypoint.h"
#include "src/engines/kotorbase/trigger.h"
#include "src/engines/kotorbase/sound.h"
#include "src/engines/kotorbase/area.h"
#include "src/engines/kotorbase/module.h"
#include "src/engines/kotorbase/actionexecutor.h"
#include "src/engines/kotorbase/creaturesearch.h"

#include "src/engines/kotorbase/path/pathfinding.h"
#include "src/engines/kotorbase/path/objectwalkmesh.h"
#include "src/engines/kotorbase/path/doorwalkmesh.h"

namespace Engines {

namespace KotORBase {

Area::Area(Module &module, const Common::UString &resRef) :
		Object(kObjectTypeArea),
		_module(&module),
		_resRef(resRef),
		_visible(false),
		_activeObject(0),
		_highlightAll(false),
		_triggersVisible(false),
		_activeTrigger(0),
		_walkmeshInvisible(true) {

	_pathfinding = new Pathfinding(_module->getWalkableSurfaces());
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding = new Engines::LocalPathfinding(_pathfinding);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);

	try {
		load();
	} catch (...) {
		clear();
		throw;
	}

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	_module->removeObject(*this);

	hide();

	removeFocus();

	clear();

	delete _localPathfinding;
	delete _pathfinding;
}

void Area::load() {
	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRooms();

	_are = std::make_unique<Aurora::GFF3File>(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(_are->getTopLevel());

	Aurora::GFF3File git(_resRef, Aurora::kFileTypeGIT, MKTAG('G', 'I', 'T', ' '));
	loadGIT(git.getTopLevel());
}

void Area::clear() {
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		_module->removeObject(**o);

	_objects.clear();
	_creatures.clear();
	_rooms.clear();
	_triggers.clear();
	_situatedObjects.clear();
	_activeTrigger = 0;
}

uint32_t Area::getMusicDayTrack() const {
	return _musicDayTrack;
}

uint32_t Area::getMusicNightTrack() const {
	return _musicNightTrack;
}

uint32_t Area::getMusicBattleTrack() const {
	return _musicBattleTrack;
}

void Area::setMusicDayTrack(uint32_t track) {
	_musicDayTrack = track;
	_musicDay      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicNightTrack(uint32_t track) {
	_musicNightTrack = track;
	_musicNight      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicBattleTrack(uint32_t track) {
	_musicBattleTrack = track;

	if (_musicBattleTrack != Aurora::kStrRefInvalid) {
		const Aurora::TwoDAFile &ambientMusic = TwoDAReg.get2DA("ambientmusic");

		// Normal battle music
		_musicBattle = ambientMusic.getRow(_musicBattleTrack).getString("Resource");

		// Battle stingers
		Common::UString stinger[3];
		stinger[0] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger1");
		stinger[1] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger2");
		stinger[2] = ambientMusic.getRow(_musicBattleTrack).getString("Stinger3");

		_musicBattleStinger.clear();
		for (int i = 0; i < 3; i++)
			if (!stinger[i].empty())
				_musicBattleStinger.push_back(stinger[i]);
	}
}

void Area::stopSound() {
	stopAmbientMusic();
	stopAmbientSound();
}

void Area::stopAmbientMusic() {
	SoundMan.stopChannel(_ambientMusic);
}

void Area::stopAmbientSound() {
	SoundMan.stopChannel(_ambientSound);
}

void Area::playAmbientMusic(Common::UString music) {
	stopAmbientMusic();

	// TODO: Area::playAmbientMusic(): Day/Night
	if (music.empty())
		music = _musicDay;

	if (music.empty())
		return;

	_ambientMusic = ::Engines::playSound(music, Sound::kSoundTypeMusic, true);
}

void Area::playAmbientSound(Common::UString sound) {
	stopAmbientSound();

	// TODO: Area::playAmbientSound():  Day/Night
	if (sound.empty())
		sound = _ambientDay;

	if (sound.empty())
		return;

	_ambientSound = ::Engines::playSound(sound, Sound::kSoundTypeSFX, true, _ambientDayVol);
}

int Area::getNorthAxis() {
	return _northAxis;
}

void Area::getMapPoint1(float &x, float &y) {
	x = _mapPt1X;
	y = _mapPt1Y;
}

void Area::getMapPoint2(float &x, float &y) {
	x = _mapPt2X;
	y = _mapPt2Y;
}

void Area::getWorldPoint1(float &x, float &y) {
	x = _worldPt1X;
	y = _worldPt1Y;
}

void Area::getWorldPoint2(float &x, float &y) {
	x = _worldPt2X;
	y = _worldPt2Y;
}

void Area::show() {
	if (_visible)
		return;

	GfxMan.lockFrame();

	std::set<Common::UString> visibleRooms(getRoomsVisibleByPartyLeader());

	// Show rooms
	for (auto &r : _rooms) {
		if (visibleRooms.find(r->getResRef()) != visibleRooms.end())
			r->show();
	}

	// Show objects
	for (auto &o : _objects) {
		Door *door = ObjectContainer::toDoor(o);
		if (door) {
			door->show();
			continue;
		}

		const Room *room = o->getRoom();
		if (room && room->isVisible())
			o->show();
	}

	// Show walkmesh
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);

	GfxMan.unlockFrame();

	// Play music and sound
	playAmbientSound();
	playAmbientMusic();

	_visible = true;
}

void Area::hide() {
	if (!_visible)
		return;

	removeFocus();

	stopSound();

	GfxMan.lockFrame();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	// Hide rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->hide();

	// Hide walkmesh
	_pathfinding->showWalkmesh(false);
	_localPathfinding->showWalkmesh(false);

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::loadLYT() {
	try {
		std::unique_ptr<Common::SeekableReadStream> lyt(ResMan.getResource(_resRef, Aurora::kFileTypeLYT));
		if (!lyt)
			throw Common::Exception("No such LYT");

		_lyt.load(*lyt);

	} catch (Common::Exception &e) {
		e.add("Failed loading LYT \"%s\"", _resRef.c_str());
		throw;
	}
}

void Area::loadVIS() {
	try {
		std::unique_ptr<Common::SeekableReadStream> vis(ResMan.getResource(_resRef, Aurora::kFileTypeVIS));
		if (!vis)
			throw Common::Exception("No such VIS");

		_vis.load(*vis);

	} catch (Common::Exception &e) {
		e.add("Failed loading VIS \"%s\"", _resRef.c_str());
		throw;
	}
}

void Area::loadARE(const Aurora::GFF3Struct &are) {
	// Tag
	_tag = are.getString("Tag");

	// Name
	_name = are.getString("Name");

	// Minimap data
	const Aurora::GFF3Struct &map = are.getStruct("Map");
	_mapPt1X = map.getDouble("MapPt1X");
	_mapPt1Y = map.getDouble("MapPt1Y");
	_mapPt2X = map.getDouble("MapPt2X");
	_mapPt2Y = map.getDouble("MapPt2Y");
	_worldPt1X = map.getDouble("WorldPt1X");
	_worldPt1Y = map.getDouble("WorldPt1Y");
	_worldPt2X = map.getDouble("WorldPt2X");
	_worldPt2Y = map.getDouble("WorldPt2Y");
	_northAxis = map.getSint("NorthAxis");

	// Camera style
	loadCameraStyle(are.getUint("CameraStyle"));

	// Scripts
	readScripts(are);
}

void Area::loadCameraStyle(uint32_t id) {
	const Aurora::TwoDAFile &tda = TwoDAReg.get2DA("camerastyle");
	const Aurora::TwoDARow &row = tda.getRow(id);
	_cameraStyle.distance = row.getFloat("distance");
	_cameraStyle.pitch = row.getFloat("pitch");
	_cameraStyle.height = row.getFloat("height");
	_cameraStyle.viewAngle = row.getFloat("viewangle");
}

void Area::loadGIT(const Aurora::GFF3Struct &git) {
	if (git.hasField("AreaProperties"))
		loadProperties(git.getStruct("AreaProperties"));

	if (git.hasField("WaypointList"))
		loadWaypoints(git.getList("WaypointList"));

	if (git.hasField("Placeable List"))
		loadPlaceables(git.getList("Placeable List"));

	if (git.hasField("Door List"))
		loadDoors(git.getList("Door List"));

	if (git.hasField("Creature List"))
		loadCreatures(git.getList("Creature List"));

	if (git.hasField("SoundList"))
		loadSounds(git.getList("SoundList"));

	if (git.hasField("TriggerList"))
		loadTriggers(git.getList("TriggerList"));
}

void Area::loadProperties(const Aurora::GFF3Struct &props) {
	// Ambient sound

	const Aurora::TwoDAFile &ambientSound = TwoDAReg.get2DA("ambientsound");

	uint32_t ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32_t ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32_t ambientDayVol   = CLIP<uint32_t>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32_t ambientNightVol = CLIP<uint32_t>(props.getUint("AmbientSndNightVol", 127), 0, 127);

	_ambientDayVol   = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientDayVol   / 127.0f)));
	_ambientNightVol = 1.25f * (1.0f - (1.0f / powf(5.0f, ambientNightVol / 127.0f)));

	// TODO: PresetInstance0 - PresetInstance7


	// Ambient music

	setMusicDayTrack  (props.getUint("MusicDay"   , Aurora::kStrRefInvalid));
	setMusicNightTrack(props.getUint("MusicNight" , Aurora::kStrRefInvalid));

	// Battle music

	setMusicBattleTrack(props.getUint("MusicBattle", Aurora::kStrRefInvalid));
}

void Area::loadRooms() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (Aurora::LYTFile::RoomArray::const_iterator r = rooms.begin(); r != rooms.end(); ++r) {
		Room *room = new Room(r->model, r->x, r->y, r->z);
		_rooms.push_back(room);
		_pathfinding->addRoom(room);
	}

	_pathfinding->connectRooms();
}

void Area::loadObject(Object &object) {
	_objects.push_back(&object);
	_module->addObject(object);

	if (!object.isStatic())
		addToObjectMap(&object);

	notifyObjectMoved(object);
}

void Area::loadWaypoints(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator w = list.begin(); w != list.end(); ++w) {
		Waypoint *waypoint = new Waypoint(**w);

		loadObject(*waypoint);
	}
}

void Area::loadPlaceables(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
		_situatedObjects.push_back(placeable);
		_localPathfinding->addStaticObjects(new ObjectWalkmesh(placeable));
	}
}

void Area::loadDoors(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(*_module, **d);

		loadObject(*door);
		_situatedObjects.push_back(door);
		_localPathfinding->addStaticObjects(new DoorWalkmesh(door));
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = _module->createCreature(**c);
		addCreature(creature);
	}
}

void Area::loadTriggers(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator t = list.begin(); t != list.end(); ++t) {
		Trigger *trigger = new Trigger(**t);

		loadObject(*trigger);
		_triggers.push_back(trigger);
	}
}

void Area::loadSounds(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		SoundObject *soundObject = new SoundObject(**c);

		loadObject(*soundObject);
	}
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	bool hasMove = false;
	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if        (e->type == Events::kEventMouseMove) { // Moving the mouse
			hasMove = true;
		} else if (e->type == Events::kEventMouseUp) { // Clicking
			if (e->button.button == SDL_BUTTON_LMASK) {
				checkActive(e->button.x, e->button.y);
				click(e->button.x, e->button.y);
			}
		} else if (e->type == Events::kEventKeyDown) { // Holding down Left Shift
			if (e->key.keysym.sym == SDLK_LSHIFT)
				highlightAll(true);
		} else if (e->type == Events::kEventKeyUp) {   // Releasing Left Shift
			if (e->key.keysym.sym == SDLK_LSHIFT)
				highlightAll(false);
		}
	}

	_eventQueue.clear();

	if (hasMove)
		checkActive();
}

Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(Object *object) {
	if (object == _activeObject)
		return;

	if (_activeObject)
		_activeObject->leave();

	_module->leaveObject(_activeObject);

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();

	_module->enterObject(_activeObject);
}

void Area::checkActive(int x, int y) {
	if (_highlightAll)
		return;

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	if ((x < 0) || (y < 0))
		CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::click(int x, int y) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	Object *o = getObjectAt(x, y);
	if (!o)
		return;

	_module->clickObject(o);
}

void Area::highlightAll(bool enabled) {
	if (_highlightAll == enabled)
		return;

	_highlightAll = enabled;

	for (ObjectMap::iterator o = _objectMap.begin(); o != _objectMap.end(); ++o)
		if (o->second->isClickable())
			o->second->highlight(enabled);
}

void Area::removeFocus() {
	if (_activeObject)
		_activeObject->leave();

	_activeObject = 0;
}

void Area::notifyCameraMoved() {
	if (!_module->isConversationActive())
		checkActive();
}

float Area::evaluateElevation(float x, float y) {
	return _pathfinding->getHeight(x, y, true);
}

bool Area::walkable(const glm::vec3 &orig, const glm::vec3 &dest) const {
	std::vector<glm::vec3> path;
	path.push_back(orig);
	path.push_back(dest);
	_localPathfinding->buildWalkmeshAround(path, 0.1f);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);

	return _localPathfinding->walkable(dest);
}

void Area::toggleWalkmesh() {
	_walkmeshInvisible = !_walkmeshInvisible;
	_pathfinding->showWalkmesh(!_walkmeshInvisible);
	_localPathfinding->showWalkmesh(!_walkmeshInvisible);
}

bool Area::rayTest(const glm::vec3 &orig, const glm::vec3 &dest, glm::vec3 &intersect) const {
	return _localPathfinding->findIntersection(orig, dest, intersect);
}

void Area::toggleTriggers() {
	_triggersVisible = !_triggersVisible;
	for (std::vector<Trigger *>::const_iterator it = _triggers.begin();
			it != _triggers.end();
			++it) {
		(*it)->setVisible(_triggersVisible);
	}
}

void Area::evaluateTriggers(float x, float y) {
	Trigger *trigger = 0;

	for (std::vector<Trigger *>::iterator it = _triggers.begin();
			it != _triggers.end();
			++it) {
		Trigger *t = *it;
		if (t->contains(x, y)) {
			trigger = t;
			break;
		}
	}

	if (_activeTrigger != trigger) {
		if (_activeTrigger)
			_activeTrigger->runScript(kScriptExit, _activeTrigger, _module->getPC());
		_activeTrigger = trigger;
		if (_activeTrigger)
			_activeTrigger->runScript(kScriptEnter, _activeTrigger, _module->getPC());
	}
}

void Area::showAllRooms() {
	GfxMan.pauseAnimations();

	for (RoomList::iterator r = _rooms.begin();
			r != _rooms.end(); ++r) {
		if (!(*r)->isVisible())
			(*r)->show();
	}

	for (ObjectList::iterator o = _objects.begin();
			o != _objects.end(); ++o) {
		if (!(*o)->isVisible())
			(*o)->show();
	}

	GfxMan.resumeAnimations();
}

void Area::notifyObjectMoved(Object &o) {
	float x, y, _;
	o.getPosition(x, y, _);
	o.setRoom(_pathfinding->getRoomAt(x, y));

	if (o.getType() == kObjectTypeCreature)
		updatePerception((Creature &)o);
}

void Area::updatePerception(Creature &subject) {
	for (auto &object : _creatures) {
		if (object == &subject || object->isDead())
			continue;

		subject.updatePerception(*object);
	}
}

void Area::notifyPartyLeaderMoved() {
	Creature *partyLeader = _module->getPartyLeader();

	const Room *prevRoom = partyLeader->getRoom();
	notifyObjectMoved(*partyLeader);
	const Room *currentRoom = partyLeader->getRoom();

	if (currentRoom != prevRoom)
		updateRoomsVisiblity();
}

bool Area::isMinigame() {
	return _are->getTopLevel().hasField("MiniGame");
}

const Aurora::GFF3Struct &Area::getMinigame() {
	return _are->getTopLevel().getStruct("MiniGame");
}

std::set<Common::UString> Area::getRoomsVisibleByPartyLeader() const {
	Creature *partyLeader = _module->getPartyLeader();
	Common::UString currentRoom = partyLeader->getRoom()->getResRef();

	std::set<Common::UString> rooms { currentRoom };

	for (auto &resRef : getRoomsVisibleFrom(currentRoom)) {
		Common::UString lcResRef(resRef.toLower());
		if (rooms.find(lcResRef) == rooms.end())
			rooms.insert(lcResRef);
	}

	return rooms;
}

void Area::updateRoomsVisiblity() {
	std::set<Common::UString> visibleRooms(getRoomsVisibleByPartyLeader());

	GfxMan.pauseAnimations();

	for (auto &r : _rooms) {
		bool shouldBeVisible = visibleRooms.find(r->getResRef()) != visibleRooms.end();
		bool visible = r->isVisible();

		if (visible && !shouldBeVisible)
			r->hide();
		else if (shouldBeVisible && !visible)
			r->show();
	}

	for (auto &o : _objects) {
		const Room *room = o->getRoom();
		bool shouldBeVisible = room && room->isVisible();
		bool visible = o->isVisible();

		if (visible && !shouldBeVisible)
			o->notifyNotSeen();
		else if (shouldBeVisible && !visible)
			o->show();
	}

	GfxMan.resumeAnimations();
}

const Area::CameraStyle &Area::getCameraStyle() const {
	return _cameraStyle;
}

const std::vector<Common::UString> &Area::getRoomsVisibleFrom(const Common::UString &room) const {
	return _vis.getVisibilityArray(room);
}

Object *Area::getActiveObject() {
	return _activeObject;
}

Object *Area::getObjectByTag(const Common::UString &tag) {
	for (ObjectList::iterator o = _objects.begin();
			o != _objects.end(); ++o) {
		if ((*o)->getTag().stricmp(tag) == 0)
			return *o;
	}
	return 0;
}

Creature *Area::getNearestCreature(const Object *target, int UNUSED(nth), const CreatureSearchCriteria &criteria) const {
	// TODO: Find Nth nearest using all criterias

	Creature *result = 0;
	float lowestDistance = 0.0f;

	float x, y, z;
	target->getPosition(x, y, z);
	glm::vec3 targetPosition(x, y, z);

	for (auto c = _creatures.begin(); c != _creatures.end(); ++c) {
		if (*c == target || (*c)->isDead())
			continue;

		if (!(*c)->matchSearchCriteria(target, criteria))
			continue;

		(*c)->getPosition(x, y, z);
		float dist = glm::distance(targetPosition, glm::vec3(x, y, z));

		if ((result == 0) || (dist < lowestDistance)) {
			result = *c;
			lowestDistance = dist;
		}
	}

	return result;
}

const std::vector<Creature *> &Area::getCreatures() const {
	return _creatures;
}

void Area::processCreaturesActions(float dt) {
	ActionExecutor::ExecutionContext ctx;
	ctx.area = this;
	ctx.frameTime = dt;

	for (auto &c : _creatures) {
		if (c->isDead())
			continue;

		const Action *action = c->getCurrentAction();
		if (!action)
			continue;

		ctx.creature = c;
		ActionExecutor::execute(*action, ctx);
	}
}

void Area::handleCreaturesDeath() {
	for (auto &c : _creatures) {
		if (c->handleDeath())
			_module->signalUserDefinedEvent(c, 1007);
	}
}

void Area::addCreature(Creature *creature) {
	loadObject(*creature);
	_creatures.push_back(creature);
}

void Area::addToObjectMap(Object *object) {
	const std::list<uint32_t> &ids = object->getIDs();
	for (std::list<uint32_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
		_objectMap.insert(std::make_pair(*id, object));
}

void Area::removeObject(Object *object) {
	if (object == _activeObject) {
		_activeObject->leave();
		_module->leaveObject(_activeObject);
		_activeObject = 0;
	}

	std::vector<Creature *>::iterator crit = std::find(_creatures.begin(), _creatures.end(), object);
	if (crit != _creatures.end())
		_creatures.erase(crit);

	std::vector<Trigger *>::iterator tit = std::find(_triggers.begin(), _triggers.end(), object);
	if (tit != _triggers.end())
		_triggers.erase(tit);

	std::list<Situated *>::iterator soit = std::find(_situatedObjects.begin(), _situatedObjects.end(), object);
	if (soit != _situatedObjects.end())
		_situatedObjects.erase(soit);

	if (object == _activeTrigger)
		_activeTrigger = 0;

	if (!object->isStatic()) {
		const std::list<uint32_t> &ids = object->getIDs();

		for (std::list<uint32_t>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.erase(*id);
	}

	_module->removeObject(*object);
	_objects.remove(object);
}

} // End of namespace KotORBase

} // End of namespace Engines

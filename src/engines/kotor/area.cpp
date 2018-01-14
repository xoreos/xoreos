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
 *  The context holding a Star Wars: Knights of the Old Republic area.
 */

#include "src/common/scopedptr.h"
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

#include "src/engines/kotor/area.h"
#include "src/engines/kotor/room.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/waypoint.h"
#include "src/engines/kotor/placeable.h"
#include "src/engines/kotor/door.h"
#include "src/engines/kotor/creature.h"
#include "src/engines/kotor/trigger.h"
#include "src/engines/kotor/actionexecutor.h"
#include "src/engines/kotor/sound.h"

namespace Engines {

namespace KotOR {

Area::CameraStyle::CameraStyle()
		: distance(0.0f),
		  pitch(0.0f),
		  height(0.0f) {
}

Area::Area(Module &module, const Common::UString &resRef)
		: Object(kObjectTypeArea),
		  _module(&module),
		  _resRef(resRef),
		  _visible(false),
		  _activeObject(0),
		  _highlightAll(false),
		  _triggersVisible(false),
		  _activeTrigger(0),
		  _walkmeshInvisible(true) {

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
}

void Area::load() {
	loadLYT(); // Room layout
	loadVIS(); // Room visibilities

	loadRooms();

	Aurora::GFF3File are(_resRef, Aurora::kFileTypeARE, MKTAG('A', 'R', 'E', ' '));
	loadARE(are.getTopLevel());

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

uint32 Area::getMusicDayTrack() const {
	return _musicDayTrack;
}

uint32 Area::getMusicNightTrack() const {
	return _musicNightTrack;
}

uint32 Area::getMusicBattleTrack() const {
	return _musicBattleTrack;
}

void Area::setMusicDayTrack(uint32 track) {
	_musicDayTrack = track;
	_musicDay      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicNightTrack(uint32 track) {
	_musicNightTrack = track;
	_musicNight      = TwoDAReg.get2DA("ambientmusic").getRow(track).getString("Resource");
}

void Area::setMusicBattleTrack(uint32 track) {
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

	// Show rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

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

	GfxMan.unlockFrame();

	_visible = false;
}

void Area::loadLYT() {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> lyt(ResMan.getResource(_resRef, Aurora::kFileTypeLYT));
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
		Common::ScopedPtr<Common::SeekableReadStream> vis(ResMan.getResource(_resRef, Aurora::kFileTypeVIS));
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

void Area::loadCameraStyle(uint32 id) {
	const Aurora::TwoDAFile &tda = TwoDAReg.get2DA("camerastyle");
	const Aurora::TwoDARow &row = tda.getRow(id);
	_cameraStyle.distance = row.getFloat("distance");
	_cameraStyle.pitch = row.getFloat("pitch");
	_cameraStyle.height = row.getFloat("height");
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

	uint32 ambientDay   = props.getUint("AmbientSndDay"  , Aurora::kStrRefInvalid);
	uint32 ambientNight = props.getUint("AmbientSndNight", Aurora::kStrRefInvalid);

	_ambientDay   = ambientSound.getRow(ambientDay  ).getString("Resource");
	_ambientNight = ambientSound.getRow(ambientNight).getString("Resource");

	uint32 ambientDayVol   = CLIP<uint32>(props.getUint("AmbientSndDayVol"  , 127), 0, 127);
	uint32 ambientNightVol = CLIP<uint32>(props.getUint("AmbientSndNightVol", 127), 0, 127);

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
		_rooms.push_back(new Room(r->model, r->x, r->y, r->z));
	}
}

void Area::loadObject(KotOR::Object &object) {
	_objects.push_back(&object);
	_module->addObject(object);

	if (!object.isStatic()) {
		const std::list<uint32> &ids = object.getIDs();

		for (std::list<uint32>::const_iterator id = ids.begin(); id != ids.end(); ++id)
			_objectMap.insert(std::make_pair(*id, &object));
	}

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
	}
}

void Area::loadDoors(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator d = list.begin(); d != list.end(); ++d) {
		Door *door = new Door(*_module, **d);

		loadObject(*door);
		_situatedObjects.push_back(door);
	}
}

void Area::loadCreatures(const Aurora::GFF3List &list) {
	for (Aurora::GFF3List::const_iterator c = list.begin(); c != list.end(); ++c) {
		Creature *creature = new Creature(**c);

		loadObject(*creature);
		_creatures.push_back(creature);
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
		} else if (e->type == Events::kEventMouseDown) { // Clicking
			if (e->button.button == SDL_BUTTON_LMASK) {
				checkActive(e->button.x, e->button.y);
				click(e->button.x, e->button.y);
			}
		} else if (e->type == Events::kEventKeyDown) { // Holding down TAB
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(true);
		} else if (e->type == Events::kEventKeyUp) {   // Releasing TAB
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(false);
		}
	}

	_eventQueue.clear();

	if (hasMove)
		checkActive();
}

KotOR::Object *Area::getObjectAt(int x, int y) {
	const Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	ObjectMap::iterator o = _objectMap.find(obj->getID());
	if (o == _objectMap.end())
		return 0;

	return o->second;
}

void Area::setActive(KotOR::Object *object) {
	if (object == _activeObject)
		return;

	if (_activeObject)
		_activeObject->leave();

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();
}

void Area::checkActive(int x, int y) {
	if (_highlightAll)
		return;

	Common::StackLock lock(_mutex);

	if ((x < 0) || (y < 0))
		CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::click(int x, int y) {
	Common::StackLock lock(_mutex);

	KotOR::Object *o = getObjectAt(x, y);
	if (!o)
		return;

	o->click(_module->getPC());
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
	checkActive();
}

float Area::evaluateElevation(Object *object, float x, float y, bool doHighlight) {
	Room *room = object->getRoom();

	float result = room ? room->evaluateElevation(x, y, doHighlight) : FLT_MIN;
	if (result != FLT_MIN)
		return result;

	if (room && doHighlight)
		room->disableWalkmeshHighlight();

	for (RoomList::iterator r = _rooms.begin();
			r != _rooms.end(); ++r) {
		if (*r != room) {
			result = (*r)->evaluateElevation(x, y, doHighlight);
			if (result != FLT_MIN)
				break;
		}
	}

	return result;
}

bool Area::testCollision(const glm::vec3 &orig, const glm::vec3 &dest) const {
	for (std::list<Situated *>::const_iterator s = _situatedObjects.begin();
			s != _situatedObjects.end(); ++s) {
		if ((*s)->testCollision(orig, dest))
			return true;
	}
	return false;
}

void Area::toggleWalkmesh() {
	_walkmeshInvisible = !_walkmeshInvisible;

	for (RoomList::iterator r = _rooms.begin();
			r != _rooms.end(); ++r) {
		(*r)->setWalkmeshInvisible(_walkmeshInvisible);
	}

	for (std::list<Situated *>::iterator s = _situatedObjects.begin();
			s != _situatedObjects.end(); ++s) {
		(*s)->setWalkmeshInvisible(_walkmeshInvisible);
	}
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
			_activeTrigger->runScript(kScriptExit, this, _module->getPC());
		_activeTrigger = trigger;
		if (_activeTrigger)
			_activeTrigger->runScript(kScriptEnter, this, _module->getPC());
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
	float x, y, z;
	o.getPosition(x, y, z);
	o.setRoom(getRoomAt(x, y));
}

void Area::notifyPCMoved() {
	Creature *pc = _module->getPC();

	const Room *prevPCRoom = pc->getRoom();
	notifyObjectMoved(*pc);
	const Room *pcRoom = pc->getRoom();

	if (pcRoom == prevPCRoom)
		return;

	std::vector<Common::UString> visRooms;
	if (pcRoom) {
		visRooms.push_back(pcRoom->getResRef());

		const std::vector<Common::UString> va = _vis.getVisibilityArray(pcRoom->getResRef());
		for (std::vector<Common::UString>::const_iterator v = va.begin();
				v != va.end(); ++v) {
			Common::UString lcResRef(v->toLower());
			if (std::find(visRooms.begin(), visRooms.end(), lcResRef) == visRooms.end())
				visRooms.push_back(lcResRef);
		}
	}

	GfxMan.pauseAnimations();

	for (RoomList::iterator r = _rooms.begin();
			r != _rooms.end(); ++r) {
		bool visible = std::find(visRooms.begin(), visRooms.end(), (*r)->getResRef()) != visRooms.end();
		if (visible) {
			if (!(*r)->isVisible())
				(*r)->show();
		} else if ((*r)->isVisible())
			(*r)->hide();
	}

	for (ObjectList::iterator o = _objects.begin();
			o != _objects.end(); ++o) {
		const Room *objRoom = (*o)->getRoom();
		bool visible = objRoom && objRoom->isVisible();
		if (visible) {
			if (!(*o)->isVisible())
				(*o)->show();
		} else if ((*o)->isVisible())
			(*o)->hideSoft();
	}

	GfxMan.resumeAnimations();
}

void Area::getCameraStyle(float &distance, float &pitch, float &height) const {
	distance = _cameraStyle.distance;
	pitch = _cameraStyle.pitch;
	height = _cameraStyle.height;
}

const std::vector<Common::UString> &Area::getRoomsVisibleFrom(const Common::UString &room) const {
	return _vis.getVisibilityArray(room);
}

KotOR::Object *Area::getActiveObject() {
	return _activeObject;
}

KotOR::Object *Area::getObjectByTag(const Common::UString &tag) {
	for (ObjectList::iterator o = _objects.begin();
			o != _objects.end(); ++o) {
		if ((*o)->getTag().stricmp(tag) == 0)
			return *o;
	}
	return 0;
}

void Area::processCreaturesActions(float dt) {
	for (std::vector<Creature *>::iterator c = _creatures.begin();
			c != _creatures.end(); ++c) {
		ActionExecutor::executeActions(**c, *this, dt);
	}
}

Room *Area::getRoomAt(float x, float y) const {
	for (RoomList::const_iterator r = _rooms.begin();
			r != _rooms.end(); ++r) {
		if ((*r)->evaluateElevation(x, y) != FLT_MIN)
			return *r;
	}
	return 0;
}

} // End of namespace KotOR

} // End of namespace Engines

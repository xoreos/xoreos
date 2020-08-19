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
 *  The context holding a Sonic Chronicles: The Dark Brotherhood area.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gdafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"
#include "src/graphics/camera.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/engines/sonic/area.h"
#include "src/engines/sonic/types.h"
#include "src/engines/sonic/module.h"
#include "src/engines/sonic/areabackground.h"
#include "src/engines/sonic/areaminimap.h"
#include "src/engines/sonic/placeable.h"

static const uint32_t kAREID = MKTAG('A', 'R', 'E', ' ');

namespace Engines {

namespace Sonic {

Area::Area(Module &module, uint32_t id) : Object(kObjectTypeArea),
	_module(&module), _width(0), _height(0), _startPosX(0.0f), _startPosY(0.0f),
	_miniMapWidth(0), _miniMapHeight(0), _soundMapBank(-1), _sound(-1), _soundType(-1), _soundBank(-1),
	_numberRings(0), _numberChaoEggs(0), _activeObject(0), _highlightAll(false) {

	_id = id;
	ObjectMan.registerObject(this);

	load();

	// Tell the module that we exist
	_module->addObject(*this);
}

Area::~Area() {
	ObjectMan.unregisterObject(this);

	_module->removeObject(*this);

	hide();

	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		_module->removeObject(**o);
}

const Common::UString &Area::getName() {
	return _name;
}

uint32_t Area::getWidth() const {
	return _width;
}

uint32_t Area::getHeight() const {
	return _height;
}

float Area::getStartX() const {
	return _startPosX;
}

float Area::getStartY() const {
	return _startPosY;
}

void Area::getCameraPosition(float x, float y, float &cameraX, float &cameraY, float &cameraZ) {
	cameraX = x - kScreenWidth / 2.0f;
	cameraY = kCameraHeight;
	cameraZ = (y - kScreenHeight / 2.0f) / sin(Common::deg2rad(kCameraAngle));
}

void Area::getWorldPosition(float x, float y, float z, float &worldX, float &worldY, float &worldZ) {
	worldX = x + kScreenWidth / 2.0f;
	worldY = z;
	worldZ = (y - kScreenHeight + kScreenHeight / 2.0f) / sin(Common::deg2rad(kCameraAngle)) - kCameraHeight;

	worldZ = floor(worldZ);
}

void Area::getCameraLimits(float &minX, float &minY, float &minZ,
                           float &maxX, float &maxY, float &maxZ) const {

	minX = kScreenWidth / 2.0f;
	minY = -FLT_MAX;
	minZ = (kScreenHeight / 2.0f) / sin(Common::deg2rad(kCameraAngle));
	maxX = _width - kScreenWidth / 2.0f;
	maxY =  FLT_MAX;
	maxZ = (_height - kScreenHeight / 2.0f) / sin(Common::deg2rad(kCameraAngle));
}

void Area::show() {
	GfxMan.lockFrame();

	if (_mmPanel)
		_mmPanel->show();
	if (_bgPanel)
		_bgPanel->show();

	// Show objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->show();

	GfxMan.unlockFrame();
}

void Area::hide() {
	GfxMan.lockFrame();

	removeFocus();

	// Hide objects
	for (ObjectList::iterator o = _objects.begin(); o != _objects.end(); ++o)
		(*o)->hide();

	if (_mmPanel)
		_mmPanel->hide();
	if (_bgPanel)
		_bgPanel->hide();

	GfxMan.unlockFrame();
}

void Area::enter() {
	CameraMan.reset();

	if (_bgPanel) {
		float x, y, z, minX, minY, minZ, maxX, maxY, maxZ;

		getCameraLimits(minX, minY, minZ, maxX, maxY, maxZ);
		getCameraPosition(_startPosX, _startPosY, x, y, z);

		CameraMan.limit(minX, minY, minZ, maxX, maxY, maxZ);
		CameraMan.setOrientation(-kCameraAngle, 0.0f, 0.0f);
		CameraMan.setPosition(x, y, z);
	}

	CameraMan.update();
}

void Area::leave() {
	CameraMan.reset();
	CameraMan.update();
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	bool hasMove = false;
	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if        (e->type == Events::kEventMouseMove) {
			hasMove = true;
		} else if (e->type == Events::kEventKeyDown) {
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(true);
		} else if (e->type == Events::kEventKeyUp) {
			if (e->key.keysym.sym == SDLK_TAB)
				highlightAll(false);
		}
	}

	_eventQueue.clear();

	if (hasMove)
		checkActive();
}

void Area::load() {
	loadDefinition();
	loadBackground();
	loadMiniMap();
	loadLayout();
}

void Area::loadDefinition() {
	const Aurora::GDAFile &areas = TwoDAReg.getGDA("areas");
	if (!areas.hasRow(_id))
		throw Common::Exception("No such Area ID %u (%u)", _id, (uint)areas.getRowCount());

	_name = TalkMan.getString(areas.getInt(_id, "Name", 0xFFFFFFFF));

	_background = areas.getString(_id, "Background");
	if (_background.empty())
		throw Common::Exception("Area has no background");

	_layout = areas.getString(_id, "Layout");
	if (_layout.empty())
		throw Common::Exception("Area has no layout");

	const uint32_t tileSizeX = areas.getInt(_id, "TileSizeX");
	const uint32_t tileSizeY = areas.getInt(_id, "TileSizeY");
	if ((tileSizeX != 64) || (tileSizeY != 64))
		throw Common::Exception("Unsupported tile dimensions (%ux%u)", tileSizeX, tileSizeY);

	_width  = areas.getInt(_id, "AreaWidth");
	_height = areas.getInt(_id, "AreaHeight");
	if ((_width == 0) || (_height == 0))
		throw Common::Exception("Invalid area dimensions (%ux%u)", _width, _height);

	_startPosX = areas.getFloat(_id, "StartPosX");
	_startPosY = areas.getFloat(_id, "StartPosY");

	if ((_startPosX < 0.0f) || (_startPosY < 0.0f) || (_startPosX > _width) || (_startPosY > _height))
		throw Common::Exception("Invalid start position (%f+%f, %ux%u", _startPosX, _startPosY, _width, _height);

	_miniMap = areas.getString(_id, "MiniMapString");

	_miniMapWidth  = areas.getInt(_id, "MiniMapWidth");
	_miniMapHeight = areas.getInt(_id, "MiniMapHeight");

	_soundMap = areas.getString(_id, "SoundMap");

	_soundMapBank = areas.getInt(_id, "SoundMapBank" , -1);
	_sound        = areas.getInt(_id, "AreaSound"    , -1);
	_soundType    = areas.getInt(_id, "AreaSoundType", -1);
	_soundBank    = areas.getInt(_id, "AreaSoundBank", -1);

	_numberRings    = areas.getInt(_id, "NumberRings");
	_numberChaoEggs = areas.getInt(_id, "NumberChaoEggs");
}

void Area::loadBackground() {
	_bgPanel = std::make_unique<AreaBackground>(_background);

	if ((_bgPanel->getImageWidth() != _width) || (_bgPanel->getImageHeight() != _height))
		throw Common::Exception("Background and area dimensions don't match (%ux%u vs. %ux%u)",
		                        _bgPanel->getImageWidth(), _bgPanel->getImageHeight(), _width, _height);
}

void Area::loadMiniMap() {
	if (!_miniMap.empty())
		_mmPanel = std::make_unique<AreaMiniMap>(_miniMap);
}

void Area::loadLayout() {
	Aurora::GFF4File are(_layout, Aurora::kFileTypeARE, kAREID);
	const Aurora::GFF4Struct &areTop = are.getTopLevel();

	_tag = areTop.getString(40000);

	if (areTop.hasField(40001))
		loadPlaceables(areTop.getList(40001));
}

void Area::loadObject(Object &object) {
	_objects.push_back(&object);

	_objectMap.insert(std::make_pair(object.getModelID(), &object));
	_module->addObject(object);
}

void Area::loadPlaceables(const Aurora::GFF4List &list) {
	for (Aurora::GFF4List::const_iterator p = list.begin(); p != list.end(); ++p) {
		Placeable *placeable = new Placeable(**p);

		loadObject(*placeable);
	}
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

	_activeObject = object;

	if (_activeObject)
		_activeObject->enter();
}

void Area::checkActive() {
	if (_highlightAll)
		return;

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	int x, y;
	CursorMan.getPosition(x, y);

	setActive(getObjectAt(x, y));
}

void Area::highlightAll(bool enabled) {
	if (_highlightAll == enabled)
		return;

	_highlightAll = enabled;

	for (ObjectMap::iterator o = _objectMap.begin(); o != _objectMap.end(); ++o)
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

} // End of namespace Sonic

} // End of namespace Engines

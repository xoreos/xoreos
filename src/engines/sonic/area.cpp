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
 *  An area in Sonic.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gdafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/talkman.h"

#include "src/engines/sonic/area.h"
#include "src/engines/sonic/areabackground.h"
#include "src/engines/sonic/areaminimap.h"

static const uint32 kAREID = MKTAG('A', 'R', 'E', ' ');

namespace Engines {

namespace Sonic {

Area::Area(int32 id) : _id(id), _width(0), _height(0), _startPosX(0.0f), _startPosY(0.0f),
	_miniMapWidth(0), _miniMapHeight(0), _soundMapBank(-1), _sound(-1), _soundType(-1), _soundBank(-1),
	_numberRings(0), _numberChaoEggs(0), _bgPanel(0), _mmPanel(0) {

	load();
}

Area::~Area() {
	hide();

	delete _mmPanel;
	delete _bgPanel;
}

int32 Area::getID() const {
	return _id;
}

const Common::UString &Area::getName() {
	return _name;
}

uint32 Area::getWidth() const {
	return _width;
}

uint32 Area::getHeight() const {
	return _height;
}

float Area::getStartX() const {
	return _startPosX;
}

float Area::getStartY() const {
	return _startPosY;
}

void Area::show() {
	if (_mmPanel)
		_mmPanel->show();
	if (_bgPanel)
		_bgPanel->show();
}

void Area::hide() {
	if (_mmPanel)
		_mmPanel->hide();
	if (_bgPanel)
		_bgPanel->hide();
}

void Area::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Area::processEventQueue() {
	_eventQueue.clear();
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
		throw Common::Exception("No such Area ID %d (%u)", _id, areas.getRowCount());

	_name = TalkMan.getString(areas.getInt(_id, "Name", 0xFFFFFFFF));

	_background = areas.getString(_id, "Background");
	if (_background.empty())
		throw Common::Exception("Area has no background");

	_layout = areas.getString(_id, "Layout");
	if (_layout.empty())
		throw Common::Exception("Area has no layout");

	const uint32 tileSizeX = areas.getInt(_id, "TileSizeX");
	const uint32 tileSizeY = areas.getInt(_id, "TileSizeY");
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
	_bgPanel = new AreaBackground(_background);

	if ((_bgPanel->getImageWidth() != _width) || (_bgPanel->getImageHeight() != _height))
		throw Common::Exception("Background and area dimensions don't match (%ux%u vs. %ux%u)",
		                        _bgPanel->getImageWidth(), _bgPanel->getImageHeight(), _width, _height);
}

void Area::loadMiniMap() {
	if (!_miniMap.empty())
		_mmPanel = new AreaMiniMap(_miniMap);
}

void Area::loadLayout() {
	Aurora::GFF4File are(_layout, Aurora::kFileTypeARE, kAREID);
	const Aurora::GFF4Struct &areTop = are.getTopLevel();

	_tag = areTop.getString(40000);
}

} // End of namespace Sonic

} // End of namespace Engines

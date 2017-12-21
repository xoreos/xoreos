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
 *  A rooms layout for an area.
 */

#include <vector>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/strutil.h"

#include "src/aurora/resman.h"

#include "src/graphics/camera.h"
#include "src/graphics/graphics.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/jade/arealayout.h"
#include "src/engines/jade/room.h"

namespace Engines {

namespace Jade {

AreaLayout::AreaLayout() : _resRef(""), _layout(""), _visible(false) {

}

AreaLayout::AreaLayout(const Common::UString &resRef) : _resRef(resRef), _layout(resRef), _visible(false) {
	try {
		load();
	} catch (...) {
		clear();
		throw;
	}
}

AreaLayout::~AreaLayout() {
	hide();

	clear();
}

const Common::UString &AreaLayout::getResRef() {
	return _resRef;
}

void AreaLayout::show() {
	if (_visible)
		return;

	GfxMan.lockFrame();

	// Show rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->show();

	updateCamera();

	GfxMan.unlockFrame();

	_visible = true;
}

void AreaLayout::hide() {
	if (!_visible)
		return;

	GfxMan.lockFrame();

	// Hide rooms
	for (RoomList::iterator r = _rooms.begin(); r != _rooms.end(); ++r)
		(*r)->hide();

	GfxMan.unlockFrame();

	_visible = false;
}

void AreaLayout::updateCamera() {
	Room *current = currentRoom();
	if (!current)
		return;

	Common::ConfigDomain *roomProps = _art.getDomain(current->getResRef());
	if (!roomProps)
		return;

	// Set the camera to a specific position.
	if (roomProps->getInt("CamStationary", 0) == 1) {
		uint camPointCount = roomProps->getUint("CamPointCount", 0);
		if (camPointCount > 0) {
			// Choose one at random.
			int selectedCamPoint = std::rand() % camPointCount;
			Common::UString camPoint = roomProps->getString(Common::UString::format("CamPoint%u", selectedCamPoint), "");

			std::vector<Common::UString> camPos;
			Common::UString::split(camPoint, ' ', camPos);

			float posX, posY, posZ;
			parseString(camPos[0], posX);
			parseString(camPos[1], posY);
			parseString(camPos[2], posZ);

			CameraMan.setPosition(posX, posY, posZ);
			// TODO camera orientation is always set to 0.0 0.0 0.0 0.0 0.0 0.0?
			CameraMan.setOrientation(90.0f, 0.0f, -45.0f);
			CameraMan.update();
		}
	}

	// TODO Set the other camera attributes for the room.
	// CamAllowFirstPerson
	// CamDistance
	// CamFOV
	// CamGroundFollow
	// CamHeight
	// CamHeightMax
	// CamPathOffset
	// CamStartPosition
	// CamTargetDistance
	// CamTargetDistanceMin
	// CamTargetOffset
	// CamTargetType
	// CamTransitions
	// CamTranstime
}

void AreaLayout::load() {
	loadResources();

	loadLYT(); // Room layout
	loadVIS(); // Room visibilities
	loadART(); // Room properties

	loadRooms();
}

void AreaLayout::clear() {
	_rooms.clear();
}

void AreaLayout::loadResources() {
	Common::ChangeID change;

	_resources.push_back(Common::ChangeID());
	indexMandatoryArchive(_resRef + "-a.rim", 1001, &_resources.back());
}

void AreaLayout::loadLYT() {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> lyt(ResMan.getResource(_layout, Aurora::kFileTypeLYT));
		if (!lyt)
			throw Common::Exception("No such LYT");

		_lyt.load(*lyt);

	} catch (Common::Exception &e) {
		e.add("Failed loading LYT \"%s\"", _layout.c_str());
		throw;
	}
}

void AreaLayout::loadVIS() {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> vis(ResMan.getResource(_layout, Aurora::kFileTypeVIS));
		if (!vis)
			throw Common::Exception("No such VIS");

		_vis.load(*vis);

	} catch (Common::Exception &e) {
		e.add("Failed loading VIS \"%s\"", _layout.c_str());
		throw;
	}
}

void AreaLayout::loadART() {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> art(ResMan.getResource(_layout, Aurora::kFileTypeART));
		if (!art)
			throw Common::Exception("No such ART");

		_art.load(*art);

	} catch (Common::Exception &e) {
		e.add("Failed loading ART \"%s\"", _layout.c_str());
		throw;
	}
}

void AreaLayout::loadRooms() {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (size_t i = 0; i < rooms.size(); i++)
		_rooms.push_back(new Room(rooms[i].model, i, rooms[i].x, rooms[i].y, rooms[i].z, rooms[i].canWalk));
}

Room *AreaLayout::currentRoom() const {
	for (RoomList::const_iterator r = _rooms.begin(); r != _rooms.end(); ++r) {
		if ((*r)->isWalkable()) {
			const Common::ConfigDomain *roomProps = _art.getDomain((*r)->getResRef());
			if (!roomProps)
				continue;

			// Prefer rooms with a stationary camera
			// Only relevant for special areas the Player cannot normally enter
			if (roomProps->getUint("CamStationary", 0) == 1)
				return *r;
		}
	}
	// TODO find the room with the player.
	return NULL;
}
} // End of namespace Jade

} // End of namespace Engines

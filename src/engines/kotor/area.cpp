/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/area.cpp
 *  An area.
 */

#include "engines/kotor/area.h"
#include "engines/kotor/placeable.h"
#include "engines/kotor/creature.h"

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"
#include "common/maths.h"

#include "engines/engine.h"
#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "aurora/resman.h"
#include "aurora/util.h"

#include "graphics/aurora/model.h"

static const uint32 kAREID = MKID_BE('ARE ');
static const uint32 kGITID = MKID_BE('GIT ');

namespace Engines {

namespace KotOR {

Area::Room::Room(const Aurora::LYTFile::Room &lRoom) : lytRoom(&lRoom), model(0), visible(false) {
}

Area::Room::~Room() {
	delete model;
}


Area::Area() {
}

Area::~Area() {
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		delete *room;

	for (std::list<Placeable *>::iterator plc = _placeables.begin(); plc != _placeables.end(); ++plc)
		delete *plc;

	for (std::list<Creature *>::iterator crt =  _creatures.begin(); crt != _creatures.end(); ++crt)
		delete *crt;
}

void Area::load(const Common::UString &name) {
	status("Loading area \"%s\"", name.c_str());

	loadLYT(name); // Room layout
	loadVIS(name); // Room visibilities

	loadModels(name); // Room models
	loadVisibles();   // Room model visibilities

	loadARE(name); // Statics
	loadGIT(name); // Dynamics
}

void Area::loadLYT(const Common::UString &name) {
	Common::SeekableReadStream *lyt = 0;
	try {
		if (!(lyt = ResMan.getResource(name, Aurora::kFileTypeLYT)))
			throw Common::Exception("No such LYT");

		_lyt.load(*lyt);

		delete lyt;
	} catch (Common::Exception &e) {
		delete lyt;
		e.add("Failed loading LYT \"%s\"", name.c_str());
		throw e;
	} catch (...) {
		delete lyt;
		throw;
	}
}

void Area::loadVIS(const Common::UString &name) {
	Common::SeekableReadStream *vis = 0;
	try {
		if (!(vis = ResMan.getResource(name, Aurora::kFileTypeVIS)))
			throw Common::Exception("No such VIS");

		_vis.load(*vis);

		delete vis;
	} catch (Common::Exception &e) {
		delete vis;
		e.add("Failed loading VIS \"%s\"", name.c_str());
		throw e;
	} catch (...) {
		delete vis;
		throw;
	}
}

void Area::loadARE(const Common::UString &name) {
	Aurora::GFFFile are;
	loadGFF(are, name, Aurora::kFileTypeARE, kAREID);
}

void Area::loadGIT(const Common::UString &name) {
	Aurora::GFFFile git;
	loadGFF(git, name, Aurora::kFileTypeGIT, kGITID);

	const Aurora::GFFStruct &top = git.getTopLevel();

	// Load placeables
	if (top.hasField("Placeable List")) {
		const Aurora::GFFList &list = top.getList("Placeable List");

		for (Aurora::GFFList::const_iterator p = list.begin(); p != list.end(); ++p)
			loadPlaceable(**p);
	}

	// Load creatures
	if (top.hasField("Creature List")) {
		const Aurora::GFFList &list = top.getList("Creature List");

		for (Aurora::GFFList::const_iterator c = list.begin(); c != list.end(); ++c)
			loadCreature(**c);
	}
}

void Area::loadPlaceable(const Aurora::GFFStruct &placeable) {
	Common::UString resref = placeable.getString("TemplateResRef");
	if (resref.empty())
		throw Common::Exception("Placeable without a template");

	float x = placeable.getDouble("X");
	float y = placeable.getDouble("Y");
	float z = placeable.getDouble("Z");

	float bearing = placeable.getDouble("Bearing");

	Placeable *place = 0;
	try {
		place = new Placeable;

		place->load(resref);
		place->setPosition(x, y, z);
		place->setOrientation(0.0, Common::rad2deg(bearing), 0.0);

	} catch (Common::Exception &e) {
		delete place;
		e.add("Failed loading placeable \"%s\"", resref.c_str());
		throw e;
	} catch (...) {
		delete place;
		throw;
	}

	_placeables.push_back(place);
}

void Area::loadCreature(const Aurora::GFFStruct &creature) {
	Common::UString resref = creature.getString("TemplateResRef");
	if (resref.empty())
		throw Common::Exception("Creature without a template");

	float x = creature.getDouble("XPosition");
	float y = creature.getDouble("YPosition");
	float z = creature.getDouble("ZPosition");

	float bearingX = creature.getDouble("XOrientation");
	float bearingY = creature.getDouble("YOrientation");

	Creature *creat = 0;
	try {
		creat = new Creature;

		creat->load(resref);
		creat->setPosition(x, y, z);

		float orientation[3];
		Common::vector2orientation(bearingX, bearingY,
		                           orientation[0], orientation[1], orientation[2]);

		creat->setOrientation(orientation[0], orientation[1], orientation[2]);

	} catch (Common::Exception &e) {
		delete creat;
		e.add("Failed loading creature \"%s\"", resref.c_str());
		throw e;
	} catch (...) {
		delete creat;
		throw;
	}

	_creatures.push_back(creat);
}

void Area::loadModels(const Common::UString &name) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	_rooms.reserve(rooms.size());
	for (size_t i = 0; i < rooms.size(); i++) {
		const Aurora::LYTFile::Room &lytRoom = rooms[i];

		if (lytRoom.model == "****")
			// No model for that room
			continue;

		Room *room = new Room(lytRoom);

		try {
			room->model = loadModelObject(lytRoom.model);
		} catch (Common::Exception &e) {
			delete room;
			e.add("Can't load model \"%s\" for area \"%s\"", lytRoom.model.c_str(), name.c_str());
			throw e;
		} catch (...) {
			delete room;
			throw;
		}

		room->model->setPosition(lytRoom.x, lytRoom.y, lytRoom.z);

		_rooms.push_back(room);
	}

}

void Area::loadVisibles() {
	// Go through all rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room) {
		// Get visibility information for that room
		const std::vector<Common::UString> &rooms = _vis.getVisibilityArray((*room)->lytRoom->model);

		if (rooms.empty()) {
			// If no info is available, assume all rooms are visible

			for (std::vector<Room *>::iterator iRoom = _rooms.begin(); iRoom != _rooms.end(); ++iRoom)
				(*room)->visibles.push_back(*iRoom);

			return;
		}

		// Otherwise, go through all rooms again, look for a match with the visibilities
		for (std::vector<Room *>::iterator iRoom = _rooms.begin(); iRoom != _rooms.end(); ++iRoom) {

			for (std::vector<Common::UString>::const_iterator vRoom = rooms.begin(); vRoom != rooms.end(); ++vRoom) {
				if (vRoom->equalsIgnoreCase((*iRoom)->lytRoom->model)) {
					// Mark that room as visible from the first room
					(*room)->visibles.push_back(*iRoom);
					break;
				}
			}

		}

	}

}

void Area::show() {
	// Rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->model->show();

	// Placeables
	for (std::list<Placeable *>::iterator it = _placeables.begin(); it != _placeables.end(); ++it)
		(*it)->show();

	// Creatures
	for (std::list<Creature *>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
		(*it)->show();
}

void Area::hide() {
	// Rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->model->hide();

	// Placeables
	for (std::list<Placeable *>::iterator it = _placeables.begin(); it != _placeables.end(); ++it)
		(*it)->hide();

	// Creatures
	for (std::list<Creature *>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
		(*it)->hide();
}

/*
void Area::setPosition(float x, float y, float z) {
	// Set room positions
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->model->setPosition((*room)->lytRoom->x + x, (*room)->lytRoom->y + y, (*room)->lytRoom->z + z);

	// Switch all rooms temporarily to invisible
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->visible = false;

	// Look in what rooms we're in
	std::vector<Room *> ins;
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room) {
		// Roughly head position.
		if ((*room)->model->isIn(0.0, 1.5, 0.0)) {
			// The rooms we're in should be visible
			(*room)->visible = true;

			ins.push_back(*room);
		}
	}

	if (ins.empty()) {
		// When we're outside everything, still display everything

		for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
			(*room)->visible = true;

	} else {
		// Display stuff that's visible from the room we're in

		for (std::vector<Room *>::iterator in = ins.begin(); in != ins.end(); ++in)
			for (std::vector<Room *>::iterator vis = (*in)->visibles.begin(); vis != (*in)->visibles.end(); ++vis)
				(*vis)->visible = true;
	}

	// Show/Hide according to the visibility
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room) {
		if ((*room)->visible)
			(*room)->model->show();
		else
			(*room)->model->hide();
	}

	// Set placeable positions
	for (std::list<Placeable *>::iterator it = _placeables.begin(); it != _placeables.end(); ++it)
		(*it)->moveWorld(x, y, z);

	// Set creature positions
	for (std::list<Creature *>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
		(*it)->moveWorld(x, y, z);
}

void Area::setOrientation(float x, float y, float z) {
	// Rooms
	for (std::vector<Room *>::iterator room = _rooms.begin(); room != _rooms.end(); ++room)
		(*room)->model->setOrientation(x, y, z);

	// Placables
	for (std::list<Placeable *>::iterator it = _placeables.begin(); it != _placeables.end(); ++it)
		(*it)->turnWorld(x, y, z);

	// Creatures
	for (std::list<Creature *>::iterator it = _creatures.begin(); it != _creatures.end(); ++it)
		(*it)->turnWorld(x, y, z);
}
*/

} // End of namespace KotOR

} // End of namespace Engines

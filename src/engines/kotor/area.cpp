/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "common/error.h"
#include "common/ustring.h"
#include "common/stream.h"

#include "aurora/resman.h"

#include "graphics/aurora/model_kotor.h"

namespace Engines {

namespace KotOR {

Area::Area() {
}

Area::~Area() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		delete *model;
}

void Area::load(const Common::UString &name) {
	loadLYT(name); // Room layout
	loadVIS(name); // Room visibilities

	loadARE(name); // Statics
	loadGIT(name); // Dynamics

	loadModels(name);
}

void Area::loadLYT(const Common::UString &name) {
	Common::SeekableReadStream *lyt = 0;
	try {
		if (!(lyt = ResMan.getResource(name, Aurora::kFileTypeLYT)))
			throw Common::Exception("No such LYT \"%s\"", name.c_str());

		_lyt.load(*lyt);

		delete lyt;
	} catch (...) {
		delete lyt;
		throw;
	}
}

void Area::loadVIS(const Common::UString &name) {
	Common::SeekableReadStream *vis = 0;
	try {
		if (!(vis = ResMan.getResource(name, Aurora::kFileTypeVIS)))
			throw Common::Exception("No such VIS \"%s\"", name.c_str());

		_vis.load(*vis);

		delete vis;
	} catch (...) {
		delete vis;
		throw;
	}
}

void Area::loadARE(const Common::UString &name) {
	Common::SeekableReadStream *are = 0;
	try {
		if (!(are = ResMan.getResource(name, Aurora::kFileTypeARE)))
			throw Common::Exception("No such ARE \"%s\"", name.c_str());

		delete are;
	} catch (...) {
		delete are;
		throw;
	}
}

void Area::loadGIT(const Common::UString &name) {
	Common::SeekableReadStream *git = 0;
	try {
		if (!(git = ResMan.getResource(name, Aurora::kFileTypeGIT)))
			throw Common::Exception("No such GIT \"%s\"", name.c_str());

		delete git;
	} catch (...) {
		delete git;
		throw;
	}
}

void Area::loadModels(const Common::UString &name) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	_models.resize(rooms.size());
	for (size_t i = 0; i < rooms.size(); i++)
		_models[i] = 0;

	for (size_t i = 0; i < rooms.size(); i++) {
		const Aurora::LYTFile::Room &room = rooms[i];

		if (room.model == "****")
			// No model for that room
			continue;

		try {
			_models[i] = loadModel(room.model);
		} catch (Common::Exception &e) {
			e.add("Can't load model \"%s\" for area \"%s\"", room.model.c_str(), name.c_str());
			throw e;
		} catch (...) {
			throw;
		}

		_models[i]->setPosition(room.x, room.y, room.z);
	}
}

void Area::show() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		if (*model)
			(*model)->show();
}

void Area::hide() {
	for (std::vector<Graphics::Aurora::Model *>::iterator model = _models.begin(); model != _models.end(); ++model)
		if (*model)
			(*model)->hide();
}

void Area::setPosition(float x, float y, float z) {
	const Aurora::LYTFile::RoomArray &rooms = _lyt.getRooms();
	for (size_t i = 0; i < rooms.size(); i++)
		if (_models[i])
			_models[i]->setPosition(rooms[i].x + x, rooms[i].y + y, rooms[i].z + z);
}

Graphics::Aurora::Model *Area::loadModel(const Common::UString &resref) {
	Common::SeekableReadStream *mdl = 0, *mdx = 0;
	Graphics::Aurora::Model *model = 0;

	try {
		if (!(mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL)))
			throw Common::Exception("No such MDL");
		if (!(mdx = ResMan.getResource(resref, Aurora::kFileTypeMDX)))
			throw Common::Exception("No such MDX");

		model = new Graphics::Aurora::Model_KotOR(*mdl, *mdx, false);

	} catch (...) {
		delete mdl;
		delete mdx;
		delete model;
		throw;
	}

	return model;
}

} // End of namespace KotOR

} // End of namespace Engines

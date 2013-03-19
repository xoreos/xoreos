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

/** @file engines/nwn/tileset.cpp
 *  NWN tileset.
 */

#include "common/error.h"
#include "common/configfile.h"
#include "common/stream.h"

#include "aurora/types.h"
#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "engines/nwn/tileset.h"

namespace Engines {

namespace NWN {

Tileset::Tileset(const Common::UString &resRef) {
	Common::SeekableReadStream *setFile = ResMan.getResource(resRef, Aurora::kFileTypeSET);
	if (!setFile)
		throw Common::Exception("No such tileset \"%s\"", resRef.c_str());

	Common::ConfigFile set;

	try {
		set.load(*setFile);
	} catch (...) {
		delete setFile;
		throw;
	}

	delete setFile;

	load(set);
}

Tileset::~Tileset() {
}

const Common::UString &Tileset::getName() const {
	return _name;
}

float Tileset::getTilesHeight() const {
	return _tilesHeight;
}

const Tileset::Tile &Tileset::getTile(uint n) const {
	if (n >= _tiles.size())
		throw Common::Exception("Tileset has no tile %u", n);

	return _tiles[n];
}

void Tileset::load(const Common::ConfigFile &set) {
	const Common::ConfigDomain *general = set.getDomain("GENERAL");
	if (!general)
		throw Common::Exception("Tileset has no \"GENERAL\" domain");

	loadGeneral(*general);

	const Common::ConfigDomain *tiles = set.getDomain("TILES");
	if (!tiles)
		throw Common::Exception("Tileset has no \"TILES\" domain");

	_tiles.resize(tiles->getUint("Count"));

	for (uint i = 0; i < _tiles.size(); i++)
		loadTile(set, i, _tiles[i]);
}

void Tileset::loadGeneral(const Common::ConfigDomain &general) {
	_name = TalkMan.getString(general.getUint("DisplayName", Aurora::kStrRefInvalid));

	_tilesHeight = general.getUint("Transition");
}

void Tileset::loadTile(const Common::ConfigFile &set, uint i, Tile &tile) {
	Common::UString domainName = Common::UString::sprintf("TILE%u", i);
	const Common::ConfigDomain *domain = set.getDomain(domainName);
	if (!domain)
		throw Common::Exception("Tileset has no \"%s\" domain", domainName.c_str());

	tile.model = domain->getString("Model");
}

} // End of namespace NWN

} // End of namespace Engines

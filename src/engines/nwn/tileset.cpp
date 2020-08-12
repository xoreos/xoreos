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
 *  A tileset in Neverwinter Nights.
 */

#include <memory>

#include "src/common/error.h"
#include "src/common/configfile.h"
#include "src/common/readstream.h"

#include "src/aurora/types.h"
#include "src/aurora/resman.h"
#include "src/aurora/talkman.h"

#include "src/engines/nwn/tileset.h"

namespace Engines {

namespace NWN {

Tileset::Tileset(const Common::UString &resRef) {
	std::unique_ptr<Common::SeekableReadStream> setFile(ResMan.getResource(resRef, Aurora::kFileTypeSET));
	if (!setFile)
		throw Common::Exception("No such tileset \"%s\"", resRef.c_str());

	Common::ConfigFile set;

	set.load(*setFile);

	load(set);
}

Tileset::~Tileset() {
}

const Common::UString &Tileset::getName() const {
	return _name;
}

const Common::UString &Tileset::getEnvironmentMap() const {
	return _environmentMap;
}

float Tileset::getTilesHeight() const {
	return _tilesHeight;
}

const Tileset::Tile &Tileset::getTile(size_t n) const {
	if (n >= _tiles.size())
		throw Common::Exception("Tileset has no tile %u", (uint)n);

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

	for (size_t i = 0; i < _tiles.size(); i++)
		loadTile(set, i, _tiles[i]);
}

void Tileset::loadGeneral(const Common::ConfigDomain &general) {
	_name = TalkMan.getString(general.getUint("DisplayName", Aurora::kStrRefInvalid));

	_tilesHeight    = general.getUint("Transition");
	_environmentMap = general.getString("EnvMap");
}

void Tileset::loadTile(const Common::ConfigFile &set, uint i, Tile &tile) {
	Common::UString domainName = Common::String::format("TILE%u", i);
	const Common::ConfigDomain *domain = set.getDomain(domainName);
	if (!domain)
		throw Common::Exception("Tileset has no \"%s\" domain", domainName.c_str());

	tile.model = domain->getString("Model");
}

} // End of namespace NWN

} // End of namespace Engines

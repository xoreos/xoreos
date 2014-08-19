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
 */

/** @file engines/nwn/tileset.h
 *  NWN tileset.
 */

#ifndef ENGINES_NWN_TILESET_H
#define ENGINES_NWN_TILESET_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

namespace Common {
	class ConfigFile;
	class ConfigDomain;
}

namespace Engines {

namespace NWN {

class Tileset {
public:
	struct Tile {
		Common::UString model;
	};

	Tileset(const Common::UString &resRef);
	~Tileset();

	const Common::UString &getName() const;

	float getTilesHeight() const;

	const Tile &getTile(uint n) const;

private:
	Common::UString _resRef;
	Common::UString _name;

	float _tilesHeight;

	std::vector<Tile> _tiles;

	void load(const Common::ConfigFile &set);

	void loadGeneral(const Common::ConfigDomain &general);
	void loadTile(const Common::ConfigFile &set, uint i, Tile &tile);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_TILESET_H

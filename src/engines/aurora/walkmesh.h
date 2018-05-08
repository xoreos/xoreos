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
 *  Generic walkmesh.
 */

#ifndef ENGINES_AURORA_WALKMESH_H
#define ENGINES_AURORA_WALKMESH_H

#include <vector>

#include "src/common/types.h"

namespace Engines {

struct WalkmeshData {
	std::vector<float> vertices;
	std::vector<uint32> indices;
	std::vector<bool> faceWalkableMap;
};

class Walkmesh {
public:
	void clear();
	void refreshIndexGroups();
	const WalkmeshData &getData() const;
	const std::vector<uint32> &getIndicesWalkable() const;
protected:
	WalkmeshData _data;
	std::vector<uint32> _indicesWalkable;
	std::vector<uint32> _indicesNonWalkable;
};

} // End of namespace Engines

#endif // ENGINES_AURORA_WALKMESH_H

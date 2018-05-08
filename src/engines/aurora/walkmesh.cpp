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

#include "src/engines/aurora/walkmesh.h"

namespace Engines {

void Walkmesh::clear() {
	_data.vertices.clear();
	_data.indices.clear();
	_data.faceWalkableMap.clear();
	_indicesWalkable.clear();
	_indicesNonWalkable.clear();
}

void Walkmesh::refreshIndexGroups() {
	_indicesWalkable.clear();
	_indicesNonWalkable.clear();

	size_t faceCount = _data.indices.size() / 3;
	size_t index = 0;

	for (size_t i = 0; i < faceCount; ++i) {
		float i0 = _data.indices[index + 0];
		float i1 = _data.indices[index + 1];
		float i2 = _data.indices[index + 2];

		if (_data.faceWalkableMap[i]) {
			_indicesWalkable.push_back(i0);
			_indicesWalkable.push_back(i1);
			_indicesWalkable.push_back(i2);
		} else {
			_indicesNonWalkable.push_back(i0);
			_indicesNonWalkable.push_back(i1);
			_indicesNonWalkable.push_back(i2);
		}

		index += 3;
	}
}

const WalkmeshData &Walkmesh::getData() const {
	return _data;
}

const std::vector<uint32> &Walkmesh::getIndicesWalkable() const {
	return _indicesWalkable;
}

} // End of namespace Engines

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
 *  WalkmeshObject implementation for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_PATH_OBJECTWALKMESH_H
#define ENGINES_KOTORBASE_PATH_OBJECTWALKMESH_H

#include "external/glm/vec2.hpp"

#include "src/aurora/types.h"

#include "src/engines/aurora/objectwalkmesh.h"

namespace Engines {

namespace KotORBase {

class Situated;

class ObjectWalkmesh : public Engines::ObjectWalkmesh {
public:
	ObjectWalkmesh(Situated *situated, Aurora::FileType fileType = Aurora::kFileTypePWK);

	const std::vector<float> &getVertices() const;
	const std::vector<uint32_t> &getFaces() const;

	bool in(const glm::vec2 &minBox, const glm::vec2 &maxBox) const;
	bool in(const glm::vec2 &point) const;
	bool in(const glm::vec3 &start, const glm::vec3 &end) const;

	bool findIntersection(const glm::vec3 &start, const glm::vec3 &end,
	                      glm::vec3 &intersect) const;

	void load(const Common::UString &resRef,
	          float orientation[4],
	          float position[3]);

protected:
	Aurora::FileType _fileType;

private:
	void computeMinMax();

	glm::vec3 _min;
	glm::vec3 _max;

	std::vector<float>  _vertices;
	std::vector<uint32_t> _faces;
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PATH_OBJECTWALKMESH_H

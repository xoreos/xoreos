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
 *  KotOR walkmesh (BWM).
 */

#ifndef ENGINES_KOTOR_WALKMESH_H
#define ENGINES_KOTOR_WALKMESH_H

#include "glm/mat4x4.hpp"

#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/walkmesh.h"

namespace Engines {

namespace KotOR {

class Walkmesh : public Graphics::Aurora::Walkmesh {
public:
	void load(const Common::UString &resRef,
	          ::Aurora::FileType type = ::Aurora::kFileTypeWOK,
	          const glm::mat4 &transform = glm::mat4());
private:
	void appendFromStream(Common::SeekableReadStream &stream, const glm::mat4 &transform);

	void appendFaceTypes(Common::SeekableReadStream &stream,
	                     uint32 faceCount,
	                     uint32 faceTypeOffset);

	void appendIndices(Common::SeekableReadStream &stream,
	                   uint32 faceCount,
	                   uint32 faceOffset);

	void appendVertices(Common::SeekableReadStream &stream,
	                    uint32 vertexCount,
	                    uint32 vertexOffset,
	                    const glm::mat4 &transform);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_WALKMESH_H

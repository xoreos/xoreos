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
 *  KotOR walkmesh (.wok) file loader. Ported from Supermanu's pathfinding
 *  branch.
 */

#ifndef ENGINES_KOTOR_WALKMESH_H
#define ENGINES_KOTOR_WALKMESH_H

#include <vector>

#include "src/common/readstream.h"
#include "src/common/types.h"

#include "src/engines/aurora/walkmesh.h"

#include "src/graphics/renderable.h"

namespace Engines {

namespace KotOR {

class Walkmesh : public Engines::Walkmesh, public Graphics::Renderable {
public:
	Walkmesh();
	void load(const Common::UString &resRef);
	float getElevationAt(float x, float y, uint32 &faceIndex) const;

	/** Highlight face with specified index.
	 *
	 *  @param index Index of the face to highlight or -1 to disable
	 *               highlighting
	 */
	void highlightFace(uint32 index);

	void setInvisible(bool invisible);

	// .--- Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);
	// '---
private:
	int _highlightFaceIndex;
	bool _invisible;

	void appendFromStream(Common::SeekableReadStream &stream);
	void appendFaceTypes(Common::SeekableReadStream &stream, uint32 faceCount, uint32 faceTypeOffset);
	void appendIndices(Common::SeekableReadStream &stream, uint32 faceCount, uint32 faceOffset);
	void appendVertices(Common::SeekableReadStream &stream, uint32 vertexCount, uint32 vertexOffset);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_WALKMESH_H

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

/** @file graphics/meshutil.h
 *  A scene manager.
 */

#ifndef GRAPHICS_MESHUTIL_H
#define GRAPHICS_MESHUTIL_H

#include <vector>

#include <boost/functional/hash.hpp>

#include "common/types.h"
#include "common/util.h"
#include "common/maths.h"

namespace Ogre {
	class SubMesh;
	class SceneManager;
	class MeshManager;
}

namespace Graphics {

Ogre::MeshManager &getOgreMeshManager();

struct VertexDeclaration {
	bool textureUVW;

	uint16 faces;
	uint16 vertices;
	uint16 textures;

	std::vector<float>  bufferVerticesNormals;
	std::vector<float>  bufferTexCoords;
	std::vector<uint16> bufferIndices;

	VertexDeclaration();
	VertexDeclaration(uint16 f, uint16 v, uint16 t, bool uvw = false);

	void resize();
	void getBounds(float &minX, float &minY, float &minZ,
	               float &maxX, float &maxY, float &maxZ, float &radius) const;
};

void createMesh(Ogre::SubMesh *mesh, const VertexDeclaration &decl);

void createMesh(Ogre::SubMesh *mesh, uint16 vertexCount, uint16 faceCount,
                const float *vertices, const float *normals, const uint16 *indices,
                const float *texCoords1 = 0, const float *texCoords2 = 0, const float *texCoords3 = 0);

} // End of namespace Graphics

#endif // GRAPHICS_MESHUTIL_H

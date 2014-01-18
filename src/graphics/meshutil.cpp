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

/** @file graphics/meshutil.cpp
 *  A scene manager.
 */

#include <cstring>

#include <OgreRoot.h>
#include <OgreMeshManager.h>
#include <OgreSubMesh.h>
#include <OgreHardwareBufferManager.h>

#include "common/error.h"
#include "common/threads.h"

#include "graphics/meshutil.h"

#include "events/requests.h"

namespace Graphics {

VertexDeclaration::VertexDeclaration() : textureUVW(false), faces(0), vertices(0), textures(0) {
}

VertexDeclaration::VertexDeclaration(uint16 f, uint16 v, uint16 t, bool uvw) :
	textureUVW(uvw), faces(f), vertices(v), textures(t) {

	resize();
}

void VertexDeclaration::resize() {
	bufferVerticesNormals.resize(vertices * 2 * 3);
	bufferTexCoords.resize(vertices * textures * (textureUVW ? 3 : 2));
	bufferIndices.resize(faces * 3);
}

void VertexDeclaration::getBounds(float &minX, float &minY, float &minZ,
                                  float &maxX, float &maxY, float &maxZ, float &radius) const {

	if (bufferVerticesNormals.size() < 6) {
		minX = minY = minZ = maxX = maxY = maxZ = radius = 0.0f;
		return;
	}

	minX = FLT_MAX; minY = FLT_MAX; minZ = FLT_MAX; maxX = -FLT_MAX; maxY = -FLT_MAX; maxZ = -FLT_MAX;
	for (uint i = 0; i < bufferVerticesNormals.size(); i += 6) {
		minX = MIN(minX, bufferVerticesNormals[i + 0]);
		minY = MIN(minY, bufferVerticesNormals[i + 1]);
		minZ = MIN(minZ, bufferVerticesNormals[i + 2]);
		maxX = MAX(maxX, bufferVerticesNormals[i + 0]);
		maxY = MAX(maxY, bufferVerticesNormals[i + 1]);
		maxZ = MAX(maxZ, bufferVerticesNormals[i + 2]);
	}

	radius = MAX(maxX - minY, MAX(maxY - minY, maxZ - minZ)) / 2.0f;
}


void createMesh(Ogre::SubMesh *mesh, const VertexDeclaration &decl) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&createMesh, mesh, decl));

		return RequestMan.callInMainThread(functor);
	}

	assert (decl.bufferVerticesNormals.size() == (decl.vertices * 2 * 3));
	assert (decl.bufferTexCoords.size() == (uint)(decl.vertices * decl.textures * (decl.textureUVW ? 3 : 2)));
	assert (decl.bufferIndices.size() == (decl.faces * 3));

	Ogre::VertexData *vertexData = new Ogre::VertexData();

	mesh->useSharedVertices = false;
	mesh->vertexData        = vertexData;

	vertexData->vertexCount = decl.vertices;

	// Set up the vertex declaration

	Ogre::VertexDeclaration *vertexDecl = vertexData->vertexDeclaration;

	vertexDecl->addElement(0, 0 * 3 * sizeof(float), Ogre::VET_FLOAT3, Ogre::VES_POSITION);
	vertexDecl->addElement(0, 1 * 3 * sizeof(float), Ogre::VET_FLOAT3, Ogre::VES_NORMAL);

	for (uint i = 0; i < decl.textures; i++)
		vertexDecl->addElement(1, i * (decl.textureUVW ? 3 : 2) * sizeof(float),
				decl.textureUVW ? Ogre::VET_FLOAT3 : Ogre::VET_FLOAT2, Ogre::VES_TEXTURE_COORDINATES, i);

	// Prepare hardware buffers

	Ogre::VertexBufferBinding *bind = vertexData->vertexBufferBinding;

	Ogre::HardwareBufferManager &bufferMan = Ogre::HardwareBufferManager::getSingleton();
	Ogre::HardwareVertexBufferSharedPtr vBuf((Ogre::HardwareVertexBuffer *) 0);
	Ogre::HardwareIndexBufferSharedPtr  iBuf((Ogre::HardwareIndexBuffer  *) 0);

	// Vertex/Normals buffer

	vBuf = bufferMan.createVertexBuffer(vertexDecl->getVertexSize(0), decl.vertices,
	                                    Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	assert(vBuf->getSizeInBytes() == (decl.bufferVerticesNormals.size() * sizeof(float)));

	vBuf->writeData(0, vBuf->getSizeInBytes(), &decl.bufferVerticesNormals[0], true);

	bind->setBinding(0, vBuf);

	// Texture coordinates buffer

	if (decl.textures > 0) {
		vBuf = bufferMan.createVertexBuffer(vertexDecl->getVertexSize(1), decl.vertices,
		                                    Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		assert(vBuf->getSizeInBytes() == (decl.bufferTexCoords.size() * sizeof(float)));

		vBuf->writeData(0, vBuf->getSizeInBytes(), &decl.bufferTexCoords[0], true);

		bind->setBinding(1, vBuf);
	}

	// Index buffer

	iBuf = bufferMan.createIndexBuffer(Ogre::HardwareIndexBuffer::IT_16BIT, decl.faces * 3,
	                                   Ogre::HardwareBuffer::HBU_STATIC_WRITE_ONLY);

	assert(iBuf->getSizeInBytes() == (decl.bufferIndices.size() * sizeof(uint16)));

	mesh->indexData->indexBuffer = iBuf;
	mesh->indexData->indexCount  = decl.faces * 3;
	mesh->indexData->indexStart  = 0;

	iBuf->writeData(0, iBuf->getSizeInBytes(), &decl.bufferIndices[0], true);
}

void createMesh(Ogre::SubMesh *mesh, uint16 vertexCount, uint16 faceCount,
                const float *vertices, const float *normals, const uint16 *indices,
                const float *texCoords1, const float *texCoords2, const float *texCoords3) {

	uint16 textureCount = 0;
	if      (texCoords3)
		textureCount = 3;
	else if (texCoords2)
		textureCount = 2;
	else if (texCoords1)
		textureCount = 1;

	VertexDeclaration vertexDecl(faceCount, vertexCount, textureCount);

	for (uint i = 0, j = 0; i < vertexDecl.bufferVerticesNormals.size(); i += 6, j += 3) {
		float *d = &vertexDecl.bufferVerticesNormals[i];

		const float *v = &vertices[j];
		const float *n = &normals[j];

		*d++ = *v++;
		*d++ = *v++;
		*d++ = *v++;
		*d++ = *n++;
		*d++ = *n++;
		*d++ = *n++;
	}

	for (uint i = 0, j = 0; i < vertexDecl.bufferTexCoords.size(); i += (textureCount * 2), j += 2) {
		float *d = &vertexDecl.bufferTexCoords[i];

		if (textureCount >= 1) {
			const float *t = texCoords1 ? &texCoords1[j] : 0;

			*d++ = t ? *t++ : 0.0;
			*d++ = t ? *t++ : 0.0;
		}

		if (textureCount >= 2) {
			const float *t = texCoords2 ? &texCoords2[j] : 0;

			*d++ = t ? *t++ : 0.0;
			*d++ = t ? *t++ : 0.0;
		}

		if (textureCount >= 3) {
			const float *t = texCoords3 ? &texCoords3[j] : 0;

			*d++ = t ? *t++ : 0.0;
			*d++ = t ? *t++ : 0.0;
		}
	}

	memcpy(&vertexDecl.bufferIndices[0], indices, vertexDecl.bufferIndices.size() * sizeof(float));

	createMesh(mesh, vertexDecl);
}

Ogre::MeshManager &getOgreMeshManager() {
	return Ogre::MeshManager::getSingleton();
}

} // End of namespace Graphics

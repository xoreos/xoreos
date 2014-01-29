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

/** @file graphics/aurora/quad.cpp
 *  A simple quad.
 */

#include <OgreEntity.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreMeshManager.h>
#include <OgreTextureUnitState.h>
#include <OgrePass.h>
#include <OgreTechnique.h>

#include "common/threads.h"
#include "common/uuid.h"
#include "common/error.h"

#include "graphics/util.h"
#include "graphics/textureman.h"
#include "graphics/materialman.h"

#include "graphics/aurora/quad.h"
#include "graphics/aurora/types.h"

#include "events/requests.h"

namespace Graphics {

namespace Aurora {

Ogre::Entity *createQuadEntity(float width, float height, Ogre::MaterialPtr material,
                               float topLeftU, float topLeftV, float bottomRightU, float bottomRightV,
                               const Common::UString &scene) {

	VertexDeclaration vertexDecl(2, 4, 1);

	vertexDecl.bufferVerticesNormals[ 0] =  0.0;
	vertexDecl.bufferVerticesNormals[ 1] =  0.0;
	vertexDecl.bufferVerticesNormals[ 2] =  0.0;

	vertexDecl.bufferVerticesNormals[ 3] =  0.0;
	vertexDecl.bufferVerticesNormals[ 4] =  0.0;
	vertexDecl.bufferVerticesNormals[ 5] =  1.0;

	vertexDecl.bufferVerticesNormals[ 6] =  width;
	vertexDecl.bufferVerticesNormals[ 7] =  0.0;
	vertexDecl.bufferVerticesNormals[ 8] =  0.0;

	vertexDecl.bufferVerticesNormals[ 9] =  0.0;
	vertexDecl.bufferVerticesNormals[10] =  0.0;
	vertexDecl.bufferVerticesNormals[11] =  1.0;

	vertexDecl.bufferVerticesNormals[12] =  width;
	vertexDecl.bufferVerticesNormals[13] = -height;
	vertexDecl.bufferVerticesNormals[14] =  0.0;

	vertexDecl.bufferVerticesNormals[15] =  0.0;
	vertexDecl.bufferVerticesNormals[16] =  0.0;
	vertexDecl.bufferVerticesNormals[17] =  1.0;

	vertexDecl.bufferVerticesNormals[18] =  0.0;
	vertexDecl.bufferVerticesNormals[19] = -height;
	vertexDecl.bufferVerticesNormals[20] =  0.0;

	vertexDecl.bufferVerticesNormals[21] =  0.0;
	vertexDecl.bufferVerticesNormals[22] =  0.0;
	vertexDecl.bufferVerticesNormals[23] =  1.0;

	vertexDecl.bufferTexCoords[0] = topLeftU;
	vertexDecl.bufferTexCoords[1] = 1.0 - topLeftV;

	vertexDecl.bufferTexCoords[2] = bottomRightU;
	vertexDecl.bufferTexCoords[3] = 1.0 - topLeftV;

	vertexDecl.bufferTexCoords[4] = bottomRightU;
	vertexDecl.bufferTexCoords[5] = 1.0 - bottomRightV;

	vertexDecl.bufferTexCoords[6] = topLeftU;
	vertexDecl.bufferTexCoords[7] = 1.0 - bottomRightV;

	vertexDecl.bufferIndices[0] = 0;
	vertexDecl.bufferIndices[1] = 2;
	vertexDecl.bufferIndices[2] = 1;
	vertexDecl.bufferIndices[3] = 2;
	vertexDecl.bufferIndices[4] = 0;
	vertexDecl.bufferIndices[5] = 3;

	Common::UString name = Common::generateIDNumberString();

	Ogre::MeshPtr mesh = getOgreMeshManager().createManual(name.c_str(), "General");
	Ogre::SubMesh *subMesh = mesh->createSubMesh();

	createMesh(subMesh, vertexDecl);

	// Bounding box / sphere

	float minX, minY, minZ, maxX, maxY, maxZ, meshRadius;
	vertexDecl.getBounds(minX, minY, minZ, maxX, maxY, maxZ, meshRadius);

	mesh->_setBounds(Ogre::AxisAlignedBox(minX, minY, minZ, maxX, maxY, maxZ));
	mesh->_setBoundingSphereRadius(meshRadius);

	mesh->load();

		// Create entity and put the material on it

	Ogre::Entity *entity = getOgreSceneManager(scene).createEntity(mesh);
	entity->setMaterial(material);

	return entity;
}


Quad::Quad(const Common::UString &scene) : Renderable(scene),
	_r(1.0), _g(1.0), _b(1.0), _a(1.0),
	_topLeftU(0.0), _topLeftV(0.0), _bottomRightU(1.0), _bottomRightV(1.0),
	_entity(0), _needUpdate(true), _newTexture(true) {

	createRootNode();
}

Quad::Quad(const Common::UString &texture, const Common::UString &scene) : Renderable(scene),
	_textureName(texture), _r(1.0), _g(1.0), _b(1.0), _a(1.0),
	_topLeftU(0.0), _topLeftV(0.0), _bottomRightU(1.0), _bottomRightV(1.0),
	_entity(0), _needUpdate(true), _newTexture(true) {

	createRootNode();
}

Quad::Quad(const Ogre::TexturePtr &texture, const Common::UString &scene) : Renderable(scene),
	_texture(texture), _r(1.0), _g(1.0), _b(1.0), _a(1.0),
	_topLeftU(0.0), _topLeftV(0.0), _bottomRightU(1.0), _bottomRightV(1.0),
	_entity(0), _needUpdate(true), _newTexture(false) {

	createRootNode();
}

Quad::~Quad() {
	destroy();
}

void Quad::createRootNode() {
	Common::UString name = Common::generateIDNumberString();

	_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(name.c_str());
}

void Quad::setTexture() {
	if (!Common::isMainThread()) {
		void (Quad::*f)() = &Quad::setTexture;

		Events::MainThreadFunctor<void> functor(boost::bind(f, this));

		return RequestMan.callInMainThread(functor);
	}

	_textureName.clear();

	_newTexture = true;
	_needUpdate = true;

	if (_visible)
		update();
}

void Quad::setTexture(const Common::UString &texture) {
	if (!Common::isMainThread()) {
		void (Quad::*f)(const Common::UString &) = &Quad::setTexture;

		Events::MainThreadFunctor<void> functor(boost::bind(f, this, texture));

		return RequestMan.callInMainThread(functor);
	}

	_textureName = texture;

	_newTexture = true;
	_needUpdate = true;

	if (_visible)
		update();
}

void Quad::setTexture(const Ogre::TexturePtr &texture) {
	if (!Common::isMainThread()) {
		void (Quad::*f)(const Ogre::TexturePtr &) = &Quad::setTexture;

		Events::MainThreadFunctor<void> functor(boost::bind(f, this, texture));

		return RequestMan.callInMainThread(functor);
	}

	_texture = texture;

	_needUpdate = true;

	if (_visible)
		update();
}

void Quad::setColor(float r, float g, float b, float a) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Quad::setColor, this, r, g, b, a));

		return RequestMan.callInMainThread(functor);
	}

	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_needUpdate = true;

	if (_visible)
		update();
}

void Quad::setUV(float topLeftU, float topLeftV, float bottomRightU, float bottomRightV) {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Quad::setUV, this, topLeftU, topLeftV, bottomRightU, bottomRightV));

		return RequestMan.callInMainThread(functor);
	}

	_topLeftU     = topLeftU;
	_topLeftV     = topLeftV;
	_bottomRightU = bottomRightU;
	_bottomRightV = bottomRightV;

	_needUpdate = true;

	if (_visible)
		update();
}

void Quad::setSelectable(bool selectable) {
	if (_entity)
		_entity->setQueryFlags(selectable ? kSelectableQuad : kSelectableNone);

	Renderable::setSelectable(selectable);
}

void Quad::setVisible(bool visible) {
	if (_visible == visible)
		return;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Quad::setVisible, this, visible));

		return RequestMan.callInMainThread(functor);
	}

	if (visible && _needUpdate)
		update();

	Renderable::setVisible(visible);
}

void Quad::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Quad::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	_rootNode->setVisible(false);

	if (_entity) {
		_rootNode->detachObject(_entity);

		getOgreSceneManager(_scene).destroyMovableObject(_entity);
		_entity = 0;
	}

	getOgreSceneManager(_scene).destroySceneNode(_rootNode);
	_rootNode = 0;
}

void Quad::destroyEntity() {
	if (!_entity)
		return;

	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&Quad::destroyEntity, this));

		return RequestMan.callInMainThread(functor);
	}

	getOgreSceneManager(_scene).destroyMovableObject(_entity);

	_entity = 0;
}

void Quad::update() {
	destroyEntity();

	if (_newTexture) {
		if (_textureName.empty())
			_texture.setNull();

		try {
			_texture = TextureMan.get(_textureName);
		} catch (Common::Exception &e) {
			printException(e, "WARNING: ");
		}
	}

	_material = MaterialMan.createDynamic();

	if (!_texture.isNull()) {
		Ogre::TextureUnitState *texState = _material->getTechnique(0)->getPass(0)->createTextureUnitState();

		texState->setTexture(_texture);
	}

	MaterialMan.setColorModifier(_material, _r, _g, _b, _a);

	_material->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	if (_a < 1.0)
		_material->getTechnique(0)->getPass(0)->setDepthWriteEnabled(false);

	_entity = createQuadEntity(1.0, 1.0, _material, _topLeftU, _topLeftV, _bottomRightU, _bottomRightV, _scene);

	_entity->setQueryFlags(_selectable ? kSelectableQuad : kSelectableNone);
	_entity->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));

	_rootNode->attachObject(_entity);
	_rootNode->setVisible(_visible);


	_textureName.clear();

	_newTexture = false;
	_needUpdate = false;
}

void Quad::collectMaterials(std::list<Ogre::MaterialPtr> &materials, bool makeDynamic, bool makeTransparent) {
	if (makeDynamic)
		_entity->setMaterial((_material = MaterialMan.makeDynamic(_material)));

	if (makeTransparent)
		MaterialMan.setTransparent(_material, true);

	materials.push_back(_material);
}

} // End of namespace Aurora

} // End of namespace Graphics

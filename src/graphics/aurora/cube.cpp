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

/** @file graphics/aurora/cube.cpp
 *  A simple rotating cube.
 */

#include <OgreRoot.h>
#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreSubMesh.h>
#include <OgreMesh.h>
#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>

#include "common/ustring.h"
#include "common/util.h"
#include "common/error.h"
#include "common/uuid.h"

#include "graphics/util.h"
#include "graphics/meshutil.h"
#include "graphics/materialman.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/cube.h"

static const float  kSide1Vertices[] = { -1, -1, -1, 1, -1, -1, -1, 1, -1, 1, 1, -1 };
static const float  kSide1Normals [] = { 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1 };
static const uint16 kSide1Indices [] = { 0, 2, 1, 3, 1, 2 };
static const float  kSide2Vertices[] = { -1, -1, 1, 1, -1, 1, -1, 1, 1, 1, 1, 1 };
static const float  kSide2Normals [] = { 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1 };
static const uint16 kSide2Indices [] = { 0, 1, 2, 3, 2, 1 };
static const float  kSide3Vertices[] = { -1, -1, -1, -1, -1, 1, -1, 1, -1, -1, 1, 1 };
static const float  kSide3Normals [] = { -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0, 0 };
static const uint16 kSide3Indices [] = { 0, 1, 2, 3, 2, 1 };
static const float  kSide4Vertices[] = { 1, -1, -1, 1, -1, 1, 1, 1, -1, 1, 1, 1 };
static const float  kSide4Normals [] = { 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0 };
static const uint16 kSide4Indices [] = { 0, 2, 1, 3, 1, 2 };
static const float  kSide5Vertices[] = { -1, -1, -1, 1, -1, -1, -1, -1, 1, 1, -1, 1 };
static const float  kSide5Normals [] = { 0, -1, 0, 0, -1, 0, 0, -1, 0, 0, -1, 0 };
static const uint16 kSide5Indices [] = { 0, 1, 2, 3, 2, 1 };
static const float  kSide6Vertices[] = { -1, 1, -1, 1, 1, -1, -1, 1, 1, 1, 1, 1 };
static const float  kSide6Normals [] = { 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0 };
static const uint16 kSide6Indices [] = { 0, 2, 1, 3, 1, 2 };
static const float  kSideTexCoords[] = { 0, 0, 1, 0, 0, 1, 1, 1 };
static const float  kBounds       [] = { -1, -1, -1, 1, 1, 1};
static const float  kRadius          = 1.0;

namespace Graphics {

namespace Aurora {

Cube::Cube(const Common::UString &texture, const Common::UString &scene) : Renderable(scene), _entity(0) {
	Common::UString name = Common::generateIDNumberString();

	try {
		Ogre::MeshPtr mesh = getOgreMeshManager().createManual(name.c_str(), "General");

		// Create meshes

		Ogre::SubMesh *subMesh = 0;

		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide1Vertices, kSide1Normals, kSide1Indices, kSideTexCoords);
		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide2Vertices, kSide2Normals, kSide2Indices, kSideTexCoords);
		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide3Vertices, kSide3Normals, kSide3Indices, kSideTexCoords);
		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide4Vertices, kSide4Normals, kSide4Indices, kSideTexCoords);
		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide5Vertices, kSide5Normals, kSide5Indices, kSideTexCoords);
		subMesh = mesh->createSubMesh();
		createMesh(subMesh, 4, 2, kSide6Vertices, kSide6Normals, kSide6Indices, kSideTexCoords);

		// Bounding box / sphere

		mesh->_setBounds(Ogre::AxisAlignedBox(kBounds[0], kBounds[1], kBounds[2], kBounds[3], kBounds[4], kBounds[5]));
		mesh->_setBoundingSphereRadius(kRadius);

		mesh->load();

		// Create entity, put the requested material on it and attach it to a scene node

		_entity = getOgreSceneManager(_scene).createEntity(mesh);
		_entity->setQueryFlags(kSelectableNone);

		_entity->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));

		Ogre::MaterialPtr material = MaterialMan.create(texture, true);
		_materials.push_back(material);

		_entity->setMaterial(material);

		_rootNode = getOgreSceneManager(_scene).getRootSceneNode()->createChildSceneNode(name.c_str());

		_rootNode->attachObject(_entity);
		_rootNode->setVisible(false);

		// Create a rotating animations

		float animLength = 4.0f;
		int animSubDivisions = 4;

		Ogre::Animation *anim = getOgreSceneManager(_scene).createAnimation(name.c_str(), animLength);

		Ogre::NodeAnimationTrack *trackX = anim->createNodeTrack(0, _rootNode);
		Ogre::NodeAnimationTrack *trackY = anim->createNodeTrack(1, _rootNode);
		Ogre::NodeAnimationTrack *trackZ = anim->createNodeTrack(2, _rootNode);

		for (int i = 0; i < animSubDivisions; i++) {
			float degreePerSub = 360.0f / animSubDivisions;
			float timePerSub   = animLength / animSubDivisions;

			Ogre::Quaternion rotX(Ogre::Degree(i *  degreePerSub), Ogre::Vector3::UNIT_X);
			Ogre::Quaternion rotY(Ogre::Degree(i * -degreePerSub), Ogre::Vector3::UNIT_Y);
			Ogre::Quaternion rotZ(Ogre::Degree(i *  degreePerSub), Ogre::Vector3::UNIT_Z);

			trackX->createNodeKeyFrame(i * timePerSub)->setRotation(rotX);
			trackY->createNodeKeyFrame(i * timePerSub)->setRotation(rotY);
			trackZ->createNodeKeyFrame(i * timePerSub)->setRotation(rotZ);
		}

		Ogre::AnimationState *animState = getOgreSceneManager(_scene).createAnimationState(name.c_str());
		animState->setEnabled(false);

	} catch (std::exception &e) {
		throw Common::Exception("%s", e.what());
	}
}

Cube::~Cube() {
	if (!_rootNode)
		return;

	setVisible(false);

	Ogre::SceneManager &scene = getOgreSceneManager(_scene);

	destroyAnimation(_rootNode->getName().c_str());

	while (_rootNode->numAttachedObjects() > 0) {
		Ogre::MovableObject *object = _rootNode->detachObject((unsigned short) 0);

		if (object)
			scene.destroyMovableObject(object);
	}

	scene.destroySceneNode(_rootNode);
}

void Cube::startRotate() {
	_rootNode->setInitialState();
	getOgreSceneManager(_scene).getAnimationState(_rootNode->getName())->setEnabled(true);
}

void Cube::stopRotate() {
	getOgreSceneManager(_scene).getAnimationState(_rootNode->getName())->setEnabled(false);
}

void Cube::setSelectable(bool selectable) {
	_entity->setQueryFlags(selectable ? kSelectableCube : kSelectableNone);

	Renderable::setSelectable(selectable);
}

} // End of namespace Aurora

} // End of namespace Graphics

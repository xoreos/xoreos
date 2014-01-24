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

/** @file graphics/aurora/model.cpp
 *  Base class for models found in Aurora games.
 */

#include <OgreSceneNode.h>
#include <OgreSceneManager.h>
#include <OgreMeshManager.h>
#include <OgreEntity.h>
#include <OgreSubEntity.h>
#include <OgreAnimation.h>
#include <OgreAnimationState.h>

#include "common/error.h"
#include "common/stream.h"
#include "common/uuid.h"

#include "graphics/util.h"
#include "graphics/meshutil.h"
#include "graphics/materialman.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/model.h"

namespace Graphics {

namespace Aurora {

Model::NodeEntity::NodeEntity() : node(0), entity(0), dontRender(false), inheritedPosition(false) {
	position[0] = 0.0;
	position[1] = 0.0;
	position[2] = 0.0;

	orientation[0] = 1.0;
	orientation[1] = 0.0;
	orientation[2] = 0.0;
	orientation[3] = 0.0;
}


Model::State::State(const Common::UString &n) : name(n), animation(0), animationState(0) {
}


Model::Model(const Common::UString &file, const Common::UString &scene) : Renderable(scene),
	_fileName(file), _currentState(0) {

}

Model::~Model() {
	setVisible(false);
	setState(0);

	for (ModelMap::iterator c = _childModels.begin(); c != _childModels.end(); ++c)
		delete c->second;

	Ogre::SceneManager &scene = getOgreSceneManager(_scene);

	for (StateMap::iterator s = _states.begin(); s != _states.end(); ++s)
		destroyAnimation(s->second->animation);

	for (EntityList::iterator e = _entities.begin(); e != _entities.end(); ++e)
		scene.destroyMovableObject(*e);

	if (_rootNode) {
		_rootNode->removeAndDestroyAllChildren();

		scene.destroySceneNode(_rootNode);
	}

	for (StateMap::iterator s = _states.begin(); s != _states.end(); ++s)
		delete s->second;
}

std::vector<Common::UString> Model::getStates() const {
	std::vector<Common::UString> names;

	for (StateMap::const_iterator s = _states.begin(); s != _states.end(); ++s)
		names.push_back(s->first);

	return names;
}

std::vector<Common::UString> Model::getNodes() const {
	std::vector<Common::UString> names;

	State *state = _currentState;
	if (!state) {
		StateMap::const_iterator rootState = _states.find("");
		if (rootState != _states.end())
			state = rootState->second;
	}

	if (!state)
		return names;

	for (NodeEntities::const_iterator n = state->nodeEntities.begin(); n != state->nodeEntities.end(); ++n)
		names.push_back(n->first);

	return names;
}

bool Model::setState(const Common::UString &name) {
	StateMap::iterator s = _states.find(name);
	if (s == _states.end())
		return false;

	LOCK_FRAME();

	setState(s->second);
	return true;
}

void Model::setState(State *state) {
	if (_currentState) {
		for (NodeEntities::iterator n = _currentState->nodeEntities.begin(); n != _currentState->nodeEntities.end(); ++n)
			if (n->second.node) {
				n->second.node->showBoundingBox(false);
				if (n->second.entity)
					n->second.node->detachObject(n->second.entity);
			}
	}

	_currentState = state;

	if (_currentState) {
		for (NodeEntities::iterator n = _currentState->nodeEntities.begin(); n != _currentState->nodeEntities.end(); ++n)
			if (n->second.node) {
				n->second.node->setPosition(n->second.position[0], n->second.position[1], n->second.position[2]);
				n->second.node->setOrientation(n->second.orientation[0], n->second.orientation[1],
						n->second.orientation[2], n->second.orientation[3]);
				if (n->second.inheritedPosition)
					n->second.node->setInitialState();

				if (n->second.entity && !n->second.dontRender) {
					n->second.entity->setVisible(_visible);
					n->second.node->attachObject(n->second.entity);
				}
			}
	}
}

bool Model::playAnimation(bool loop) {
	if (!_currentState || !_currentState->animation || !_currentState->animationState)
		return false;

	LOCK_FRAME();

	_currentState->animationState->setLoop(loop);
	_currentState->animationState->setEnabled(true);
	return true;
}

bool Model::playAnimation(const Common::UString &name, bool loop) {
	LOCK_FRAME();

	if (setState(name))
		return false;

	return playAnimation(loop);
}

bool Model::stopAnimation() {
	if (!_currentState || !_currentState->animation || !_currentState->animationState)
		return false;

	LOCK_FRAME();

	_currentState->animationState->setEnabled(false);
	return true;
}

void Model::addToNode(Model *model, const Common::UString &node) {
	LOCK_FRAME();

	if (!model)
		throw Common::Exception("Trying to attach a non-existing model");

	// Get the nodes we operate on
	StateMap::iterator thisRootState = _states.find("");
	if (thisRootState == _states.end())
		throw Common::Exception("Model has no root state");

	NodeEntities::iterator attachNodeEntities = thisRootState->second->nodeEntities.find(node);
	if (attachNodeEntities == thisRootState->second->nodeEntities.end())
		throw Common::Exception("No node \"%s\" in model", node.c_str());

	Ogre::SceneNode *attachNode = attachNodeEntities->second.node;
	if (!attachNode)
		throw Common::Exception("Model node \"%s\" has no SceneNode", node.c_str());

	Ogre::SceneNode *otherRootNode = model->_rootNode;
	if (!otherRootNode)
		throw Common::Exception("Child model has no root node");

	Ogre::SceneNode *otherRootNodeParent = otherRootNode->getParentSceneNode();

	// Detach the root node from the other model and attach it to our node
	if (otherRootNodeParent)
		otherRootNodeParent->removeChild(otherRootNode);
	attachNode->addChild(otherRootNode);

	model->_rootNode = 0;

	// Override the properties for selecting the model
	for (EntityList::iterator e = model->_entities.begin(); e != model->_entities.end(); ++e) {
		(*e)->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));
		(*e)->setQueryFlags(_entities.empty() ? kSelectableNone : (*_entities.begin())->getQueryFlags());
	}

	// Assert visibility
	setVisible(_visible);

	// And register the model as a child of ours.
	_childModels.insert(std::make_pair(model->getID(), model));
}

void Model::setSelectable(bool selectable) {
	for (ModelMap::iterator c = _childModels.begin(); c != _childModels.end(); ++c)
		c->second->setSelectable(selectable);

	for (EntityList::iterator e = _entities.begin(); e != _entities.end(); ++e)
		(*e)->setQueryFlags(selectable ? kSelectableModel : kSelectableNone);

	Renderable::setSelectable(selectable);
}

void Model::showBoundingBox(bool show) {
	for (ModelMap::iterator c = _childModels.begin(); c != _childModels.end(); ++c)
		c->second->showBoundingBox(show);

	if (_currentState) {
		for (NodeEntities::iterator n = _currentState->nodeEntities.begin(); n != _currentState->nodeEntities.end(); ++n)
			if (n->second.node && !n->second.dontRender)
				n->second.node->showBoundingBox(show);
	}
}

void Model::collectMaterials(std::list<Ogre::MaterialPtr> &materials, bool makeDynamic, bool makeTransparent) {
	for (EntityList::iterator e = _entities.begin(); e != _entities.end(); ++e) {
		Ogre::MaterialPtr material = (*e)->getSubEntity(0)->getMaterial();
		if (makeDynamic)
			(*e)->setMaterial((material = MaterialMan.makeDynamic(material)));

		materials.push_back(material);
	}
}

void Model::createNode(NodeEntity *&nodeEntity, State *state, Common::UString name, Ogre::SceneNode *parent) {
	if (name.empty())
		name = Common::generateIDRandomString();

	// Create a node entitiy in this state
	std::pair<NodeEntities::iterator, bool> node = state->nodeEntities.insert(std::make_pair(name, NodeEntity()));
	nodeEntity = &node.first->second;

	// Copy the SceneNode from the root state if it exists
	NodeEntities::iterator rootNodeEntity = _states[""]->nodeEntities.find(name);
	if (rootNodeEntity != _states[""]->nodeEntities.end())
		nodeEntity->node = rootNodeEntity->second.node;

	// If we don't have a SceneNode now, create a new one
	if (!nodeEntity->node) {
		nodeEntity->node = parent->createChildSceneNode();
		nodeEntity->node->setVisible(false);
	}

	// If this node doesn't exist in the root state, create it
	if (rootNodeEntity ==_states[""]->nodeEntities.end()) {
		std::pair<NodeEntities::iterator, bool> newRootNode;
		newRootNode = _states[""]->nodeEntities.insert(std::make_pair(name, NodeEntity()));

		newRootNode.first->second.node       = nodeEntity->node;
		newRootNode.first->second.dontRender = true;
	}
}

Ogre::Entity *Model::createEntity(const VertexDeclaration &vertexDecl, const Ogre::MaterialPtr &material) {
	// Create a mesh according to the vertex declaration

	Ogre::MeshPtr mesh = Ogre::MeshManager::getSingleton().createManual(Common::generateIDRandomString().c_str(), "General");
	Ogre::SubMesh *subMesh = mesh->createSubMesh();

	createMesh(subMesh, vertexDecl);

	// Bounding box / sphere

	float minX, minY, minZ, maxX, maxY, maxZ, meshRadius;
	vertexDecl.getBounds(minX, minY, minZ, maxX, maxY, maxZ, meshRadius);

	mesh->_setBounds(Ogre::AxisAlignedBox(minX, minY, minZ, maxX, maxY, maxZ));
	mesh->_setBoundingSphereRadius(meshRadius);

	// Load the mesh and create an entity for it

	mesh->load();

	Ogre::Entity *entity = getOgreSceneManager(_scene).createEntity(mesh);
	entity->setQueryFlags(kSelectableNone);

	entity->getUserObjectBindings().setUserAny("renderable", Ogre::Any((Renderable *) this));

	// Assign the material to the entity
	entity->setMaterial(material.isNull() ? MaterialMan.getSolidColor(0.0, 0.0, 0.0, 0.0) : material);

	return entity;
}

void Model::readValue(Common::SeekableReadStream &stream, uint32 &value) {
	value = stream.readUint32LE();
}

void Model::readValue(Common::SeekableReadStream &stream, float &value) {
	value = stream.readIEEEFloatLE();
}

void Model::readArrayDef(Common::SeekableReadStream &stream,
                         uint32 &offset, uint32 &count) {

	offset = stream.readUint32LE();

	uint32 usedCount      = stream.readUint32LE();
	uint32 allocatedCount = stream.readUint32LE();

	if (usedCount != allocatedCount)
		warning("Model::readArrayDef(): usedCount != allocatedCount (%d, %d)",
		        usedCount, allocatedCount);

	count = usedCount;
}

template<typename T>
void Model::readArray(Common::SeekableReadStream &stream,
                      uint32 offset, uint32 count, std::vector<T> &values) {

	uint32 pos = stream.seekTo(offset);

	values.resize(count);
	for (uint32 i = 0; i < count; i++)
		readValue(stream, values[i]);

	stream.seekTo(pos);
}

template
void Model::readArray<uint32>(Common::SeekableReadStream &stream,
                              uint32 offset, uint32 count, std::vector<uint32> &values);
template
void Model::readArray<float>(Common::SeekableReadStream &stream,
                             uint32 offset, uint32 count, std::vector<float> &values);

} // End of namespace Aurora

} // End of namespace Graphics

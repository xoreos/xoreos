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
#include <OgreEntity.h>

#include "common/stream.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/meshutil.h"

namespace Graphics {

namespace Aurora {

Model::NodeEntity::NodeEntity() : node(0), entity(0), dontRender(false) {
}


Model::State::State(const Common::UString &n) : name(n) {
}


Model::Model() : _currentState(0) {
}

Model::~Model() {
	if (!_rootNode)
		return;

	setVisible(false);
	setState(0);

	Ogre::SceneManager &scene = getOgreSceneManager();

	if (scene.hasAnimationState(_rootNode->getName()))
		scene.destroyAnimationState(_rootNode->getName());
	if (scene.hasAnimation(_rootNode->getName()))
		scene.destroyAnimation(_rootNode->getName());

	for (EntityList::iterator e = _entities.begin(); e != _entities.end(); ++e)
		scene.destroyMovableObject(*e);

	_rootNode->removeAndDestroyAllChildren();

	scene.destroySceneNode(_rootNode);

	for (StateMap::iterator s = _states.begin(); s != _states.end(); ++s)
		delete s->second;
}

void Model::setState(State *state) {
	if (_currentState) {
		for (NodeEntities::iterator n = _currentState->nodeEntities.begin(); n != _currentState->nodeEntities.end(); ++n)
			if (n->second.node && n->second.entity)
				n->second.node->detachObject(n->second.entity);
	}

	_currentState = state;

	if (_currentState) {
		for (NodeEntities::iterator n = _currentState->nodeEntities.begin(); n != _currentState->nodeEntities.end(); ++n)
			if (n->second.node && n->second.entity && !n->second.dontRender) {
				n->second.entity->setVisible(_visible);
				n->second.node->attachObject(n->second.entity);
			}
	}
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

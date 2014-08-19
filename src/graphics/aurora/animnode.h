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

/** @file graphics/aurora/animnode.h
 *  A node within an animation.
 */

#ifndef GRAPHICS_AURORA_ANIMNODE_H
#define GRAPHICS_AURORA_ANIMNODE_H

#include <list>
#include <vector>

#include "common/ustring.h"
#include "common/transmatrix.h"
#include "common/boundingbox.h"

#include "graphics/types.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/textureman.h"

namespace Graphics {

namespace Aurora {

class ModelNode;

class AnimNode {
public:
	AnimNode(ModelNode *modelnode);
	~AnimNode();

	/** Get the node's name. */
	const Common::UString &getName() const;

	/** Update the model properties interpolating between frames */
	void update(Model *model, float lastFrame, float nextFrame, float scale);
protected:
	// Animation *_animation; ///< The animation this node belongs to.

	AnimNode *_parent;               ///< The node's parent.
	std::list<AnimNode *> _children; ///< The node's children.

	Common::UString _name; ///< The node's name.
	ModelNode *_nodedata;

public:
	// General helpers

	AnimNode *getParent();             ///< Get the node's parent.
	const AnimNode *getParent() const; ///< Get the node's parent.

	void setParent(AnimNode *parent); ///< Set the node's parent.

	// void reparent(AnimNode &parent);


	friend class Animation;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ANIMNODE_H

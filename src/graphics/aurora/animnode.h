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
 *  A node within an animation.
 */

#ifndef GRAPHICS_AURORA_ANIMNODE_H
#define GRAPHICS_AURORA_ANIMNODE_H

#include <list>

#include "src/common/ustring.h"
#include "src/common/matrix4x4.h"
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"

#include "src/graphics/aurora/types.h"

namespace Graphics {

namespace Aurora {

class AnimNode {
public:
	AnimNode(ModelNode *modelnode);
	~AnimNode();

	/** Get the node's name. */
	const Common::UString &getName() const;
	/** Get the node's model node. */
	ModelNode *getNodeData();

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

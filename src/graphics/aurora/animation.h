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
 *  An animation to be applied to a model.
 */

#ifndef GRAPHICS_AURORA_ANIMATION_H
#define GRAPHICS_AURORA_ANIMATION_H

#include <vector>
#include <list>
#include <map>

#include "src/common/ustring.h"
#include "src/common/transmatrix.h"
#include "src/common/boundingbox.h"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

class AnimNode;

class Animation {
public:
	Animation();
	~Animation();

	/** Get the animation's name. */
	const Common::UString &getName() const;
	void setName(Common::UString &name);

protected:
	typedef std::list<AnimNode *> NodeList;
	typedef std::map<Common::UString, AnimNode *, Common::UString::iless> NodeMap;

	NodeList nodeList; ///< The nodes within the state.
	NodeMap  nodeMap;  ///< The nodes within the state, indexed by name.

	NodeList rootNodes; ///< The nodes in the state without a parent.

	Common::UString _name; ///< The model's name.
	float _length;
	float _transtime;

public:
	void setLength(float length);
	float getLength() const;
	void setTransTime(float transtime);

	void update(Model *model, float lastFrame, float nextFrame);
	void addAnimNode(AnimNode *node);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ANIMATION_H

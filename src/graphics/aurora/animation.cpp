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

#include "src/common/readstream.h"
#include "src/common/debug.h"

#include "src/graphics/graphics.h"
#include "src/graphics/camera.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/animation.h"
#include "src/graphics/aurora/animnode.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Animation::Animation() {

}

Animation::~Animation() {
	for (NodeList::iterator n = nodeList.begin(); n != nodeList.end(); ++n)
		delete *n;
}

const Common::UString &Animation::getName() const {
	return _name;
}

void Animation::setName(Common::UString &name) {
	_name = name;
}

void Animation::setLength(float length) {
	_length = length;
}

float Animation::getLength() const {
	return _length;
}

void Animation::setTransTime(float transtime) {
	_transtime = transtime;
}

void Animation::update(Model *model, float lastFrame, float nextFrame) {
	// TODO: Also need to fire off associated events
	//       for event in _events event->fire()


	float scale = model->getAnimationScale(_name);
	for (NodeList::iterator n = nodeList.begin();
	     n != nodeList.end(); ++n) {
		(*n)->update(model, lastFrame, nextFrame, scale);
	}
}

void Animation::addAnimNode(AnimNode *node) {
	nodeList.push_back(node);
	nodeMap.insert(std::make_pair(node->getName(), node));
}

} // End of namespace Aurora

} // End of namespace Graphics

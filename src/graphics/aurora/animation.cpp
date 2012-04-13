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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/animation.cpp
 *  An animation to be applied to a model.
 */

#include "common/stream.h"
#include "common/debug.h"

#include "graphics/graphics.h"
#include "graphics/camera.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/animation.h"
#include "graphics/aurora/animnode.h"

using Common::kDebugGraphics;

namespace Graphics {

namespace Aurora {

Animation::Animation() {

}

Animation::~Animation() {
}

const Common::UString &Animation::getName() const {
	return _name;
}

void Animation::setName(Common::UString& name) {
	_name=name;
}

void Animation::setLength(float length) {
	_length=length;
}

float Animation::getLength() {
	return _length;
}

void Animation::setTransTime(float transtime) {
	_transtime=transtime;
}

void Animation::update(Model *model, float lastFrame, float nextFrame) {
	//TODO: also need to fire off associated events
	//for event in _events event->fire()

	for (NodeList::iterator n = nodeList.begin();
	        n != nodeList.end(); ++n) {
		(*n)->update(model, lastFrame, nextFrame);
	}
}

void Animation::addAnimNode(AnimNode* node) {
	nodeList.push_back(node);
	nodeMap.insert(std::make_pair(node->getName(), node));
}

} // End of namespace Aurora

} // End of namespace Graphics

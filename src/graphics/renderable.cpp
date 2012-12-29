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

/** @file graphics/renderable.cpp
 *  An object that can be displayed by the graphics manager.
 */

#include "common/error.h"

#include "graphics/renderable.h"
#include "graphics/types.h"
#include "graphics/graphics.h"

namespace Graphics {

Renderable::Renderable(RenderableType type) : _clickable(false), _distance(0.0) {
	if        (type == kRenderableTypeVideo) {
		_queueExists  = kQueueVideo;
		_queueVisible = kQueueVisibleVideo;
	} else if (type == kRenderableTypeObject) {
		_queueExists  = kQueueWorldObject;
		_queueVisible = kQueueVisibleWorldObject;
	} else if (type == kRenderableTypeGUIFront) {
		_queueExists  = kQueueGUIFrontObject;
		_queueVisible = kQueueVisibleGUIFrontObject;
	} else
		throw Common::Exception("Unknown Renderable type %d", type);

	addToQueue(_queueExists);

	_id = GfxMan.createRenderableID();
}

Renderable::~Renderable() {
	hide();

	removeFromQueue(_queueExists);
}

bool Renderable::operator<(const Queueable &q) const {
	return _distance < static_cast<const Renderable &>(q)._distance;
}

double Renderable::getDistance() const {
	return _distance;
}

uint32 Renderable::getID() const {
	return _id;
}

bool Renderable::isClickable() const {
	return _clickable;
}

void Renderable::setClickable(bool clickable) {
	_clickable = clickable;
}

const Common::UString &Renderable::getTag() const {
	return _tag;
}

void Renderable::setTag(const Common::UString &tag) {
	_tag = tag;
}

bool Renderable::isVisible() const {
	return isInQueue(_queueVisible);
}

void Renderable::resort() {
	sortQueue(_queueVisible);
}

void Renderable::show() {
	lockQueue(_queueVisible);

	addToQueue(_queueVisible);
	sortQueue(_queueVisible);

	unlockQueue(_queueVisible);
}

void Renderable::hide() {
	removeFromQueue(_queueVisible);
}

bool Renderable::isIn(float x, float y) const {
	return false;
}

bool Renderable::isIn(float x, float y, float z) const {
	return false;
}

bool Renderable::isIn(float x1, float y1, float z1, float x2, float y2, float z2) const {
	return false;
}

} // End of namespace Graphics

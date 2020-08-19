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
 *  An object that can be displayed by the graphics manager.
 */

#include "src/common/system.h"
#include "src/common/error.h"

#include "src/graphics/renderable.h"
#include "src/graphics/graphics.h"

namespace Graphics {

Renderable::Renderable(RenderableType type) : _clickable(false), _distance(0.0f) {
	switch (type) {
		case kRenderableTypeVideo:
			_queueExists  = kQueueVideo;
			_queueVisible = kQueueVisibleVideo;
			break;

		case kRenderableTypeObject:
			_queueExists  = kQueueWorldObject;
			_queueVisible = kQueueVisibleWorldObject;
			break;

		case kRenderableTypeGUIFront:
			_queueExists  = kQueueGUIFrontObject;
			_queueVisible = kQueueVisibleGUIFrontObject;
			break;

		case kRenderableTypeGUIBack:
			_queueExists  = kQueueGUIBackObject;
			_queueVisible = kQueueVisibleGUIBackObject;
			break;

		case kRenderableTypeConsole:
			_queueExists  = kQueueGUIConsoleObject;
			_queueVisible = kQueueVisibleGUIConsoleObject;
			break;

		default:
			throw Common::Exception("Unknown Renderable type %d", type);
	}

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

void Renderable::advanceTime(float UNUSED(dt)) {
}

double Renderable::getDistance() const {
	return _distance;
}

uint32_t Renderable::getID() const {
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

bool Renderable::isIn(float UNUSED(x), float UNUSED(y)) const {
	return false;
}

bool Renderable::isIn(float UNUSED(x), float UNUSED(y), float UNUSED(z)) const {
	return false;
}

bool Renderable::isIn(float UNUSED(x1), float UNUSED(y1), float UNUSED(z1),
                      float UNUSED(x2), float UNUSED(y2), float UNUSED(z2)) const {

	return false;
}

void Renderable::lockFrame() {
	GfxMan.lockFrame();
}

void Renderable::unlockFrame() {
	GfxMan.unlockFrame();
}

void Renderable::lockFrameIfVisible() {
	if (isVisible())
		GfxMan.lockFrame();
}

void Renderable::unlockFrameIfVisible() {
	if (isVisible())
		GfxMan.unlockFrame();
}

} // End of namespace Graphics

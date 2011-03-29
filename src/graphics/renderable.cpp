/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	if        (type == kRenderableTypeObject) {
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

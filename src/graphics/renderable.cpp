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

#include "graphics/renderable.h"
#include "graphics/types.h"

namespace Graphics {

Renderable::Renderable(Queueable<Renderable>::Queue &queue) : Queueable<Renderable>(queue) {
	_distance = 0.0;
}

Renderable::~Renderable() {
}

double Renderable::getDistance() const {
	return _distance;
}

const Common::UString &Renderable::getTag() const {
	return _tag;
}

void Renderable::setTag(const Common::UString &tag) {
	_tag = tag;
}

bool Renderable::isIn(float x, float y) const {
	return false;
}

bool Renderable::isIn(float x, float y, float z) const {
	return false;
}

void Renderable::changedResolution(int oldWidth, int oldHeight, int newWidth, int newHeight) {
}

void Renderable::setCurrentDistance() {
	GLfloat modelView[16];

	glGetFloatv(GL_MODELVIEW_MATRIX, modelView);
	_distance = modelView[12] * modelView[12] + modelView[13] * modelView[13] + modelView[14] * modelView[14];
}

} // End of namespace Graphics

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/glcontainer.cpp
 *  A container of OpenGL elements.
 */

#include "common/threads.h"

#include "events/requests.h"

#include "graphics/glcontainer.h"

namespace Graphics {

GLContainer::GLContainer() : _built(false) {
}

GLContainer::~GLContainer() {
}

void GLContainer::rebuild() {
	if (!Common::isMainThread()) {
		RequestMan.dispatchAndWait(RequestMan.rebuild(*this));
		return;
	}

	doRebuild();

	_built = true;
}

void GLContainer::destroy() {
	if (!_built)
		return;

	if (!Common::isMainThread()) {
		RequestMan.dispatchAndWait(RequestMan.destroy(*this));
		return;
	}

	doDestroy();

	_built = false;
}

} // End of namespace Graphics

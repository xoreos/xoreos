/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/font.cpp
 *  A font.
 */

#include "graphics/types.h"

#include "graphics/aurora/font.h"

namespace Graphics {

namespace Aurora {

Font::Font() {
}

Font::~Font() {
}

void Font::draw(const Common::UString &text, float r, float g, float b, float a) const {
	glColor4f(r, g, b, a);
	draw(text);
	glColor4f(1.0, 1.0, 1.0, 1.0);
}

} // End of namespace Aurora

} // End of namespace Graphics

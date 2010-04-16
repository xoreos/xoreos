/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/text.cpp
 *  A text object.
 */

#include "graphics/text.h"
#include "graphics/font.h"

namespace Graphics {

Text::Text(const Font &font, const std::string &str) : _font(&font), _str(str) {
	addToQueue();
}

Text::~Text() {
}

void Text::newFrame() {
}

void Text::render() {
}

} // End of namespace Graphics

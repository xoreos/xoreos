/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/font.h
 *  A font.
 */

#ifndef GRAPHICS_FONT_H
#define GRAPHICS_FONT_H

#include <string>
#include <vector>

#include "common/types.h"

#include "graphics/types.h"
#include "graphics/listcontainer.h"

namespace Graphics {

class Texture;

/** A font. */
class Font : public ListContainer {
public:
	Font(const std::string &name);
	~Font();

// ListContainer
public:
	void rebuild();
	void destroy();

private:
	struct Char {
		ListID listID;
		double tX[4], tY[4];
		double vX[4], vY[4];
	};

	Texture *_texture;

	uint32 _charCount;
	ListID _listStart;

	std::vector<Char> _chars;

	void load();
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H

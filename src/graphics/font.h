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

	/** Set the font's texture. */
	void setTexture() const;

	/** Draw a character and return its width. */
	float drawCharacter(char c) const;

	float getScale()  const;
	float getSpaceR() const;
	float getSpaceB() const;

// ListContainer
public:
	void rebuild();
	void destroy();

private:
	struct Char {
		ListID listID;
		float width;
		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	bool _firstTime;

	Texture *_texture;

	ListID _listStart;

	std::vector<Char> _chars;

	float _scale;
	float _spaceR;
	float _spaceB;

	void load();
};

} // End of namespace Graphics

#endif // GRAPHICS_FONT_H

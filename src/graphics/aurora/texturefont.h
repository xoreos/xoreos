/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/texturefont.h
 *  A texture font, as used by NWN and KotOR/KotOR2.
 */

#ifndef GRAPHICS_AURORA_TEXTUREFONT_H
#define GRAPHICS_AURORA_TEXTUREFONT_H

#include <vector>

#include "common/types.h"

#include "graphics/font.h"

#include "graphics/aurora/textureman.h"

namespace Common {
	class UString;
}

namespace Graphics {

namespace Aurora {

class Texture;

/** A texture font, as used by NWN and KotOR/KotOR2. */
class TextureFont : public Graphics::Font {
public:
	TextureFont(const Common::UString &name);
	~TextureFont();

	float getWidth(const Common::UString &text) const;
	float getHeight(const Common::UString &text) const;

	void draw(const Common::UString &text) const;

private:
	/** A font character. */
	struct Char {
		float width;
		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	TextureHandle _texture;

	std::vector<Char> _chars;

	float _scale;
	float _spaceR;
	float _spaceB;

	void load();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXTUREFONT_H

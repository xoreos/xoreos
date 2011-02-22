/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/abcfont.h
 *  An ABC/SBM font, as used by Jade Empire.
 */

#ifndef GRAPHICS_AURORA_ABCFONT_H
#define GRAPHICS_AURORA_ABCFONT_H

#include "common/types.h"

#include "graphics/font.h"

#include "graphics/aurora/textureman.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

/** An ABC/SBM font, as used by Jade Empire. */
class ABCFont : public Graphics::Font {
public:
	ABCFont(const Common::UString &name);
	~ABCFont();

	float getWidth(const Common::UString &text) const;
	float getHeight(const Common::UString &text) const;

	void draw(const Common::UString &text) const;

private:
	/** A font character. */
	struct Char {
		uint32 dataX;
		uint32 dataY;
		uint8  width;
		int8   spaceL;
		int8   spaceR;

		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	TextureHandle _texture;

	uint8 _base;

	Char _invalid;
	Char _ascii[128];
	std::map<uint32, Char> _extended;

	void load(const Common::UString &name);

	void readCharDesc(Char &c, Common::SeekableReadStream &abc);
	void calcCharVertices(Char &c);

	const Char &findChar(uint32 c) const;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ABCFONT_H

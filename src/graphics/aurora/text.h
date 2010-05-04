/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/text.h
 *  A text object.
 */

#ifndef GRAPHICS_AURORA_TEXT_H
#define GRAPHICS_AURORA_TEXT_H

#include "common/ustring.h"

#include "graphics/guifrontelement.h"

namespace Graphics {

namespace Aurora {

class Font;

class Text : public GUIFrontElement {
public:
	Text(const Font &font, float x, float y, const Common::UString &str);
	~Text();

	void newFrame();

	void render();

private:
	bool _firstTime;

	const Font *_font;

	float _x;
	float _y;

	Common::UString _str;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXT_H

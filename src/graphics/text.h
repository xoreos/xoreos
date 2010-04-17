/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/text.h
 *  A text object.
 */

#ifndef GRAPHICS_TEXT_H
#define GRAPHICS_TEXT_H

#include <string>

#include "graphics/guifrontelement.h"

namespace Graphics {

class Font;

class Text : public GUIFrontElement {
public:
	Text(const Font &font, double x, double y, const std::string &str);
	~Text();

	void newFrame();

	void render();

private:
	bool _firstTime;

	const Font *_font;

	double _x;
	double _y;

	std::string _str;
};

} // End of namespace Graphics

#endif // GRAPHICS_TEXT_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include "graphics/types.h"
#include "graphics/listcontainer.h"
#include "graphics/guifrontelement.h"

#include "graphics/aurora/fontman.h"

namespace Graphics {

namespace Aurora {

/** A text object. */
class Text : public GUIFrontElement, public ListContainer {
public:
	Text(const FontHandle &font, const Common::UString &str, float align = 0.0);
	Text(const FontHandle &font, const Common::UString &str, float r, float g, float b, float a,
	     float align = 0.0);
	~Text();

	void set(const Common::UString &str);
	void setPosition(float x, float y);
	void setColor(float r, float g, float b, float a);
	void unsetColor();

	void show(); ///< The text should be rendered.
	void hide(); ///< The text should not be rendered.

	bool isVisible(); // Is the text visible?

	float getWidth()  const;
	float getHeight() const;

	// Renderable
	void newFrame();
	void render();

protected:
	void doRebuild();
	void doDestroy();

	void set_internal(const Common::UString &str);
	void setPosition_internal(float x, float y);

private:
	FontHandle _font;

	float _x;
	float _y;

	float _width;
	float _height;

	bool _hasColor;
	float _r, _g, _b, _a;

	float _align;

	Common::UString _str;

	ListID _list;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXT_H

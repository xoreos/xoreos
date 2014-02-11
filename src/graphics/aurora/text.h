/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file graphics/aurora/text.h
 *  A text object.
 */

#ifndef GRAPHICS_AURORA_TEXT_H
#define GRAPHICS_AURORA_TEXT_H

#include "common/ustring.h"

#include "graphics/renderable.h"

#include "graphics/aurora/fontman.h"

namespace Graphics {

namespace Aurora {

class Text : public Renderable {
public:
	Text(const FontHandle &font, const Common::UString &str, float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);
	~Text();

	/** Return the string of this text object. */
	const Common::UString &get() const;

	/** Return the color of this text object. */
	void getColor(float &r, float &g, float &b, float &a) const;

	/** Set the string of this text object. */
	void set(const Common::UString &str);

	/** Set the color of this text object. */
	void setColor(float r, float g, float b, float a = 1.0);

	/** Show/Hide the bouding box(es) of this text object. */
	void showBoundingBox(bool show);

	/** Change whether the text object can be selected (picked) by the user. */
	void setSelectable(bool selectable);

private:
	FontHandle _font;

	Common::UString _str;

	float _r;
	float _g;
	float _b;
	float _a;


	void destroy();
	void create();

	void destroyText();
	void createText();

	void updateColor();
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_TEXT_H

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

/** @file engines/kotor/gui/widgets/kotorwidget.h
 *  A KotOR widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H
#define ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"
#include "graphics/aurora/highlightable.h"
#include "graphics/aurora/highlightabletext.h"

#include "engines/aurora/widget.h"

namespace Engines {

namespace KotOR {

/** Base class for all widgets in KotOR. */
class KotORWidget : public Widget {
public:
	KotORWidget(::Engines::GUI &gui, const Common::UString &tag);
	~KotORWidget();

	virtual void load(const Aurora::GFFStruct &gff);

	void show();
	void hide();

	void setTag(const Common::UString &tag);

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setFill(const Common::UString &fill);
	void setColor(float r, float g, float b, float a);
	void setText(const Common::UString &text);

	Graphics::Aurora::Highlightable *getTextHighlightableComponent() const;

protected:
	struct Extend {
		float x;
		float y;
		float w;
		float h;

		Extend();
	};

	struct Border {
		Common::UString corner;
		Common::UString edge;
		Common::UString fill;

		uint32 fillStyle;
		uint32 dimension;
		uint32 innerOffset;

		float r, g, b;

		bool pulsing;

		Border();
	};

	struct Text {
		Common::UString font;
		Common::UString text;
		uint32 strRef;

		float halign;
		float valign;

		float r, g, b;

		bool pulsing;

		Text();
	};

	float _width;
	float _height;

	float _r;
	float _g;
	float _b;
	float _a;

	Graphics::Aurora::GUIQuad *_quad;
	Graphics::Aurora::HighlightableText *_text;


	Extend createExtend(const Aurora::GFFStruct &gff);
	Border createBorder(const Aurora::GFFStruct &gff);
	Text   createText  (const Aurora::GFFStruct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H

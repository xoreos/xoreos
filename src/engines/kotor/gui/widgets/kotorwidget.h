/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A KotOR widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H
#define ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"
#include "src/graphics/aurora/highlightable.h"
#include "src/graphics/aurora/highlightabletext.h"
#include "src/graphics/aurora/borderquad.h"
#include "src/graphics/aurora/subscenequad.h"

#include "src/engines/aurora/widget.h"

namespace Engines {

namespace KotOR {

/** Base class for all widgets in KotOR. */
class KotORWidget : public Widget {
public:
	KotORWidget(::Engines::GUI &gui, const Common::UString &tag);
	~KotORWidget();

	virtual void load(const Aurora::GFF3Struct &gff);

	void show();
	void hide();

	void setWrapped(bool wrapped);

	void setTag(const Common::UString &tag);

	void setPosition(float x, float y, float z);
	void setSubScene(Graphics::Aurora::SubSceneQuad *subscene);

	virtual void setInvisible(bool invisible);

	/** Create a scissor test over this widget. */
	void setScissor(int x, int y, int width, int height);

	/** Change the font for this widget. */
	void setFont(const Common::UString &fnt);

	/** Set the widget clickable, or not clickable. */
	void setClickable(bool clickable);

	/** Set if the widget should be highlighted. */
	void setHighlight(bool highlight);
	/** If the widget is highlighted. */
	bool isHighlight();

	/** Set the width of the widget. */
	void setWidth(float width);
	/** Set the height of the widget. */
	void setHeight(float height);

	float getWidth () const;
	float getHeight() const;

	void setFill(const Common::UString &fill);
	void setColor(float r, float g, float b, float a);
	void setText(const Common::UString &text);

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

	struct Hilight {
		Common::UString fill;

		Hilight();
	};

	Graphics::Aurora::Highlightable *getTextHighlightableComponent() const;
	Graphics::Aurora::Highlightable *getQuadHighlightableComponent() const;

	float _width;
	float _height;

	float _r;
	float _g;
	float _b;
	float _a;

	float _unselectedR, _unselectedG, _unselectedB, _unselectedA;

	bool _wrapped;

	Common::ScopedPtr<Graphics::Aurora::GUIQuad>           _quad;
	Common::ScopedPtr<Graphics::Aurora::GUIQuad>           _highlight;
	Common::ScopedPtr<Graphics::Aurora::HighlightableText> _text;
	Common::ScopedPtr<Graphics::Aurora::BorderQuad>        _border;

	Graphics::Aurora::SubSceneQuad *_subScene;

	bool _highlighted;

	Extend  createExtend (const Aurora::GFF3Struct &gff);
	Border  createBorder (const Aurora::GFF3Struct &gff);
	Text    createText   (const Aurora::GFF3Struct &gff);
	Hilight createHilight(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_KOTORWIDGET_H

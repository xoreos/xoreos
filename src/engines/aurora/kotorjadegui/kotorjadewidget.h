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
 *  Common base for Star Wars: Knights of the Old Republic and Jade Empire widgets.
 */

#ifndef ENGINES_AURORA_KOTORJADEGUI_KOTORJADEWIDGET_H
#define ENGINES_AURORA_KOTORJADEGUI_KOTORJADEWIDGET_H

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

class KotORJadeWidget : public Widget {
public:
	KotORJadeWidget(GUI &gui, const Common::UString &tag);
	~KotORJadeWidget();

	virtual void load(const Aurora::GFF3Struct &gff);

	/** Set the widget clickable, or not clickable. */
	void setClickable(bool clickable);
	/** Create a scissor test over this widget. */
	void setScissor(int x, int y, int width, int height);

	void setFill(const Common::UString &fill);
	void setFill(const Graphics::Aurora::TextureHandle &handle);
	void setColor(float r, float g, float b, float a);
	void setBorderColor(float r, float g, float b, float a);
	void setWrapped(bool wrapped);
	void setSubScene(Graphics::Aurora::SubSceneQuad *subscene);

	float getBorderDimension() const;

	// .--- Highlighting

	void setHighlight(const Common::UString &hilight);

	/** Set if the widget should be highlighted. */
	void setHighlight(bool highlight);
	/** If the widget is highlighted. */
	bool isHighlight();

	// '---

	// .--- Text
	/** Initialize the text within this widget. */
	void createText(const Common::UString &font, const Common::UString &str);

	/** Change the font for this widget. */
	void setFont(const Common::UString &fnt);

	void setText(const Common::UString &text);
	void setTextColor(float r, float g, float b, float a);
	void setHorizontalTextAlign(float halign);
	void setVerticalTextAlign(float valign);

	float getTextHeight(const Common::UString &text) const;

	// '---

	void setTag(const Common::UString &tag);

	virtual void show();
	virtual void hide();

	virtual void setPosition(float x, float y, float z);

	// .--- Size

	/** Set the width of the widget. */
	virtual void setWidth(float width);
	/** Set the height of the widget. */
	virtual void setHeight(float height);

	float getWidth () const;
	float getHeight() const;

	// '---

	virtual void setInvisible(bool invisible);

protected:
	struct Extend {
		float x, y, w, h;

		Extend();
	};

	struct Border {
		Common::UString corner;
		Common::UString edge;
		Common::UString fill;

		uint32 fillStyle;
		uint32 dimension;
		uint32 innerOffset;

		bool hasColor;
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
	uint32 _borderDimension;

	float _r, _g, _b, _a;
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

} // End of namespace Engines

#endif // ENGINES_AURORA_KOTORJADEGUI_KOTORJADEWIDGET_H

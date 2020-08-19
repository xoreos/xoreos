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
 *  Common base for the Odyssey engine widgets.
 */

#ifndef ENGINES_ODYSSEY_WIDGET_H
#define ENGINES_ODYSSEY_WIDGET_H

#include "src/graphics/aurora/borderquad.h"
#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/highlightabletext.h"

#include "src/engines/aurora/widget.h"

namespace Aurora {
	class GFF3Struct;
}

namespace Graphics {
	namespace Aurora {
		class Highlightable;
		class SubSceneQuad;
	}
}

namespace Engines {

namespace Odyssey {

class Widget : public Engines::Widget {
public:
	Widget(GUI &gui, const Common::UString &tag);

	virtual void load(const Aurora::GFF3Struct &gff);

	// Basic properties

	/** Set the widget tag. */
	void setTag(const Common::UString &tag);
	/** Set if the widget is clickable. */
	void setClickable(bool clickable);
	/** Create a scissor test over this widget. */
	void setScissor(int x, int y, int width, int height);
	/** Set fill texture of the widget. */
	void setFill(const Common::UString &fill);
	/** Set the widget color. */
	void setColor(float r, float g, float b, float a);
	/** Set if the widget should be wrapped. */
	void setWrapped(bool wrapped);

	// Basic visuals

	/** Set if the widget should be invisible. */
	virtual void setInvisible(bool invisible);

	/** Show the widget. */
	virtual void show();
	/** Hide the widget. */
	virtual void hide();

	// Positioning

	/** Set the widget position. */
	virtual void setPosition(float x, float y, float z);
	/** Set the widget rotation in degrees. */
	virtual void setRotation(float angle);

	// Size

	/** Get the widget width. */
	float getWidth() const;
	/** Get the widget height. */
	float getHeight() const;

	/** Set the widget width. */
	virtual void setWidth(float width);
	/** Set the widget height. */
	virtual void setHeight(float height);

	// Border

	/** Get size of the widget border. */
	float getBorderDimension() const;

	/** Set color of the widget border. */
	void setBorderColor(float r, float g, float b, float a);

	// Text

	/** Get height of the specified text within the widget. */
	float getTextHeight(const Common::UString &text) const;

	/** Set the widget font. */
	void setFont(const Common::UString &fnt);
	/** Set the widget text. */
	void setText(const Common::UString &text);
	/** Set color of the widget text. */
	void setTextColor(float r, float g, float b, float a);
	/** Set horizontal alignment of the widget text. */
	void setHorizontalTextAlign(float halign);
	/** Set vertical alignment of the widget text. */
	void setVerticalTextAlign(float valign);

	/** Initialize the widget text. */
	void createText(const Common::UString &font, const Common::UString &str);

	// Highlighting

	/** Is the widget highlighted? */
	bool isHighlight() const;

	/** Set if the widget should be highlighted. */
	void setHighlight(bool highlight);
	/** Set highlight texture of the widget. */
	void setHighlight(const Common::UString &hilight);

	// Sub scene

	/** Set sub scene of the widget. */
	void setSubScene(Graphics::Aurora::SubSceneQuad *subscene);

protected:
	struct Extend {
		float x, y, w, h;

		Extend();
	};

	struct Border {
		Common::UString corner;
		Common::UString edge;
		Common::UString fill;

		uint32_t fillStyle;
		uint32_t dimension;
		uint32_t innerOffset;

		bool hasColor;
		float r, g, b;

		bool pulsing;

		Border();
	};

	struct Text {
		Common::UString font;
		Common::UString text;
		uint32_t strRef;

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
	uint32_t _borderDimension;

	float _r, _g, _b, _a;
	float _unselectedR, _unselectedG, _unselectedB, _unselectedA;

	bool _wrapped;

	std::unique_ptr<Graphics::Aurora::GUIQuad>           _quad;
	std::unique_ptr<Graphics::Aurora::GUIQuad>           _highlight;
	std::unique_ptr<Graphics::Aurora::HighlightableText> _text;
	std::unique_ptr<Graphics::Aurora::BorderQuad>        _border;

	Graphics::Aurora::SubSceneQuad *_subScene;

	bool _highlighted;

	Extend  createExtend (const Aurora::GFF3Struct &gff);
	Border  createBorder (const Aurora::GFF3Struct &gff);
	Text    createText   (const Aurora::GFF3Struct &gff);
	Hilight createHilight(const Aurora::GFF3Struct &gff);
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_WIDGET_H

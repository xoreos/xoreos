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
 *  A NWN scrollbar model and widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H
#define ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H

#include <vector>

#include "src/graphics/guielement.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"

namespace Engines {

namespace NWN {

class GUI;

/** A NWN scrollbar model. */
class Scrollbar : public Graphics::GUIElement {
public:
	enum Type {
		kTypeVertical,  ///< Vertical scrollbar.
		kTypeHorizontal ///< Horizontal scrollbar.
	};

	Scrollbar(Type type);
	~Scrollbar();

	/** Set the current position of the scrollbar. */
	void setPosition(float x, float y, float z);

	/** Get the current position of the scrollbar. */
	void getPosition(float &x, float &y, float &z) const;

	/** Is the point within the scrollbar? */
	bool isIn(float x, float y) const;

	/** Set the scrollbar length */
	void setLength(float length);

	float getWidth () const; ///< Get the scrollbar's width.
	float getHeight() const; ///< Get the scrollbar's height.

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	struct Quad {
		float vX[4], vY[4];
		float tX[4], tY[4];
	};

	Type _type;

	float _x;
	float _y;
	float _z;

	float _length;

	std::vector<Quad> _quads;

	Graphics::Aurora::TextureHandle _texture;

	void createV(); ///< Create a vertical scrollbar.
	void createH(); ///< Create a horizontal scrollbar.
};

/** A NWN scrollbar widget. */
class WidgetScrollbar : public NWNWidget {
public:
	WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
	                Scrollbar::Type type, float range);
	~WidgetScrollbar();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	/** Set the length of the scrollbar, as a fraction of the range. */
	void setLength(float length);

	/** Get the current state, as a fraction of the range. */
	float getState() const;
	/** Set the current state, as a fraction of the range. */
	void setState(float state);

	float getWidth () const;
	float getHeight() const;

	float getBarPosition() const;

	void mouseDown(uint8 state, float x, float y);
	void mouseMove(uint8 state, float x, float y);
	void mouseWheel(uint8 state, int x, int y);

private:
	Scrollbar::Type _type;

	bool _full;

	float _range;
	float _length;
	float _state;

	float _dragX;
	float _dragY;
	float _dragState;

	Scrollbar _scrollbar;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H

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

/** @file engines/nwn/gui/widgets/editbox.h
 *  A NWN editbox widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_EDITBOX_H
#define ENGINES_NWN_GUI_WIDGETS_EDITBOX_H

#include "engines/nwn/gui/widgets/modelwidget.h"

namespace Graphics {

namespace Aurora {
class Text;
}

}

namespace Common {
class UString;
}

namespace Engines {

class GUI;

namespace NWN {

class WidgetButton;
class WidgetScrollbar;

/** A NWN editbox widget. */
class WidgetEditBox : public ModelWidget {
public:
	WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
	              const Common::UString &model, const Common::UString &font);
	~WidgetEditBox();

	void show();
	void hide();

	void mouseDown(uint8 state, float x, float y);
	void mouseWheel(uint8 state, int x, int y);

	void getProperties();

	/** Set title text if available. */
	void setTitle(const Common::UString &title);
	/** Set main text. */
	void setText(const Common::UString &text);

private:
	void subActive(Widget &widget);

	void createScrollbar();
	void updateScrollbarLength();
	void updateScrollbarPosition();

	/** 
	 * Scroll the main text if it is bigger than the editBox.
	 *  @param n             scroll up if negative, down otherwise
	 */
	void scroll(int n);

	Common::UString _font;

	//FIXME As there is no mechanism to easily scroll text (like the render-to-texture way).
	//      Lines are put on bare Graphics::Aurora::Text but should ideally use WidgetText.
	/** Show/Hide lines according to _firstLine variable. */
	void updateLines();

	Graphics::Aurora::Text *_title;
	std::vector<Graphics::Aurora::Text *> _text;
	float pX, pY, pZ; ///< Text position.
	uint _firstLine;
	uint _linesToShow;

	WidgetButton *_up;
	WidgetButton *_down;
	WidgetScrollbar *_scrollbar;

	bool _hasScrollbar;
	bool _hasTitle;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_EDITBOX_H

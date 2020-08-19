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
 *  A NWN button widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_BUTTON_H
#define ENGINES_NWN_GUI_WIDGETS_BUTTON_H

#include "src/common/ustring.h"

#include "src/engines/nwn/gui/widgets/modelwidget.h"

namespace Engines {

class GUI;

namespace NWN {

/** A NWN button widget. */
class WidgetButton : public ModelWidget {
public:
	enum Mode {
		kModeNormal     = 0,
		kModeToggle,
		kModeUnchanged
	};

	WidgetButton(::Engines::GUI &gui, const Common::UString &tag,
	             const Common::UString &model, const Common::UString &sound = "gui_button");
	~WidgetButton();

	void enter();
	void leave();

	void setPosition(float x, float y, float z);

	void setMode(Mode mode);
	Mode getMode() const;
	void setPressed(bool pushed);
	bool isPressed() const;
	void setDisabled(bool disabled);

	void mouseDown (uint8_t state, float x, float y);
	void mouseUp   (uint8_t state, float x, float y);
	void mouseWheel(uint8_t state, int x, int y);

private:
	Common::UString _sound;
	Mode _mode;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_BUTTON_H

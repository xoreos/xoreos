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
 *  A NWN checkbox widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_CHECKBOX_H
#define ENGINES_NWN_GUI_WIDGETS_CHECKBOX_H

#include "src/engines/nwn/gui/widgets/modelwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

class GUI;

namespace NWN {

/** A NWN checkbox widget. */
class WidgetCheckBox : public ModelWidget {
public:
	WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &model);
	~WidgetCheckBox();

	bool getState() const;
	void setState(bool state);

	void enter();
	void leave();

	void mouseDown(uint8_t state, float x, float y);
	void mouseUp  (uint8_t state, float x, float y);

protected:
	void signalGroupMemberActive();

private:
	bool _state;
	bool _down;

	void updateModel(bool highlight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_CHECKBOX_H

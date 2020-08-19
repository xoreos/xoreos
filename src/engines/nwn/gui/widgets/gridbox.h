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
 *  A NWN gridbox widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_GRIDBOX_H
#define ENGINES_NWN_GUI_WIDGETS_GRIDBOX_H

#include "src/engines/nwn/gui/widgets/listbox.h"

namespace Common {
class UString;
}

namespace Engines {

class GUI;

namespace NWN {

class WidgetGridBox : public WidgetListBox {
public:
	WidgetGridBox(::Engines::GUI &gui, const Common::UString &tag, const Common::UString &model,
	              float innerHSpace = 0.0f, float InnerVSpace = 0.0f);
	~WidgetGridBox();

	void mouseDown(uint8_t state, float x, float y);
	void unlock();
	void subActive(Widget &widget);

protected:
	void updateScrollbarLength();
	void updateVisible();

	void scrollUp(size_t n);
	void scrollDown(size_t n);

	size_t _itemsByRow;

	float _innerHSpace;
	float _innerVSpace;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_GRIDBOX_H

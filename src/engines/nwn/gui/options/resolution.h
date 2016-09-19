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
 *  The NWN resolution options menu.
 */

#ifndef ENGINES_NWN_GUI_OPTIONS_RESOLUTION_H
#define ENGINES_NWN_GUI_OPTIONS_RESOLUTION_H

#include <vector>

#include "src/graphics/resolution.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** The NWN resolution options menu. */
class OptionsResolutionMenu : public GUI {
public:
	OptionsResolutionMenu(bool isMain = false, ::Engines::Console *console = 0);
	~OptionsResolutionMenu();

	void show();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);

	void initWidget(Widget &widget);
	void callbackActive(Widget &widget);

private:
	std::vector<Graphics::Resolution> _useableResolutions;

	int _width;
	int _height;

	void initResolutionsBox(WidgetListBox &resList);

	void setResolution(size_t n);

	void adoptChanges();
	void revertChanges();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_OPTIONS_RESOLUTION_H

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
 *  The new premium module menu.
 */

#ifndef ENGINES_NWN_GUI_MAIN_NEWPREMIUM_H
#define ENGINES_NWN_GUI_MAIN_NEWPREMIUM_H

#include <vector>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/gui/widgets/listbox.h"

#include "src/engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

class Module;

class WidgetListItemPremium : public WidgetListItem {
public:
	WidgetListItemPremium(::Engines::GUI &gui, const Common::UString &font,
	                      const Common::UString &text, float spacing = 0.0f);
	~WidgetListItemPremium();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

protected:
	bool activate();
	bool deactivate();

private:
	Common::ScopedPtr<Graphics::Aurora::Model> _button;
	Common::ScopedPtr<Graphics::Aurora::Text>  _text;

	float _spacing;
};

/** The NWN new premium module menu. */
class NewPremiumMenu : public GUI {
public:
	NewPremiumMenu(Module &module, GUI &charType, ::Engines::Console *console = 0);
	~NewPremiumMenu();

	void show();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);
	void initWidget(Widget &widget);

	void callbackActive(Widget &widget);

private:
	Module *_module;

	std::vector<Common::UString> _modules;

	GUI *_charType;

	Common::UString getSelectedModule();

	void initModuleList();
	void selectedModule();
	void loadModule();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_MAIN_NEWPREMIUM_H

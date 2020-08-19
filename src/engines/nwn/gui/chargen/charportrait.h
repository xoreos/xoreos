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
 *  The portrait chooser in CharGen.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARPORTRAIT_H
#define ENGINES_NWN_GUI_CHARGEN_CHARPORTRAIT_H

#include <vector>

#include "src/engines/nwn/gui/chargen/chargenbase.h"
#include "src/engines/nwn/gui/widgets/gridbox.h"

namespace Engines {

namespace NWN {

class PortraitWidget;

class WidgetGridItemPortrait : public WidgetListItem {
public:
	WidgetGridItemPortrait(GUI &gui, const Common::UString &portrait);
	~WidgetGridItemPortrait();

	float getHeight() const;
	float getWidth() const;

	void mouseDown(uint8_t state, float x, float y);

	bool activate();
	bool deactivate();

private:
	PortraitWidget *_portraitWidget;
	Common::UString _portrait;
};

class CharPortrait : public CharGenBase {
public:
	CharPortrait(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharPortrait();

	void reset();
	void show();

	void setMainTexture(const Common::UString &texture);

private:
	void callbackActive(Widget &widget);
	const std::vector<Common::UString> initPortraitList();
	void buildPortraitBox();

	WidgetGridBox  *_portraitsBox;
	Common::UString _selectedPortrait;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARPORTRAIT_H

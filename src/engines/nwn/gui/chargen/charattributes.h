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
 *  The attributes chooser in CharGen.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARATTRIBUTES_H
#define ENGINES_NWN_GUI_CHARGEN_CHARATTRIBUTES_H

#include <vector>
#include <memory>

#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class ButtonsGroup;

class CharAttributes : public CharGenBase {
public:
	CharAttributes(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharAttributes();

	void reset();
	void show();
	void hide();

	void genTextAttributes(size_t attribute);
	uint8_t pointCost(uint8_t attrValue);
	void updateText(uint8_t attribute);

	void setRecommend();

protected:
	void callbackActive(Widget &widget);

private:
	void init();
	void initButtonsGroup();

	std::vector<uint8_t> _attributes;
	std::vector<uint8_t> _attrAdjust;
	std::vector<WidgetLabel *> _labelAttributes;

	std::unique_ptr<ButtonsGroup> _attrButtons;

	uint8_t _pointLeft;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARATTRIBUTES_H

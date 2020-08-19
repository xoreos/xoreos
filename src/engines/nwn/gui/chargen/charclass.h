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
 *  The class chooser in CharGen.
 */

#ifndef  ENGINES_NWN_GUI_CHARGEN_CHARCLASS_H
#define  ENGINES_NWN_GUI_CHARGEN_CHARCLASS_H

#include <vector>

#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class CharClass : public CharGenBase {
public:
	CharClass(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharClass();

	void reset();
	void hide();

protected:
	void fixWidgetType(const Common::UString &tag, WidgetType &type);
	void createClassList();

	void callbackActive(Widget &widget);

private:
	WidgetListBox *_classesListBox;
	std::vector<Common::UString> _classNames;
	std::vector<Common::UString> _helpTexts;
	std::vector<uint32_t> _classesId;

	size_t _firstPrestigeClass;
};

} // End of namespace NWN

} // End of namespace Engines

#endif //  ENGINES_NWN_GUI_CHARGEN_CHARCLASS_H

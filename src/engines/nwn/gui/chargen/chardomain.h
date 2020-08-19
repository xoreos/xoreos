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
 *  The domain chooser for clerc in CharGen.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARDOMAIN_H
#define ENGINES_NWN_GUI_CHARGEN_CHARDOMAIN_H

#include <vector>

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class WidgetListBox;

class WidgetListItemDomain : public WidgetListItemButton {
public:
	WidgetListItemDomain(::Engines::GUI &gui, const Common::UString &title, const Common::UString &icon, const Common::UString &desc, uint8_t domainId);
	~WidgetListItemDomain();

	void mouseUp(uint8_t state, float x, float y);
	void mouseDblClick(uint8_t state, float x, float y);

	void subActive(Engines::Widget &widget);

private:
	Common::UString _name;
	Common::UString _description;
	uint8_t _domainId;

	friend class CharDomain;
};

class CharDomain : public CharGenBase {
public:
	CharDomain(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharDomain();

	void reset();
	void fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type);

	void moveDomain(WidgetListItemDomain *item);

private:
	void callbackActive(Widget &widget);
	void createDomainList();
	void updateChosenDomains();
	void setRecommendedDomains();

	WidgetListBox *_domainListBox;
	std::vector<WidgetListItemDomain *> _chosenDomains;

	friend class WidgetListItemDomain;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARDOMAIN_H

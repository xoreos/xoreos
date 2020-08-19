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
 *  The spells selection GUI in CharGen.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARSPELLS_H
#define ENGINES_NWN_GUI_CHARGEN_CHARSPELLS_H

#include <vector>
#include <memory>

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class CharHelp;
class PortraitWidget;

struct Spell {
	uint16_t spellID;
	Common::UString name;
	Common::UString icon;
	Common::UString desc;
};

class WidgetListItemSpell: public WidgetListItemButton {
public:
	WidgetListItemSpell(::Engines::GUI &gui, Spell spell, bool isRight = true);
	~WidgetListItemSpell();

private:
	void subActive(Widget &widget);
	void callbackHelp();

	Spell _spell;

	friend class CharSpells;
};

class CharSpells : public CharGenBase {
public:
	CharSpells(CharGenChoices &choices, Engines::Console *console);
	~CharSpells();

	void reset();
	void fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type);

	void showSpellLevel(size_t spellLevel, bool forceUpdate = false);
	void moveSpell(WidgetListItemSpell *spellItem);
	void showSpellHelp(Spell &spell);

private:
	void callbackActive(Widget &widget);

	void makeSpellsList();
	void computeRemainingSpells(const Aurora::TwoDARow &classRow);
	void updateRemainLabel();
	void setRecommendedSpells();

	std::unique_ptr<CharHelp> _spellHelp;

	WidgetListBox  *_availListBox;
	WidgetListBox  *_knownListBox;

	std::vector<std::vector<Spell> > _availSpells;
	std::vector<std::vector<Spell> > _knownSpells;

	std::vector<uint8_t> _remainingSpells;

	size_t _currentSpellLevel;
	size_t _maxLevel;
	size_t _abilityLimit;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARSPELLS_H

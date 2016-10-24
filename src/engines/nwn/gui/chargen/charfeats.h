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
 *  The feats selection GUI.
 */

#ifndef ENGINES_NWN_GUI_CHARGEN_CHARFEATS_H
#define ENGINES_NWN_GUI_CHARGEN_CHARFEATS_H

#include <vector>
#include <list>
#include <map>

#include "src/common/scopedptr.h"

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class CharHelp;
class CharFeatsPopup;
class WidgetListBox;
class WidgetButton;

class WidgetListItemFeat : public WidgetListItemButton {
public:
	WidgetListItemFeat (::Engines::GUI &gui, FeatItem &feat);
	~WidgetListItemFeat();

	void callbackHelp();

private:
	void subActive(Widget &widget);
	FeatItem _feat;

	friend class CharFeats;
};

typedef std::map<FeatItem, std::vector<FeatItem> > MasterFeatsMap;

class CharFeats : public CharGenBase {
public:
	CharFeats(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharFeats();

	void reset();
	void fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type);

	void showFeatHelp(FeatItem &feat);
	void moveFeat(FeatItem feat, bool toKnownFeats, bool rebuild = true);
	void moveFeat(WidgetListItemFeat *item);

private:
	void callbackActive(Widget &widget);
	void callbackRun();

	void makeAvailList(uint8 list);
	void makeKnownList();

	void changeAvailFeats(int8 diff, bool normalFeat, bool rebuild = true);

	void setRecommendedFeats();

	Common::ScopedPtr<CharHelp>       _featHelp;
	Common::ScopedPtr<CharFeatsPopup> _featsPopup;

	WidgetListBox  *_availListBox;
	WidgetListBox  *_knownListBox;

	std::list<FeatItem> _availFeats;
	std::list<FeatItem> _knownFeats;

	std::vector<WidgetListItemFeat *> _featsTrash;

	uint8 _normalFeats;
	uint8 _bonusFeats;
	bool _hasBonusFeats;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARFEATS_H

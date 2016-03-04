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

#include "src/engines/nwn/gui/widgets/listitembutton.h"
#include "src/engines/nwn/gui/chargen/chargenbase.h"

namespace Engines {

namespace NWN {

class CharFeatHelp;
class CharFeatsPopup;
class WidgetListBox;
class WidgetButton;

struct Feat {
	uint32 featId;
	Common::UString name;
	Common::UString description;
	Common::UString icon;
	bool isMasterFeat;
	uint32 masterFeat;

	bool operator < (const Feat& f) const {
		return (name < f.name);
	}
};

class WidgetListItemFeat : public WidgetListItemButton {
public:
	WidgetListItemFeat (::Engines::GUI &gui, Feat &feat);
	~WidgetListItemFeat();

	void callbackHelp();

private:
	void subActive(Widget &widget);
	Feat _feat;

	friend class CharFeats;
};

typedef std::map<Feat, std::vector<Feat> > MasterFeatsMap;

class CharFeats : public CharGenBase {
public:
	CharFeats(CharGenChoices &choices, ::Engines::Console *console = 0);
	~CharFeats();

	void reset();
	void fixWidgetType(const Common::UString &tag, NWN::GUI::WidgetType &type);

	void showFeatHelp(Feat &feat);
	void moveFeat(WidgetListItemFeat *item);

private:
	void callbackActive(Widget &widget);
	void callbackRun();

	void makeAvailList();
	void makeKnownList();

	void changeAvailFeats(int8 diff);

	void addToMasterFeats(Feat &feat, WidgetListItemFeat *featItem);
	void removeFromMasterFeats(Feat &feat, WidgetListItemFeat *masterFeatItem);
	MasterFeatsMap::iterator findMasterFeat(Feat &feat);

	CharFeatHelp   *_featHelp;
	CharFeatsPopup *_featsPopup;
	WidgetListBox  *_availListBox;
	WidgetListBox  *_knownListBox;

	MasterFeatsMap _masterFeats;

	std::vector<WidgetListItemFeat *> _featsTrash;

	uint8 _normalFeats;
	uint8 _bonusFeats;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_CHARGEN_CHARFEATS_H

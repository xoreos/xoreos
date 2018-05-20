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
 *  A KotOR listbox widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H
#define ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetListBox : public KotORWidget {
public:
	WidgetListBox(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetListBox();

	void load(const Aurora::GFF3Struct &gff);
	void setItemSelectionEnabled(bool value);
	void setAdjustHeight(bool value);

	/** Create item widgets and return pointers to them.
	 *
	 *  @param count number of items to add or 0 to add maximum number of
	 *               items that fit into the listbox
	 */
	const std::vector<KotORWidget *> &createItemWidgets(uint count = 0);

	void addItem(const Common::UString &text);
	void removeAllItems();
	void refreshItemWidgets();

	KotORWidget *createItem(Common::UString name);

	void onClickItemWidget(const Common::UString &tag);
	void selectNextItem();
	void selectPreviousItem();

	int getSelectedIndex() const;

private:
	const Aurora::GFF3Struct *_protoItem;
	const Aurora::GFF3Struct *_scrollBar;
	std::vector<Common::UString> _items;
	std::vector<KotORWidget *> _itemWidgets;
	int _padding;
	bool _leftScrollBar;
	bool _itemSelectionEnabled;
	bool _adjustHeight;
	int _selectedIndex;
	int _startIndex;
	int _numVisibleItems;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

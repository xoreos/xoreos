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
 *  Listbox widget for Star Wars: Knights of the Old Republic.
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
	WidgetScrollbar *createScrollBar();

	/** Toggle item selection mode. When enabled listbox items can be
	 *  selected by clicking on them. One can also scroll through the
	 *  list of items (see selectNextItem and selectPreviousItem).
	 */
	void setItemSelectionEnabled(bool value);

	/** Toggle height adjustment mode. When enabled item widgets will
	 *  adjust their height during refresh based on the size of their
	 *  contents.
	 */
	void setAdjustHeight(bool value);

	/** Toggle scroll bar visibility mode. When enabled it will only
	 *  be displayed when number of underlying items exceeds number
	 *  of item widgets.
	 */
	void setHideScrollBar(bool value);

	// .--- Item widgets management

	/** Create item widgets and return pointers to them.
	 *
	 *  @param count Number of widgets to create or 0 to create a
	 *               maximum number of widgets that could fit into the
	 *               listbox
	 */
	const std::vector<KotORWidget *> &createItemWidgets(uint count = 0);

	KotORWidget *createItem(Common::UString name);
	void refreshItemWidgets();

	// '---

	// .--- Underlying items management

	void addItem(const Common::UString &text,
	             const Common::UString &icon = "",
	             int count = 1);

	void removeAllItems();

	// '---

	// .--- Event handlers

	void onClickItemWidget(const Common::UString &tag);
	void selectNextItem();
	void selectPreviousItem();

	// '---

	int getSelectedIndex() const;

private:
	struct Item {
		Common::UString text;
		Common::UString icon;
		int count;
	};

	const Aurora::GFF3Struct *_protoItem;
	const Aurora::GFF3Struct *_scrollBar;
	std::vector<Item> _items;
	WidgetScrollbar *_scrollBarWidget;
	std::vector<KotORWidget *> _itemWidgets;
	int _padding;
	bool _leftScrollBar;
	bool _itemSelectionEnabled;
	bool _adjustHeight;
	int _selectedIndex;
	int _startIndex;
	int _numVisibleItems;
	bool _hideScrollBar;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

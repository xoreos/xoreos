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
 *  A list box widget for Star Wars: Knights of the Old Republic and Jade Empire.
 */

#ifndef ENGINES_AURORA_KOTORJADEGUI_LISTBOX_H
#define ENGINES_AURORA_KOTORJADEGUI_LISTBOX_H

#include "src/engines/aurora/kotorjadegui/kotorjadewidget.h"

namespace Engines {

class WidgetScrollbar;
class WidgetProtoItem;

enum ListBoxItemType {
	kLBItemTypeDefault        = 0,
	kLBItemTypeKotORInventory = 1
};

class WidgetListBox : public KotORJadeWidget {
public:
	WidgetListBox(GUI &gui, const Common::UString &tag);
	~WidgetListBox();

	void load(const Aurora::GFF3Struct &gff);

	void setItemType(ListBoxItemType itemType);

	/** Toggle item selection mode. When enabled list box items can be
	 *  selected by clicking on them.
	 */
	void setItemSelectionEnabled(bool itemSelectionEnabled);

	/** Toggle height adjustment mode. When enabled item widgets will
	 *  adjust their height during refresh based on the size of their
	 *  contents.
	 */
	void setAdjustHeight(bool adjustHeight);

	/** Toggle scroll bar visibility mode. When enabled it will only
	 *  be displayed when number of underlying items exceeds number
	 *  of item widgets.
	 */
	void setHideScrollbar(bool hideScrollbar);

	void setPadding(uint32 padding);
	void setItemTextColor(float r, float g, float b, float a);
	void setItemBorderColor(float r, float g, float b, float a);

	// .--- Item widgets

	void createItemWidgets(uint32 count);
	void refreshItemWidgets();

	// '---

	// .--- Underlying items

	void addItem(const Common::UString &contents);
	void removeAllItems();

	// '---

	// .--- Item selection

	void selectItemByWidgetTag(const Common::UString &tag);
	void selectNextItem();
	void selectPreviousItem();

	int getSelectedIndex() const;

	// '---

	void setHeight(float height);

	void subActive(Widget &widget);

private:
	const Aurora::GFF3Struct *_protoItem;
	WidgetScrollbar *_scrollbar;
	std::vector<WidgetProtoItem *> _itemWidgets;
	std::vector<Common::UString> _items;

	ListBoxItemType _itemType;
	uint32 _padding;
	bool _leftScrollbar;
	bool _itemSelectionEnabled;
	bool _adjustHeight;
	bool _hideScrollbar;
	int _selectedIndex;
	int _startIndex;
	int _numVisibleItems;

	bool _textColorChanged;
	float _textR, _textG, _textB, _textA;

	bool _borderColorChanged;
	float _borderR, _borderG, _borderB, _borderA;

	void createScrollbar(const Aurora::GFF3Struct &gff);
	void positionItemWidgets();
};

} // End of namespace Engines

#endif // ENGINES_AURORA_KOTORJADEGUI_LISTBOX_H

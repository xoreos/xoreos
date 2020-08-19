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
 *  List box widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_LISTBOX_H
#define ENGINES_ODYSSEY_LISTBOX_H

#include <functional>

#include "src/engines/odyssey/widget.h"

namespace Engines {

namespace Odyssey {

class WidgetProtoItem;
class WidgetScrollbar;

typedef std::function<WidgetProtoItem *(GUI &gui, const Common::UString &tag)> ItemWidgetFactoryFunc;

class WidgetListBox : public Widget {
public:
	WidgetListBox(GUI &gui, const Common::UString &tag);

	void load(const Aurora::GFF3Struct &gff);

	// Basic properties

	/**
	 * Toggle item selection mode. When enabled, list box items can be
	 * selected by clicking on them.
	 */
	void setItemSelectionEnabled(bool itemSelectionEnabled);

	/**
	 * Toggle height adjustment mode. When enabled, item widgets will
	 * adjust their height during refresh based on the size of their
	 * contents.
	 */
	void setAdjustHeight(bool adjustHeight);

	/**
	 * Toggle scroll bar visibility mode. When enabled, it will only
	 * be displayed when the number of items exceeds the number of
	 * item widgets.
	 */
	void setHideScrollbar(bool hideScrollbar);

	// Items

	/** Get index of the selected item. */
	int getSelectedIndex() const;

	/** Add an item to the list box by it's contents. */
	void addItem(const Common::UString &contents);
	/** Remove all items from the list box. */
	void removeAllItems();

	/** Select a list box item by the widget tag. */
	void selectItemByWidgetTag(const Common::UString &tag);
	/** Select a list box item by index. */
	void selectItemByIndex(int index);
	/** Select the next list box item. */
	void selectNextItem();
	/** Select the previous list box item. */
	void selectPreviousItem();

	// Item widgets

	/** Set the function to call when creating a list box item widget. */
	void setItemWidgetFactoryFunction(const ItemWidgetFactoryFunc &f);

	/** Create the specified number of list box item widgets. */
	void createItemWidgets(uint32_t count);
	/** Refresh list box items widgets. */
	void refreshItemWidgets();

	// Color

	/** Set the text color of the list box items. */
	void setItemTextColor(float r, float g, float b, float a);
	/** Set the border color of the list box items. */
	void setItemBorderColor(float r, float g, float b, float a);

	// Positioning

	void setHeight(float height);
	void setPadding(uint32_t padding);

	// Widget/cursor interactions

	void subActive(Engines::Widget &widget);
	void mouseWheel(uint8_t state, int x, int y);

	// Sound

	/** Set the sound to play when the list box item is selected. */
	void setSoundSelectItem(const Common::UString &resRef);
	/** Set the sound to play when the list box item is hovered over. */
	void setSoundHoverItem(const Common::UString &resRef);
	/** Set the sound to play when the list box item is clicked. */
	void setSoundClickItem(const Common::UString &resRef);

private:
	const Aurora::GFF3Struct *_protoItem;
	WidgetScrollbar *_scrollbar;
	std::vector<WidgetProtoItem *> _itemWidgets;
	std::vector<Common::UString> _items;
	ItemWidgetFactoryFunc _itemWidgetFactoryFunc;

	uint32_t _padding;
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

	Common::UString _soundSelectItem;
	Common::UString _soundHoverItem;
	Common::UString _soundClickItem;

	WidgetProtoItem *createItemWidget(const Common::UString &tag);
	void createScrollbar(const Aurora::GFF3Struct &gff);
	void positionItemWidgets();
	void applyChangesToItemWidgets();
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_LISTBOX_H

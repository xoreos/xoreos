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

#include "src/common/strutil.h"

#include "src/aurora/gff3file.h"

#include "src/graphics/graphics.h"

#include "src/engines/kotor/gui/gui.h"

#include "src/engines/kotor/gui/widgets/listbox.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/label.h"
#include "src/engines/kotor/gui/widgets/scrollbar.h"
#include "src/engines/kotor/gui/widgets/listitem.h"

namespace Engines {

namespace KotOR {

WidgetListBox::WidgetListBox(::Engines::GUI &gui,
                             const Common::UString &tag)
		: KotORWidget(gui, tag),
		  _protoItem(0),
		  _scrollBar(0),
		  _padding(0),
		  _leftScrollBar(false),
		  _itemSelectionEnabled(false),
		  _adjustHeight(false),
		  _selectedIndex(-1),
		  _startIndex(0),
		  _numVisibleItems(0),
		  _hideScrollBar(false) {
}

WidgetListBox::~WidgetListBox() {
}

void WidgetListBox::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);

	if (gff.hasField("PROTOITEM")) {
		_protoItem = &gff.getStruct("PROTOITEM");
	}

	if (gff.hasField("LEFTSCROLLBAR")) {
		_leftScrollBar = gff.getBool("LEFTSCROLLBAR");
	}

	if (gff.hasField("SCROLLBAR")) {
		_scrollBar = &gff.getStruct("SCROLLBAR");
	}

	if (gff.hasField("PADDING")) {
		_padding = gff.getSint("PADDING");

		if (_gui->getName().stricmp("container") == 0)
			_padding = 18;
	}
}

WidgetScrollbar *WidgetListBox::createScrollBar() {
	if (!_scrollBar)
		return 0;

	_scrollBarWidget = new WidgetScrollbar(*_gui, _scrollBar->getString("TAG"));
	_scrollBarWidget->load(*_scrollBar);

	float x, y, z;
	getPosition(x, y, z);
	_scrollBarWidget->setPosition(x, y, z);

	_scrollBarWidget->setSize(_scrollBarWidget->getWidth(), _height);
	return _scrollBarWidget;
}

void WidgetListBox::setItemSelectionEnabled(bool value) {
	_itemSelectionEnabled = value;
}

void WidgetListBox::setAdjustHeight(bool value) {
	_adjustHeight = value;
}

void WidgetListBox::setHideScrollBar(bool value) {
	_hideScrollBar = value;
}

const std::vector<KotORWidget *> &WidgetListBox::createItemWidgets(uint count) {
	if (!_protoItem)
		throw Common::Exception("ListBox widget has no PROTOITEM");

	const int64 itemHeight = _protoItem->getStruct("EXTENT").getSint("HEIGHT");

	uint widgetCount = count;
	if ((count == 0) && (itemHeight > 0))
		widgetCount = getHeight() / itemHeight;

	for (uint i = 0; i < widgetCount; ++i) {
		Common::UString name = Common::UString::format("%s_ITEM_%u", _tag.c_str(), i);
		createItem(name);
	}

	return _itemWidgets;
}

void WidgetListBox::addItem(const Common::UString &text,
                            const Common::UString &icon,
                            int count) {
	Item item;
	item.text = text;
	item.icon = icon;
	item.count = count;
	_items.push_back(item);
}

void WidgetListBox::removeAllItems() {
	_startIndex = 0;
	_items.clear();
}

void WidgetListBox::refreshItemWidgets() {
	_numVisibleItems = 0;
	float totalHeight = 0;

	float x, y, z;
	getPosition(x, y, z);
	y += getHeight();

	if (_scrollBar && _leftScrollBar)
		x += _scrollBar->getStruct("BORDER").getSint("DIMENSION") +
		     _scrollBar->getStruct("EXTENT").getSint("WIDTH");

	GfxMan.lockFrame();

	bool heightExceeded = false;

	for (size_t i = 0; i < _itemWidgets.size(); ++i) {
		KotORWidget *itemWidget = _itemWidgets[i];
		bool visible = false;

		if (!heightExceeded) {
			int itemIndex = _startIndex + i;
			if (itemIndex < (int)_items.size()) { // have item to display?
				Item &item = _items[itemIndex];
				const Common::UString &text = item.text;

				if (_adjustHeight) {
					float textHeight = itemWidget->getTextHeight(text);
					if (totalHeight + textHeight > getHeight())
						heightExceeded = true;
					else {
						itemWidget->setSize(itemWidget->getWidth(), textHeight);
						itemWidget->setPosition(x, y -= textHeight + _padding, z);
						totalHeight += textHeight;
					}
				}

				if (!heightExceeded) {
					itemWidget->setText(text);
					itemWidget->setIconTexture(item.icon);
					itemWidget->setCount(item.count);
					itemWidget->setHighlight(itemIndex == _selectedIndex);
					visible = true;
				}
			}
		}

		if (visible) {
			itemWidget->setInvisible(false);
			if (isVisible())
				itemWidget->show();
			++_numVisibleItems;
		} else {
			if (isVisible())
				itemWidget->hide();
			itemWidget->setInvisible(true);
		}
	}

	if (_hideScrollBar) {
		if (_numVisibleItems < (int)_items.size()) {
			_scrollBarWidget->setInvisible(false);
			if (isVisible())
				_scrollBarWidget->show();
		} else {
			if (isVisible())
				_scrollBarWidget->hide();
			_scrollBarWidget->setInvisible(true);
		}
	}

	GfxMan.unlockFrame();
}

KotORWidget *WidgetListBox::createItem(Common::UString name) {
	if (!_protoItem)
		throw Common::Exception("ListBox widget has no PROTOITEM");

	KotORWidget *item;

	// Create a new widget.
	switch (_protoItem->getUint("CONTROLTYPE")) {
		case 4:
			item = new WidgetLabel(*_gui, name);
			break;
		case 5:
			item = new WidgetListItem(*_gui, name);
			break;
		case 6:
			item = new WidgetButton(*_gui, name);
			if (_itemSelectionEnabled)
				static_cast<WidgetButton *>(item)->setDisableHighlight(true);
			break;
		default:
			throw Common::Exception("TODO: Add other supported widget types to the ListBox");
	}

	// Load the prototype item.
	item->load(*_protoItem);

	// Calculate the new position for managing it in a list.
	float x, y, z;
	getPosition(x, y, z);

	if (_scrollBar && _leftScrollBar)
		x += _scrollBar->getStruct("BORDER").getSint("DIMENSION") +
		     _scrollBar->getStruct("EXTENT").getSint("WIDTH");

	assert(getHeight() > 0);

	y = y - _itemWidgets.size() * (item->getHeight() + _padding) + getHeight() - item->getHeight();

	item->setPosition(x, y, z);
	_itemWidgets.push_back(item);

	return item;
}

void WidgetListBox::onClickItemWidget(const Common::UString &tag) {
	if (!_itemSelectionEnabled)
		return;

	Common::UString tmp(tag);
	tmp.replaceAll(_tag + "_ITEM_", "");

	int index = -1;
	Common::parseString(tmp, index);

	if ((index >= 0) && (_selectedIndex != _startIndex + index)) {
		_selectedIndex = _startIndex + index;
		refreshItemWidgets();
	}
}

void WidgetListBox::selectNextItem() {
	if (!_itemSelectionEnabled)
		return;

	if (_selectedIndex < 0 && !_items.empty()) {
		_selectedIndex = 0;
		refreshItemWidgets();
	} else if (_selectedIndex < (int)_items.size() - 1) {
		++_selectedIndex;
		if (_selectedIndex - _startIndex >= _numVisibleItems)
			_startIndex = _selectedIndex - _numVisibleItems + 1;
		refreshItemWidgets();
	}
}

void WidgetListBox::selectPreviousItem() {
	if (!_itemSelectionEnabled)
		return;

	if (_selectedIndex < 0 && !_items.empty()) {
		_selectedIndex = 0;
		refreshItemWidgets();
	} else if (_selectedIndex > 0) {
		--_selectedIndex;
		if (_selectedIndex < _startIndex)
			_startIndex = _selectedIndex;
		refreshItemWidgets();
	}
}

int WidgetListBox::getSelectedIndex() const {
	return _selectedIndex;
}

} // End of namespace KotOR

} // End of namespace Engines

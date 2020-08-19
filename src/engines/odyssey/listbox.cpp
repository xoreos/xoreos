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

#include "src/common/util.h"
#include "src/common/strutil.h"

#include "src/aurora/gff3file.h"

#include "src/graphics/graphics.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"

#include "src/engines/odyssey/listbox.h"
#include "src/engines/odyssey/protoitem.h"
#include "src/engines/odyssey/scrollbar.h"

namespace Engines {

namespace Odyssey {

WidgetListBox::WidgetListBox(GUI &gui, const Common::UString &tag) :
		Widget(gui, tag),
		_protoItem(0),
		_scrollbar(0),
		_itemWidgetFactoryFunc(0),
		_padding(0),
		_leftScrollbar(false),
		_itemSelectionEnabled(false),
		_adjustHeight(false),
		_hideScrollbar(true),
		_selectedIndex(-1),
		_startIndex(0),
		_numVisibleItems(0),
		_textColorChanged(false),
		_textR(0.0f), _textG(0.0f), _textB(0.0f), _textA(0.0f),
		_borderColorChanged(false),
		_borderR(0.0f), _borderG(0.0f), _borderB(0.0f), _borderA(0.0f) {
}

void WidgetListBox::load(const Aurora::GFF3Struct &gff) {
	Widget::load(gff);

	_padding = gff.getSint("PADDING");
	_leftScrollbar = gff.getBool("LEFTSCROLLBAR");

	if (gff.hasField("SCROLLBAR"))
		createScrollbar(gff.getStruct("SCROLLBAR"));

	if (gff.hasField("PROTOITEM"))
		_protoItem = &gff.getStruct("PROTOITEM");
}

void WidgetListBox::setItemSelectionEnabled(bool itemSelectionEnabled) {
	_itemSelectionEnabled = itemSelectionEnabled;
	if (!_itemSelectionEnabled)
		_selectedIndex = -1;

	applyChangesToItemWidgets();
}

void WidgetListBox::setAdjustHeight(bool adjustHeight) {
	_adjustHeight = adjustHeight;
}

void WidgetListBox::setHideScrollbar(bool hideScrollbar) {
	_hideScrollbar = hideScrollbar;
}

void WidgetListBox::setPadding(uint32_t padding) {
	_padding = padding;
}

void WidgetListBox::setItemTextColor(float r, float g, float b, float a) {
	_textColorChanged = true;
	_textR = r;
	_textG = g;
	_textB = b;
	_textA = a;

	applyChangesToItemWidgets();
}

void WidgetListBox::setItemBorderColor(float r, float g, float b, float a) {
	_borderColorChanged = true;
	_borderR = r;
	_borderG = g;
	_borderB = b;
	_borderA = a;

	applyChangesToItemWidgets();
}

void WidgetListBox::addItem(const Common::UString &contents) {
	_items.push_back(contents);
}

void WidgetListBox::removeAllItems() {
	_startIndex = 0;
	_items.clear();
}

void WidgetListBox::createItemWidgets(uint32_t count) {
	if ((!_protoItem) || (!_itemWidgets.empty()))
		return;

	for (uint32_t i = 0; i < count; ++i) {
		Common::UString tag = Common::UString::format("%s_ITEM_%u", _tag.c_str(), i);
		WidgetProtoItem *item = createItemWidget(tag);
		item->load(*_protoItem);

		addChild(*item);
		addSub(*item);

		_itemWidgets.push_back(item);
	}

	positionItemWidgets();
	applyChangesToItemWidgets();
}

void WidgetListBox::refreshItemWidgets() {
	if (_itemWidgets.empty())
		return;

	_numVisibleItems = 0;
	float totalHeight = 0;

	float x, y, z;
	getPosition(x, y, z);
	y += _height;

	GfxMan.lockFrame();

	bool heightExceeded = false;

	for (size_t i = 0; i < _itemWidgets.size(); ++i) {
		WidgetProtoItem *itemWidget = _itemWidgets[i];
		bool visible = false;

		if (!heightExceeded) {
			int itemIndex = _startIndex + i;
			if (itemIndex < (int)_items.size()) { // have item to display?
				Common::UString &contents = _items[itemIndex];

				if (_adjustHeight) {
					float textHeight = itemWidget->getTextHeight(contents);
					if (totalHeight + textHeight > getHeight())
						heightExceeded = true;
					else {
						float iX, iY, iZ;
						itemWidget->getPosition(iX, iY, iZ);
						itemWidget->setPosition(iX, y -= textHeight + _padding, iZ);
						itemWidget->setHeight(textHeight);
						totalHeight += textHeight + _padding;
					}
				}

				if (!heightExceeded) {
					itemWidget->setContents(contents);
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

	if (_hideScrollbar) {
		if (_numVisibleItems < (int)_items.size()) {
			_scrollbar->setInvisible(false);
			if (isVisible())
				_scrollbar->show();
		} else {
			if (isVisible())
				_scrollbar->hide();
			_scrollbar->setInvisible(true);
		}
	}

	GfxMan.unlockFrame();
}

void WidgetListBox::selectItemByWidgetTag(const Common::UString &tag) {
	if (!tag.beginsWith(_tag + "_ITEM_"))
		return;

	Common::UString tmp(tag);
	tmp.replaceAll(_tag + "_ITEM_", "");

	int index = -1;
	Common::parseString(tmp, index);

	if ((index >= 0) && (_selectedIndex != _startIndex + index)) {
		_selectedIndex = _startIndex + index;
		playSound(_soundSelectItem, Sound::kSoundTypeSFX);
		refreshItemWidgets();
	}
}

void WidgetListBox::selectItemByIndex(int index) {
	if ((index < 0) || (static_cast<size_t>(index) >= _items.size()))
		return;

	_selectedIndex = index;
}

void WidgetListBox::selectNextItem() {
	if (_itemSelectionEnabled) {
		bool selectionChanged = false;

		if ((_selectedIndex < 0) && (!_items.empty())) {
			_selectedIndex = 0;
			selectionChanged = true;
			refreshItemWidgets();
		} else if (_selectedIndex < (int)_items.size() - 1) {
			++_selectedIndex;
			if (_selectedIndex - _startIndex >= _numVisibleItems)
				_startIndex = _selectedIndex - _numVisibleItems + 1;
			selectionChanged = true;
			refreshItemWidgets();
		}

		if (selectionChanged)
			playSound(_soundSelectItem, Sound::kSoundTypeSFX);

	} else if (_startIndex + _numVisibleItems < (int)_items.size()) {
		++_startIndex;
		refreshItemWidgets();
	}
}

void WidgetListBox::selectPreviousItem() {
	if (_itemSelectionEnabled) {
		bool selectionChanged = false;

		if ((_selectedIndex < 0) && (!_items.empty())) {
			_selectedIndex = 0;
			selectionChanged = true;
			refreshItemWidgets();
		} else if (_selectedIndex > 0) {
			--_selectedIndex;
			if (_selectedIndex < _startIndex)
				_startIndex = _selectedIndex;
			selectionChanged = true;
			refreshItemWidgets();
		}

		if (selectionChanged)
			playSound(_soundSelectItem, Sound::kSoundTypeSFX);

	} else if (_startIndex > 0) {
		--_startIndex;
		refreshItemWidgets();
	}
}

int WidgetListBox::getSelectedIndex() const {
	return _selectedIndex;
}

void WidgetListBox::setItemWidgetFactoryFunction(const ItemWidgetFactoryFunc &f) {
	_itemWidgetFactoryFunc = f;
}

void WidgetListBox::setHeight(float height) {
	float deltaHeight = height - _height;

	Widget::setHeight(height);

	if (_scrollbar) {
		height = _scrollbar->getHeight();
		_scrollbar->setHeight(height + deltaHeight);
	}
}

void WidgetListBox::subActive(Engines::Widget &widget) {
	if (_itemSelectionEnabled)
		selectItemByWidgetTag(widget.getTag());
	else
		raiseCallbackActive(widget);
}

void WidgetListBox::setSoundSelectItem(const Common::UString &resRef) {
	_soundSelectItem = resRef;
	applyChangesToItemWidgets();
}

void WidgetListBox::setSoundHoverItem(const Common::UString &resRef) {
	_soundHoverItem = resRef;
	applyChangesToItemWidgets();
}

void WidgetListBox::setSoundClickItem(const Common::UString &resRef) {
	_soundClickItem = resRef;
	applyChangesToItemWidgets();
}

WidgetProtoItem *WidgetListBox::createItemWidget(const Common::UString &tag) {
	if (!_itemWidgetFactoryFunc)
		return new WidgetProtoItem(*_gui, tag, this);

	return _itemWidgetFactoryFunc(*_gui, tag);
}

void WidgetListBox::createScrollbar(const Aurora::GFF3Struct &gff) {
	_scrollbar = new WidgetScrollbar(*_gui, _tag + "#" + gff.getString("TAG"));
	_scrollbar->load(gff);

	float x, y, z;
	getPosition(x, y, z);

	if (_leftScrollbar)
		_scrollbar->setPosition(x, y, -1.0f);
	else
		_scrollbar->setPosition(x + _width - _scrollbar->getWidth(), y, -1.0f);

	_scrollbar->setHeight(_height);

	addChild(*_scrollbar);
	addSub(*_scrollbar);
}

void WidgetListBox::positionItemWidgets() {
	float x, y, z;
	getPosition(x, y, z);

	if (_scrollbar && _leftScrollbar)
		x += _scrollbar->getWidth() + _scrollbar->getBorderDimension();

	y += _height;
	z -= 1.0f;

	size_t count = _itemWidgets.size();
	for (size_t i = 0; i < count; ++i) {
		_itemWidgets[i]->setPosition(x, y -= _itemWidgets[i]->getHeight() + _padding, z);
	}
}

void WidgetListBox::applyChangesToItemWidgets() {
	for (size_t i = 0; i < _itemWidgets.size(); ++i) {
		_itemWidgets[i]->setDisableHighlight(_itemSelectionEnabled);

		if (_textColorChanged)
			_itemWidgets[i]->setTextColor(_textR, _textG, _textB, _textA);

		if (_borderColorChanged)
			_itemWidgets[i]->setBorderColor(_borderR, _borderG, _borderB, _borderA);

		_itemWidgets[i]->setSoundHover(_soundHoverItem);
		_itemWidgets[i]->setSoundClick(_soundClickItem);
	}
}

void WidgetListBox::mouseWheel(uint8_t UNUSED(state), int UNUSED(x), int y) {
	if ((y == 0) || !_adjustHeight)
		return;

	_startIndex = MIN(
		MAX(_startIndex - y, 0),
		MAX<int>(_itemWidgets.size() - _numVisibleItems, 0)
	);
	refreshItemWidgets();
}

} // End of namespace Odyssey

} // End of namespace Engines

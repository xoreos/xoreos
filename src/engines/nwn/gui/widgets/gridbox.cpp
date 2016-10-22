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
 *  A NWN gridbox widget.
 */

#include <cassert>

#include "src/common/util.h"

#include "src/graphics/graphics.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/scrollbar.h"
#include "src/engines/nwn/gui/widgets/gridbox.h"

namespace Engines {

namespace NWN {

WidgetGridBox::WidgetGridBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model, float innerHSpace, float innerVSpace) :
                             WidgetListBox(gui, tag, model), _innerHSpace(innerHSpace),
                             _innerVSpace(innerVSpace) {
}

WidgetGridBox::~WidgetGridBox() {
}

void WidgetGridBox::subActive(Widget &widget) {
	if (widget.getTag().endsWith("#Up")) {
		scrollUp(1);
		return;
	}

	if (widget.getTag().endsWith("#Down")) {
		scrollDown(1);
		return;
	}

	if (widget.getTag().endsWith("#Bar")) {
		if (_itemsByRow == 0)
			return;

		//                                                     Round up
		const size_t rowCount   = (_items.size()        + (_itemsByRow - 1)) / _itemsByRow;
		const size_t visibleRow = (_visibleItems.size() + (_itemsByRow - 1)) / _itemsByRow;

		const ptrdiff_t maxStartRow = rowCount - visibleRow;
		if (maxStartRow <= 0)
			return;

		const size_t startRow  = _scrollbar->getState() * maxStartRow;
		const size_t startItem = startRow * _itemsByRow;

		if (startItem == _startItem)
			return;

		_startItem = startItem;
		updateVisible();
		return;
	}

	WidgetListItem *listItem = dynamic_cast<WidgetListItem *>(&widget);
	if (listItem) {
		if (_selectedItem != listItem->getItemNumber()) {
			_selectedItem = listItem->getItemNumber();
			setActive(true);
			playSound("gui_button", Sound::kSoundTypeSFX);
		}
	}
}

void WidgetGridBox::mouseDown(uint8 UNUSED(state), float x, float y) {
	if (isDisabled() || _visibleItems.empty())
		return;

	float wX, wY, wZ;
	getPosition(wX, wY, wZ);

	// Check if we clicked on the scrollbar area
	if (_scrollbar) {
		if (x > (wX + getWidth() - 20)) {
			size_t scroll = _visibleItems.size() / _itemsByRow;
			if (y > _scrollbar->getBarPosition())
				scrollUp(scroll);
			else
				scrollDown(scroll);

			return;
		}
	}
}

void WidgetGridBox::unlock() {
	assert(_locked);
	_locked = false;

	if (_items.empty()) {
		GfxMan.unlockFrame();
		return;
	}

	_itemsByRow = MIN<size_t>(_contentWidth / _items.front()->getWidth(), _items.size());

	const size_t itemsByColumn = MIN<size_t>(_contentHeight / _items.front()->getHeight(), _items.size());

	const size_t vCount = MIN<size_t>(_itemsByRow * itemsByColumn, _items.size() - _startItem);

	if ((vCount == 0) || (vCount == _visibleItems.size())) {
		GfxMan.unlockFrame();
		return;
	}

	assert(_visibleItems.size() < vCount);
	_visibleItems.reserve(vCount);

	size_t start = _startItem + _visibleItems.size();

	const float itemHeight = _items.front()->getHeight();
	const float itemWidth  = _items.front()->getWidth();

	size_t row = 0, column = 0;

	while (_visibleItems.size() < vCount) {
		WidgetListItem *item = _items[start++];

		// WORKAROUND: The x axis is shifted by 2 pixels in order to correctly render in
		// the charportrait widget.
		float itemY = _contentY - (row + 1) * (itemHeight + _innerVSpace) + _innerVSpace;
		float itemX = _contentX + (column * (itemWidth + _innerHSpace)) - 2.0f;
		item->setPosition(itemX, itemY, _contentZ - 5.0f);
		_visibleItems.push_back(item);

		if (isVisible())
			_visibleItems.back()->show();

		++column;

		if (column == _itemsByRow) {
			++row;
			column = 0;
		}
	}

	updateScrollbarLength();
	updateScrollbarPosition();

	GfxMan.unlockFrame();
}

void WidgetGridBox::updateScrollbarLength() {
	if (!_scrollbar)
		return;

	if (_visibleItems.empty())
		_scrollbar->setLength(1.0f);
	else {
		_scrollbar->setLength(((float) _visibleItems.size()) / (_items.size() + (_items.size() % _itemsByRow)));
	}
}

void WidgetGridBox::updateVisible() {
	if (_visibleItems.empty())
		return;

	GfxMan.lockFrame();

	for (size_t i = 0; i < _visibleItems.size(); i++)
		_visibleItems[i]->hide();

	if (_visibleItems.size() > _items.size())
		_visibleItems.resize(_items.size());

	const float itemHeight = _items.front()->getHeight() + _innerVSpace;
	const float itemWidth  = _items.front()->getWidth() + _innerHSpace;

	const size_t count = MIN<size_t>(_visibleItems.size(), _items.size() - _startItem);

	float itemY = _contentY - itemHeight + _innerVSpace;

	for (size_t i = 0, column = 0; i < count; i++, column++) {
		WidgetListItem *item = _items[_startItem + i];

		if (column == _itemsByRow) {
			itemY -= itemHeight;
			column = 0;
		}

		// WORKAROUND: The x axis is shifted by 2 pixels in order to correctly render in
		// the charportrait widget.
		item->setPosition(_contentX + column * itemWidth - 2.0f, itemY, _contentZ - 5.0f);
		_visibleItems[i] = item;

		if (isVisible())
			_visibleItems[i]->show();
	}

	GfxMan.unlockFrame();
}

void WidgetGridBox::scrollUp(size_t n) {
	if (_visibleItems.empty())
		return;

	if (_startItem == 0)
		return;

	_startItem -= MIN<size_t>(n * _itemsByRow, _startItem);

	updateVisible();
	updateScrollbarPosition();
}

void WidgetGridBox::scrollDown(size_t n) {
	if (_visibleItems.empty())
		return;

	if (_startItem + _visibleItems.size() >= _items.size())
		return;


	_startItem += MIN<size_t>(n * _itemsByRow, (_items.size() + _items.size() %
	                                            _itemsByRow) - _visibleItems.size() - _startItem);

	updateVisible();
	updateScrollbarPosition();
}

} // End of namespace NWN

} // End of namespace Engines

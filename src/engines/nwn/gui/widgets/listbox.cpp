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
 *  A NWN listbox widget.
 */

#include <algorithm>
#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/scrollbar.h"
#include "src/engines/nwn/gui/widgets/listbox.h"

namespace Engines {

namespace NWN {

bool compPtrItem(WidgetListItem *itemA, WidgetListItem *itemB) {
	return *itemA < *itemB;
}

WidgetListItem::WidgetListItem(::Engines::GUI &gui) : NWNWidget(gui, ""), _itemNumber(0xFFFFFFFF), _state(false) {
}

WidgetListItem::~WidgetListItem() {
}

bool WidgetListItem::operator<(const WidgetListItem &item) const {
	return getTag() < item.getTag();
}

void WidgetListItem::mouseUp(uint8 state, float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	activate();
}

void WidgetListItem::mouseDblClick(uint8 state, float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	if (state == SDL_BUTTON_LMASK) {
		WidgetListBox *ownerList = dynamic_cast<WidgetListBox *>(_owner);
		if (ownerList)
			ownerList->itemDblClicked();
	}
}

void WidgetListItem::mouseWheel(uint8 state, int x, int y) {
	if (isDisabled())
		return;

	if (y != 0)
		if (_owner)
			_owner->mouseWheel(state, x, y);

	return;
}

void WidgetListItem::select() {
	if (isDisabled())
		return;

	activate();
}

size_t WidgetListItem::getItemNumber() const {
	return _itemNumber;
}

bool WidgetListItem::getState() {
	return _state;
}

bool WidgetListItem::activate() {
	if (_state)
		return false;

	WidgetListBox *ownerList = dynamic_cast<WidgetListBox *>(_owner);
	if (!ownerList || (ownerList->getMode() != WidgetListBox::kModeSelectable))
		return false;

	_state = true;

	setActive(true);

	return true;
}

bool WidgetListItem::deactivate() {
	if (!_state)
		return false;

	WidgetListBox *ownerList = dynamic_cast<WidgetListBox *>(_owner);
	if (!ownerList || (ownerList->getMode() != WidgetListBox::kModeSelectable))
		return false;

	_state = false;

	return true;
}

void WidgetListItem::signalGroupMemberActive() {
	NWNWidget::signalGroupMemberActive();

	deactivate();
}


WidgetListItemTextLine::WidgetListItemTextLine(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui),
	_uR(1.0f), _uG(1.0f), _uB(1.0f), _uA(1.0f),
	_sR(1.0f), _sG(1.0f), _sB(0.0f), _sA(1.0f), _spacing(spacing) {

	Graphics::Aurora::FontHandle f = FontMan.get(font);

	_fontHeight = f.getFont().getHeight();

	_text.reset(new Graphics::Aurora::Text(f, text, _uR, _uG, _uB, _uA));

	_text->setClickable(true);
}

WidgetListItemTextLine::~WidgetListItemTextLine() {
}

void WidgetListItemTextLine::show() {
	_text->show();
}

void WidgetListItemTextLine::hide() {
	_text->hide();
}

void WidgetListItemTextLine::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y, z);
}

void WidgetListItemTextLine::setUnselectedColor(float r, float g, float b, float a) {
	_uR = r;
	_uG = g;
	_uB = b;
	_uA = a;

	if (!getState())
		_text->setColor(_uR, _uG, _uB, _uA);
}

void WidgetListItemTextLine::setSelectedColor(float r, float g, float b, float a) {
	_sR = r;
	_sG = g;
	_sB = b;
	_sA = a;

	if (getState())
		_text->setColor(_sR, _sG, _sB, _sA);
}

float WidgetListItemTextLine::getWidth() const {
	return _text->getWidth();
}

float WidgetListItemTextLine::getHeight() const {
	if (_text->empty())
		return _fontHeight + _spacing;

	return _text->getHeight() + _spacing;
}

void WidgetListItemTextLine::setTag(const Common::UString &tag) {
	WidgetListItem::setTag(tag);

	_text->setTag(tag);
}

bool WidgetListItemTextLine::activate() {
	if (!WidgetListItem::activate())
		return false;

	_text->setColor(_sR, _sG, _sB, _sA);

	return true;
}

bool WidgetListItemTextLine::deactivate() {
	if (!WidgetListItem::deactivate())
		return false;

	_text->setColor(_uR, _uG, _uB, _uA);

	return true;
}

WidgetListBox::WidgetListBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model) :
	ModelWidget(gui, tag, model), _contentX(0.0f), _contentY(0.0f), _contentZ(0.0f),
	_startItem(0), _selectedItem(0xFFFFFFFF), _up(0), _down(0), _scrollbar(0), _locked(false),
	_mode(kModeStatic), _hasScrollbar(false), _dblClicked(false) {

	_model->setClickable(true);

	getProperties();

	createScrollbar();
}

WidgetListBox::~WidgetListBox() {
}

void WidgetListBox::getProperties() {
	// Do we have a scroll bar?
	_hasScrollbar = _model->hasNode("scrollmin") && _model->hasNode("scrollmax");

	// Calculate content region

	Graphics::Aurora::ModelNode *node = 0;

	float topX = 8.0f, topY = getHeight() - 6.0f, topZ = 0.0f;
	if ((node = _model->getNode("text0")))
		node->getPosition(topX, topY, topZ);

	float bottomX = getWidth() - (_hasScrollbar ? 25.0f : 3.0f), bottomY = 3.0f, bottomZ = 0.0f;
	if ((node = _model->getNode("text1")))
		node->getPosition(bottomX, bottomY, bottomZ);

	_contentX = topX;
	_contentY = topY;

	_contentWidth  = bottomX - topX;
	_contentHeight = topY - bottomY;
}

void WidgetListBox::createScrollbar() {
	if (!_hasScrollbar)
		return;

	// Get top position
	float minX, minY, minZ;
	_model->getNode("scrollmin")->getPosition(minX, minY, minZ);

	// Create the "up" button
	_up = new WidgetButton(*_gui, getTag() + "#Up", "pb_scrl_up", "gui_scroll");
	_up->setPosition(minX, minY, -50.0f);
	addSub(*_up);

	// Get bottom position
	float maxX, maxY, maxZ;
	_model->getNode("scrollmax")->getPosition(maxX, maxY, maxZ);

	// Create the "down" button
	_down = new WidgetButton(*_gui, getTag() + "#Down", "pb_scrl_down", "gui_scroll");
	_down->setPosition(maxX, maxY - 10, -50.0f);
	addSub(*_down);

	// Scroll bar range (max length)
	float scrollRange = minY - (maxY - 10) - _up->getHeight() - 1;

	// Create the scrollbar
	_scrollbar =
		new WidgetScrollbar(*_gui, getTag() + "#Bar", Scrollbar::kTypeVertical, scrollRange);

	// Center the bar within the scrollbar area
	float scrollX = maxX + (_up->getWidth() - _scrollbar->getWidth()) / 2;
	// Move it to the base position
	float scrollY = maxY - 10 + _up->getHeight();

	_scrollbar->setPosition(scrollX, scrollY, -50.0f);
	addSub(*_scrollbar);
}

WidgetListBox::Mode WidgetListBox::getMode() const {
	return _mode;
}

void WidgetListBox::setMode(Mode mode) {
	_mode = mode;
}

void WidgetListBox::show() {
	ModelWidget::show();

	// Show the scrollbar
	if (_hasScrollbar) {
		_up->show();
		_down->show();
		_scrollbar->show();
	}

	// Show the visible items
	for (std::vector<WidgetListItem *>::iterator v = _visibleItems.begin(); v != _visibleItems.end(); ++v)
		(*v)->show();
}

void WidgetListBox::hide() {
	ModelWidget::hide();

	// Hide the scrollbar
	if (_hasScrollbar) {
		_up->hide();
		_down->hide();
		_scrollbar->hide();
	}

	// Hide the visible items
	for (std::vector<WidgetListItem *>::iterator v = _visibleItems.begin(); v != _visibleItems.end(); ++v)
		(*v)->hide();
}

void WidgetListBox::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	ModelWidget::setPosition(x, y, z);

	float nX, nY, nZ;
	getPosition(nX, nY, nZ);

	getPosition(x, y, z);
	for (std::list<Widget *>::iterator it = _subWidgets.begin(); it != _subWidgets.end(); ++it) {
		float sX, sY, sZ;
		(*it)->getPosition(sX, sY, sZ);

		sX -= oX;
		sY -= oY;
		sZ -= oZ;

		(*it)->setPosition(sX + nX, sY + nY, sZ + nZ);
	}

	_contentX = _contentX - oX + nX;
	_contentY = _contentY - oY + nY;
	_contentZ = _contentZ - oZ + nZ;
}

float WidgetListBox::getContentWidth() const {
	return _contentWidth;
}

float WidgetListBox::getContentHeight() const {
	return _contentHeight;
}

void WidgetListBox::lock() {
	assert(!_locked);
	_locked = true;

	GfxMan.lockFrame();
}

void WidgetListBox::clear() {
	assert(_locked);

	for (std::vector<WidgetListItem *>::iterator v = _visibleItems.begin(); v != _visibleItems.end(); ++v)
		(*v)->hide();
	_visibleItems.clear();

	for (std::vector<WidgetListItem *>::iterator i = _items.begin(); i != _items.end(); ++i)
		(*i)->remove();
	_items.clear();

	_startItem    = 0;
	_selectedItem = 0xFFFFFFFF;

	updateScrollbarLength();
}

void WidgetListBox::reserve(size_t n) {
	assert(_locked);

	_items.reserve(n);
}

void WidgetListBox::add(WidgetListItem *item, bool noTag) {
	assert(_locked);

	if (!_items.empty())
		if (round(item->getHeight()) != round(_items.front()->getHeight()))
			throw Common::Exception("WidgetListBox item sizes mismatch");

	item->_itemNumber = _items.size();

	if (!noTag)
		item->setTag(Common::UString::format("%s#Item%d", getTag().c_str(), (int)_items.size()));

	for (std::vector<WidgetListItem *>::iterator i = _items.begin(); i != _items.end(); ++i) {
		(*i)->addGroupMember(*item);
		item->addGroupMember(**i);
	}

	_items.push_back(item);

	addSub(*item);

	if (isVisible()) {
		updateScrollbarLength();
		updateScrollbarPosition();
	}
}

void WidgetListBox::remove(WidgetListItem *item) {
	assert(_locked);

	for (size_t it = 0; it < _visibleItems.size(); ++it) {
		if (_visibleItems[it] == item) {
			_visibleItems.erase(_visibleItems.begin() + it);
			break;
		}
	}

	std::vector<WidgetListItem *>::iterator itItem;
	for (std::vector<WidgetListItem *>::iterator i = _items.begin(); i != _items.end(); ++i) {
		(*i)->removeGroupMember(*item);
		item->removeGroupMember(**i);
		if (*i == item)
			itItem = i;
	}

	_items.erase(itItem);

	removeSub(*item);
	item->_itemNumber = 0xFFFFFFFF;

	_selectedItem = 0xFFFFFFFF;

	ptrdiff_t max = _items.size() - _visibleItems.size();
	if (max > 0 && _scrollbar->getState() == 1.f)
		_startItem -= 1;

	if (item->isVisible())
		item->hide();
}

void WidgetListBox::unlock() {
	assert(_locked);
	_locked = false;

	if (_items.empty()) {
		GfxMan.unlockFrame();
		return;
	}

	size_t count = MIN<size_t>(_contentHeight / _items.front()->getHeight(), _items.size());
	if ((count == 0) || (count == _visibleItems.size())) {
		GfxMan.unlockFrame();
		return;
	}

	assert(_visibleItems.size() < count);

	_visibleItems.reserve(count);

	size_t start = _startItem + _visibleItems.size();

	float itemHeight = _items.front()->getHeight();
	while (_visibleItems.size() < count) {
		WidgetListItem *item = _items[start++];

		float itemY = _contentY - (_visibleItems.size() + 1) * itemHeight;
		item->setPosition(_contentX, itemY, _contentZ - 5.0f);

		_visibleItems.push_back(item);
		if (isVisible())
			_visibleItems.back()->show();
	}

	updateScrollbarLength();
	updateScrollbarPosition();

	GfxMan.unlockFrame();

	if (isVisible())
		updateVisible();
}

void WidgetListBox::sortByTag() {
	std::sort(_items.begin(), _items.end(), compPtrItem);
	for (uint16 it = 0; it < _items.size(); ++it) {
		_items[it]->_itemNumber = it;
	}

	if (isVisible())
		updateVisible();
}

void WidgetListBox::setText(const Common::UString &font,
                            const Common::UString &text, float spacing) {

	lock();
	clear();

	Graphics::Aurora::FontHandle f = FontMan.get(font);
	std::vector<Common::UString> lines;
	f.getFont().split(text, lines, getContentWidth());

	for (std::vector<Common::UString>::iterator l = lines.begin(); l != lines.end(); ++l)
		add(new WidgetListItemTextLine(*_gui, font, *l, spacing));

	unlock();
}

void WidgetListBox::updateScrollbarLength() {
	if (!_scrollbar)
		return;

	if (_visibleItems.empty())
		_scrollbar->setLength(1.0f);
	else
		_scrollbar->setLength(((float) _visibleItems.size()) / _items.size());
}

void WidgetListBox::updateScrollbarPosition() {
	if (!_scrollbar)
		return;

	ptrdiff_t max = _items.size() - _visibleItems.size();
	if (max > 0)
		_scrollbar->setState(((float) _startItem) / max);
	else
		_scrollbar->setState(0.0f);
}

void WidgetListBox::updateVisible() {
	if (_visibleItems.empty())
		return;

	GfxMan.lockFrame();

	for (size_t i = 0; i < _visibleItems.size(); i++)
		_visibleItems[i]->hide();

	float itemHeight = _items.front()->getHeight();
	float itemY      = _contentY;
	for (size_t i = 0; i < _visibleItems.size(); i++) {
		WidgetListItem *item = _items[_startItem + i];

		itemY -= itemHeight;

		item->setPosition(_contentX, itemY, _contentZ - 5.0f);
		_visibleItems[i] = item;

		if (isVisible())
			_visibleItems[i]->show();
	}

	GfxMan.unlockFrame();
}

void WidgetListBox::itemDblClicked() {
	_dblClicked = true;

	setActive(true);
}

void WidgetListBox::scrollUp(size_t n) {
	if (_visibleItems.empty())
		return;

	if (_startItem == 0)
		return;

	_startItem -= MIN<size_t>(n, _startItem);

	updateVisible();
	updateScrollbarPosition();
}

void WidgetListBox::scrollDown(size_t n) {
	if (_visibleItems.empty())
		return;

	if (_startItem + _visibleItems.size() >= _items.size())
		return;

	_startItem += MIN<size_t>(n, _items.size() - _visibleItems.size() - _startItem);

	updateVisible();
	updateScrollbarPosition();
}

void WidgetListBox::select(size_t item) {
	if (item >= _items.size())
		return;

	_items[item]->select();
	_selectedItem = item;
}

size_t WidgetListBox::getSelected() const {
	return _selectedItem;
}

WidgetListItem *WidgetListBox::getSelectedItem() const {
	if (_selectedItem >= _items.size()) return 0;

	return _items[_selectedItem];
}

void WidgetListBox::deselect() {
	if (_selectedItem >= _items.size()) return;

	_items[_selectedItem]->deactivate();
	_selectedItem = 0xFFFFFFFF;
}

void WidgetListBox::setStartItem(size_t firstItem) {
	_startItem = MIN<size_t>(firstItem, _items.size() - _visibleItems.size());

	updateVisible();
	updateScrollbarPosition();
}

size_t WidgetListBox::getStartItem() const {
	return _startItem;
}

std::vector<WidgetListItem *>::iterator WidgetListBox::begin() {
	return _items.begin();
}

std::vector<WidgetListItem *>::iterator WidgetListBox::end() {
	return _items.end();
}

bool WidgetListBox::wasDblClicked() {
	bool dblClicked = _dblClicked;

	_dblClicked = false;

	return dblClicked;
}

void WidgetListBox::subActive(Widget &widget) {
	if (widget.getTag().endsWith("#Up")) {
		scrollUp(1);
		return;
	}

	if (widget.getTag().endsWith("#Down")) {
		scrollDown(1);
		return;
	}

	if (widget.getTag().endsWith("#Bar")) {
		ptrdiff_t max = _items.size() - _visibleItems.size();
		if (max <= 0)
			return;

		size_t startItem = _scrollbar->getState() * max;
		if (startItem == _startItem)
			return;

		_startItem = startItem;
		updateVisible();
		return;
	}

	WidgetListItem *listItem = dynamic_cast<WidgetListItem *>(&widget);
	if (listItem) {
		if (_selectedItem != listItem->_itemNumber) {
			_selectedItem = listItem->_itemNumber;
			setActive(true);
		}
	}

}

void WidgetListBox::mouseDown(uint8 UNUSED(state), float x, float y) {
	if (isDisabled())
		return;

	float wX, wY, wZ;
	getPosition(wX, wY, wZ);

	// Check if we clicked on the scrollbar area
	if (_scrollbar) {
		if (x > (wX + getWidth() - 20)) {
			if (y > _scrollbar->getBarPosition())
				scrollUp(_visibleItems.size());
			else
				scrollDown(_visibleItems.size());

			return;
		}
	}
}

void WidgetListBox::mouseWheel(uint8 UNUSED(state), int UNUSED(x), int y) {
	if (isDisabled())
		return;

	if (y > 0)
		scrollUp(1);
	else if (y < 0)
		scrollDown(1);
}


} // End of namespace NWN

} // End of namespace Engines

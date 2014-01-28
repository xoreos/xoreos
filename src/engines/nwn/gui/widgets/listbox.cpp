/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/listbox.cpp
 *  A NWN listbox widget.
 */

#include <OgreVector3.h>
#include <OgreSceneNode.h>

#include "common/util.h"
#include "common/error.h"
#include "common/ustring.h"

#include "graphics/graphics.h"
#include "graphics/guiman.h"
#include "graphics/font.h"

#include "graphics/aurora/model_nwn.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/fontman.h"

#include "engines/nwn/gui/widgets/button.h"
#include "engines/nwn/gui/widgets/scrollbar.h"
#include "engines/nwn/gui/widgets/listbox.h"

namespace Engines {

namespace NWN {

WidgetListItem::WidgetListItem(::Engines::GUI &gui) : NWNWidget(gui, ""), _itemNumber(0xFFFFFFFF), _state(false) {
}

WidgetListItem::~WidgetListItem() {
}

void WidgetListItem::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	activate();
}

void WidgetListItem::mouseDblClick(uint8 state, float x, float y) {
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
	_uR(1.0), _uG(1.0), _uB(1.0), _uA(1.0),
	_sR(1.0), _sG(1.0), _sB(0.0), _sA(1.0), _spacing(spacing) {

	Graphics::Aurora::FontHandle f = FontMan.get(font);

	_fontHeight = f.getFont().getHeight();

	_text = new Graphics::Aurora::Text(f, text, _uR, _uG, _uB, _uA);
	_text->setSelectable(true);

	_ids.push_back(_text->getID());

	updateSize();

	GUIMan.addRenderable(_text);
}

WidgetListItemTextLine::~WidgetListItemTextLine() {
	setVisible(false);

	GUIMan.removeRenderable(_text);
	delete _text;
}

void WidgetListItemTextLine::updateSize() {
	_text->getSize(_width, _height, _depth);
}

void WidgetListItemTextLine::setVisible(bool visible) {
	_text->setVisible(visible);
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
	return _width;
}

float WidgetListItemTextLine::getHeight() const {
	if (_text->get().empty())
		return _fontHeight + _spacing;

	return _height + _spacing;
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
	ModelWidget(gui, tag, model),
	_mode(kModeStatic), _contentX(0.0), _contentY(0.0), _contentZ(0.0),
	_hasScrollbar(false), _up(0), _down(0), _scrollbar(0), _dblClicked(false),
	_startItem(0), _selectedItem(0xFFFFFFFF), _locked(false) {

	_model->setSelectable(true);

	getProperties();

	createScrollbar();
}

WidgetListBox::~WidgetListBox() {
}

void WidgetListBox::getProperties() {
	// Do we have a scroll bar?
	_hasScrollbar = _model->hasNode("scrollmin") && _model->hasNode("scrollmax");

	// Calculate content region

	Ogre::SceneNode *node = 0;

	Ogre::Vector3 top(0.08, getHeight() - 0.06, 0.0);
	if ((node = _model->getNode("text0")))
		top = node->getPosition();

	Ogre::Vector3 bottom(getWidth() - (_hasScrollbar ? 0.25 : 0.03), 0.03, 0.0);
	if ((node = _model->getNode("text1")))
		bottom = node->getPosition();

	_contentX = top[0] * 100.0;
	_contentY = top[1] * 100.0;

	_contentWidth  = (bottom[0] - top[0]) * 100.0;
	_contentHeight = (top[1] - bottom[1]) * 100.0;
}

void WidgetListBox::createScrollbar() {
	if (!_hasScrollbar)
		return;

	// Get top position
	Ogre::Vector3 min = _model->getNode("scrollmin")->getPosition();

	// Create the "up" button
	_up = new WidgetButton(*_gui, getTag() + "#Up", "pb_scrl_up", "gui_scroll");
	_up->setPosition(min[0] * 100.0, min[1] * 100.0, 5.0);
	addSub(*_up);

	// Get bottom position
	Ogre::Vector3 max = _model->getNode("scrollmax")->getPosition();

	// Create the "down" button
	_down = new WidgetButton(*_gui, getTag() + "#Down", "pb_scrl_down", "gui_scroll");
	_down->setPosition(max[0] * 100.0, max[1] * 100.0 - 10.0, 5.0);
	addSub(*_down);

	// Scroll bar range (max length)
	float scrollRange = min[1] * 100.0 - (max[1] * 100.0 - 10.0) - _up->getHeight() - 1;

	// Create the scrollbar
	_scrollbar = new WidgetScrollbar(*_gui, getTag() + "#Bar", Scrollbar::kTypeVertical, scrollRange);

	// Center the bar within the scrollbar area
	float scrollX = max[0] * 100.0 + (_up->getWidth() - _scrollbar->getWidth()) / 2;
	// Move it to the base position
	float scrollY = max[1] * 100.0 - 10.0 + _up->getHeight();

	_scrollbar->setPosition(scrollX, scrollY, 5.0);
	addSub(*_scrollbar);
}

WidgetListBox::Mode WidgetListBox::getMode() const {
	return _mode;
}

void WidgetListBox::setMode(Mode mode) {
	_mode = mode;
}

void WidgetListBox::setVisible(bool visible) {
	ModelWidget::setVisible(visible);

	// Show the scrollbar
	if (_hasScrollbar) {
		_up->setVisible(visible);
		_down->setVisible(visible);
		_scrollbar->setVisible(visible);
	}

	// Show the visible items
	for (std::vector<WidgetListItem *>::iterator v = _visibleItems.begin(); v != _visibleItems.end(); ++v)
		(*v)->setVisible(visible);
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
		sZ += oZ;

		(*it)->setPosition(floor(sX + nX), floor(sY + nY), sZ + nZ);
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
		(*v)->setVisible(false);
	_visibleItems.clear();

	for (std::vector<WidgetListItem *>::iterator i = _items.begin(); i != _items.end(); ++i)
		(*i)->remove();
	_items.clear();

	_startItem    = 0;
	_selectedItem = 0xFFFFFFFF;

	updateScrollbarLength();
}

void WidgetListBox::reserve(uint n) {
	assert(_locked);

	_items.reserve(n);
}

void WidgetListBox::add(WidgetListItem *item) {
	assert(_locked);

	if (!_items.empty())
		if (round(item->getHeight()) != round(_items.front()->getHeight()))
			throw Common::Exception("WidgetListBox item sizes mismatch");

	item->_itemNumber = _items.size();

	item->setTag(Common::UString::sprintf("%s#Item%d", getTag().c_str(), (int)_items.size()));

	for (std::vector<WidgetListItem *>::iterator i = _items.begin(); i != _items.end(); ++i) {
		(*i)->addGroupMember(*item);
		item->addGroupMember(**i);
	}

	_items.push_back(item);

	addSub(*item);
}

void WidgetListBox::unlock() {
	assert(_locked);
	_locked = false;

	if (_items.empty()) {
		GfxMan.unlockFrame();
		return;
	}

	uint count = MIN<uint>(_contentHeight / _items.front()->getHeight(), _items.size());
	if ((count == 0) || (count == _visibleItems.size())) {
		GfxMan.unlockFrame();
		return;
	}

	assert(_visibleItems.size() < count);

	_visibleItems.reserve(count);

	uint start = _startItem + _visibleItems.size();

	float itemHeight = _items.front()->getHeight();
	while (_visibleItems.size() < count) {
		WidgetListItem *item = _items[start++];

		float itemY = _contentY - (_visibleItems.size()) * itemHeight;
		item->setPosition(floor(_contentX), floor(itemY), _contentZ + 5.0);

		_visibleItems.push_back(item);
		if (isVisible())
			_visibleItems.back()->setVisible(true);
	}

	updateScrollbarLength();
	updateScrollbarPosition();

	GfxMan.unlockFrame();
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
		_scrollbar->setLength(1.0);
	else
		_scrollbar->setLength(((float) _visibleItems.size()) / _items.size());
}

void WidgetListBox::updateScrollbarPosition() {
	if (!_scrollbar)
		return;

	int max = _items.size() - _visibleItems.size();
	if (max > 0)
		_scrollbar->setState(((float) _startItem) / max);
	else
		_scrollbar->setState(0.0);
}

void WidgetListBox::updateVisible() {
	if (_visibleItems.empty())
		return;

	for (uint i = 0; i < _visibleItems.size(); i++)
		_visibleItems[i]->setVisible(false);

	float itemHeight = _items.front()->getHeight();
	float itemY      = _contentY;
	for (uint i = 0; i < _visibleItems.size(); i++, itemY -= itemHeight) {
		WidgetListItem *item = _items[_startItem + i];

		item->setPosition(floor(_contentX), floor(itemY), _contentZ + 5.0);
		_visibleItems[i] = item;

		if (isVisible())
			_visibleItems[i]->setVisible(true);
	}
}

void WidgetListBox::itemDblClicked() {
	_dblClicked = true;

	setActive(true);
}

void WidgetListBox::scrollUp(uint n) {
	if (_visibleItems.empty())
		return;

	if (_startItem == 0)
		return;

	LOCK_FRAME();

	_startItem -= MIN<uint>(n, _startItem);

	updateVisible();
	updateScrollbarPosition();

	GUIMan.update();
}

void WidgetListBox::scrollDown(uint n) {
	if (_visibleItems.empty())
		return;

	if (_startItem + _visibleItems.size() >= _items.size())
		return;

	LOCK_FRAME();

	_startItem += MIN<uint>(n, _items.size() - _visibleItems.size() - _startItem);

	updateVisible();
	updateScrollbarPosition();

	GUIMan.update();
}

void WidgetListBox::select(uint item) {
	if (item >= _items.size())
		return;

	_items[item]->select();
	_selectedItem = item;
}

uint WidgetListBox::getSelected() const {
	return _selectedItem;
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
		int max = _items.size() - _visibleItems.size();
		if (max <= 0)
			return;

		uint startItem = _scrollbar->getState() * max;
		if (startItem == _startItem)
			return;

		_startItem = startItem;

		LOCK_FRAME();
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

void WidgetListBox::mouseDown(uint8 state, float x, float y) {
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

void WidgetListBox::mouseWheel(uint8 state, int x, int y) {
	if (isDisabled())
		return;

	if (y > 0)
		scrollUp(1);
	else if (y < 0)
		scrollDown(1);
}


} // End of namespace NWN

} // End of namespace Engines

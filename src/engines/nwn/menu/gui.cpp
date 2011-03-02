/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/gui.cpp
 *  A NWN GUI.
 */

#include "common/endianness.h"
#include "common/error.h"
#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/talkman.h"

#include "events/requests.h"

#include "graphics/graphics.h"
#include "graphics/font.h"

#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

NWNModelWidget::NWNModelWidget(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model) : Widget(gui, tag) {

	if (!(_model = loadModelGUI(model)))
		throw Common::Exception("Can't load widget \"%s\" for widget \"%s\"",
				model.c_str(), tag.c_str());

	_model->setTag(tag);
}

NWNModelWidget::~NWNModelWidget() {
	freeModel(_model);
}

void NWNModelWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_model->show();

	Widget::show();
}

void NWNModelWidget::hide() {
	if (!isVisible())
		return;

	_model->hide();
	Widget::hide();
}

void NWNModelWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_model->setPosition(x, y, z);
}

float NWNModelWidget::getWidth() const {
	return _model->getWidth();
}

float NWNModelWidget::getHeight() const {
	return _model->getHeight();
}


NWNTextWidget::NWNTextWidget(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &font, const Common::UString &text) :
	Widget(gui, tag), _r(1.0), _g(1.0), _b(1.0), _a(1.0) {

	_text = new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a, 0.5);
}

NWNTextWidget::~NWNTextWidget() {
	delete _text;
}

void NWNTextWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_text->show();

	Widget::show();
}

void NWNTextWidget::hide() {
	if (!isVisible())
		return;

	_text->hide();
	Widget::hide();
}

void NWNTextWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y, -z);
}

void NWNTextWidget::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_text->setColor(_r, _g, _b, _a);
}

void NWNTextWidget::setText(const Common::UString &text) {
	_text->set(text);
}

float NWNTextWidget::getWidth() const {
	return _text->getWidth();
}

float NWNTextWidget::getHeight() const {
	return _text->getHeight();
}

void NWNTextWidget::setDisabled(bool disabled) {
	if (isDisabled())
		return;

	_a = disabled ? (_a * 0.6) : (_a / 0.6);

	_text->setColor(_r, _g, _b, _a);

	Widget::setDisabled(disabled);
}


WidgetScrollbar::WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
                                 Scrollbar::Type type, float range) :
		Widget(gui, tag), _type(type), _range(range), _state(0.0), _scrollbar(type) {

	_scrollbar.setTag(tag);

	setLength(1.0);
}

WidgetScrollbar::~WidgetScrollbar() {
}

void WidgetScrollbar::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_scrollbar.show();

	Widget::show();
}

void WidgetScrollbar::hide() {
	if (!isVisible())
		return;

	_scrollbar.hide();
	Widget::hide();
}

void WidgetScrollbar::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	setState(_state);
}

void WidgetScrollbar::setLength(float percent) {
	_full = percent >= 1.0;

	// Calculate the actual length, at 2 pixel intervals
	_length = ceilf(MAX(_range * CLIP(percent, 0.0f, 1.0f), 10.0f));
	if ((((int) _length) % 2) == 1)
		_length += 1.0;

	if (_length > _range)
		_length = _range;

	_scrollbar.setLength(_length);

	setState(_state);
}

float WidgetScrollbar::getState() const {
	return _state;
}

void WidgetScrollbar::setState(float state) {
	_state = CLIP(state, 0.0f, 1.0f);

	float span = _range - _length; // Space to scroll in
	float pos  = _state * span;    // Offset within that space

	float x, y, z;
	getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		y += span - pos;
	else if (_type == Scrollbar::kTypeHorizontal)
		x += pos;

	_scrollbar.setPosition(x, y, z);
}

float WidgetScrollbar::getWidth() const {
	return _scrollbar.getWidth();
}

float WidgetScrollbar::getHeight() const {
	return _scrollbar.getHeight();
}

float WidgetScrollbar::getBarPosition() const {
	float x, y, z;
	_scrollbar.getPosition(x, y, z);

	if      (_type == Scrollbar::kTypeVertical)
		return y;
	else if (_type == Scrollbar::kTypeHorizontal)
		return x;

	return 0.0;
}

void WidgetScrollbar::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	// We only care about the left mouse button, pass everything else to the owner
	if (state != SDL_BUTTON_LMASK) {
		if (_owner)
			_owner->mouseDown(state, x, y);
		return;
	}

	_dragX     = x;
	_dragY     = y;
	_dragState = _state;
}

void WidgetScrollbar::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (_full)
		// Can't scroll when the bar is going full length
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	float steps = 1.0 / (_range - _length);

	if      (_type == Scrollbar::kTypeVertical)
		setState(_dragState + ((_dragY - y) * steps));
	else if (_type == Scrollbar::kTypeHorizontal)
		setState(_dragState + ((x - _dragX) * steps));

	setActive(true);
}


WidgetFrame::WidgetFrame(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNModelWidget(gui, tag, model) {

}

WidgetFrame::~WidgetFrame() {
}


WidgetClose::WidgetClose(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNModelWidget(gui, tag, model) {

}

WidgetClose::~WidgetClose() {
}

void WidgetClose::leave() {
	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetClose::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_model->setState("down");
	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetClose::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}


WidgetCheckBox::WidgetCheckBox(::Engines::GUI &gui, const Common::UString &tag,
                               const Common::UString &model) :
	NWNModelWidget(gui, tag, model) {

	_state = false;
	updateModel(false);
}

WidgetCheckBox::~WidgetCheckBox() {
}

void WidgetCheckBox::updateModel(bool highlight) {
	if (highlight) {
		if (_state)
			_model->setState("");
		else
			_model->setState("hilite");
	} else {
		if (_state)
			_model->setState("");
		else
			_model->setState("uncheckedup");
	}
}

bool WidgetCheckBox::getState() const {
	return _state;
}

void WidgetCheckBox::setState(bool state) {
	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (!state)
			// We can't just uncheck a radio button without checking another one
			return;

		_state = true;
		updateModel(false);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !!state;
		updateModel(false);
		setActive(true);
	}
}

void WidgetCheckBox::enter() {
	if (isDisabled())
		return;

	updateModel(true);
}

void WidgetCheckBox::leave() {
	if (isDisabled())
		return;

	updateModel(false);
}

void WidgetCheckBox::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	playSound("gui_check", Sound::kSoundTypeSFX);
}

void WidgetCheckBox::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (!_groupMembers.empty()) {
		// Group members, we are a radio button

		if (_state)
			// We are already active
			return;

		_state = true;
		updateModel(true);
		setActive(true);

	} else {
		// No group members, we are a check box

		_state = !_state;
		updateModel(true);
		setActive(true);
	}

}

void WidgetCheckBox::signalGroupMemberActive() {
	Widget::signalGroupMemberActive();

	_state = false;
	updateModel(false);
}


WidgetPanel::WidgetPanel(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : NWNModelWidget(gui, tag, model) {

}

WidgetPanel::~WidgetPanel() {
}


WidgetLabel::WidgetLabel(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &font, const Common::UString &text) :
	NWNTextWidget(gui, tag, font, text) {

}

WidgetLabel::~WidgetLabel() {
}


WidgetSlider::WidgetSlider(::Engines::GUI &gui, const Common::UString &tag, 
                           const Common::UString &model) :
	NWNModelWidget(gui, tag, model), _position(0.0), _steps(0), _state(0) {

	_width = getWidth();

	changePosition(0.0);
}

WidgetSlider::~WidgetSlider() {
}

void WidgetSlider::setPosition(float x, float y, float z) {
	NWNModelWidget::setPosition(x, y, z);
}

void WidgetSlider::setSteps(int steps) {
	_steps = steps;
}

int WidgetSlider::getState() const {
	return _state;
}

void WidgetSlider::setState(int state) {
	_state = state;

	changePosition(CLIP(((float) _state) / _steps, 0.0f, 1.0f));
}

void WidgetSlider::mouseMove(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		// We only care about moves with the left mouse button pressed
		return;

	changedValue(x, y);
}

void WidgetSlider::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	changedValue(x, y);
}

void WidgetSlider::changedValue(float x, float y) {
	float curX, curY, curZ;
	getPosition(curX, curY, curZ);

	float pX    = CLIP(x - curX, 0.0f, _width) / _width;
	int   state = roundf(pX * _steps);

	if (state == _state)
		// No change
		return;

	_state = state;

	if (_steps == 0) {
		changePosition(0.0);
		return;
	}

	changePosition(((float) _state) / _steps);

	setActive(true);
}

void WidgetSlider::changePosition(float value) {
	value = (value * _width) - (_model->getNodeWidth("thumb") / 2.0);

	_model->moveNode("thumb", -_position + value, 0.0, 0.0);

	_position = value;
}


WidgetEditBox::WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model, const Common::UString &font) :
	NWNModelWidget(gui, tag, model) {

}

WidgetEditBox::~WidgetEditBox() {
}


WidgetButton::WidgetButton(::Engines::GUI &gui, const Common::UString &tag,
                           const Common::UString &model, const Common::UString &sound) :
	NWNModelWidget(gui, tag, model) {

	_sound = sound;
}

WidgetButton::~WidgetButton() {
}

void WidgetButton::enter() {
	if (isDisabled())
		return;

	_model->setState("hilite");
}

void WidgetButton::leave() {
	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetButton::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_model->setState("down");
	playSound(_sound, Sound::kSoundTypeSFX);
}

void WidgetButton::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}


WidgetListItem::WidgetListItem(::Engines::GUI &gui) : Widget(gui, ""), _state(false) {
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

void WidgetListItem::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if ((state == SDL_BUTTON_WHEELUP) || (state == SDL_BUTTON_WHEELDOWN)) {
		if (_owner)
			_owner->mouseDown(state, x, y);
		return;
	}
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
	Widget::signalGroupMemberActive();

	deactivate();
}


WidgetListItemTextLine::WidgetListItemTextLine(::Engines::GUI &gui,
    const Common::UString &font, const Common::UString &text, float spacing) :
	WidgetListItem(gui),
	_uR(1.0), _uG(1.0), _uB(1.0), _uA(1.0),
	_sR(1.0), _sG(1.0), _sB(0.0), _sA(1.0), _spacing(spacing) {

	Graphics::Aurora::FontHandle f = FontMan.get(font);

	_fontHeight = f.getFont().getHeight();

	_text = new Graphics::Aurora::Text(f, text, _uR, _uG, _uB, _uA, 0.0);
}

WidgetListItemTextLine::~WidgetListItemTextLine() {
	delete _text;
}

void WidgetListItemTextLine::show() {
	_text->show();
}

void WidgetListItemTextLine::hide() {
	_text->hide();
}

void WidgetListItemTextLine::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y, -z);
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
	if (_text->isEmpty())
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
	NWNModelWidget(gui, tag, model),
	_mode(kModeStatic), _contentX(0.0), _contentY(0.0), _contentZ(0.0),
	_hasScrollbar(false), _up(0), _down(0), _scrollbar(0), _dblClicked(false),
	_startItem(0), _selectedItem(0xFFFFFFFF), _locked(false) {

	getProperties();

	createScrollbar();
}

WidgetListBox::~WidgetListBox() {
}

void WidgetListBox::getProperties() {
	// Do we have a scroll bar?
	_hasScrollbar = _model->hasNode("scrollmin") && _model->hasNode("scrollmax");

	// Calculate content region

	float topX = 8.0, topY = getHeight() - 6.0, topZ = 0.0;
	_model->getNodePosition("text0", topX, topY, topZ);

	float bottomX = getWidth() - (_hasScrollbar ? 25.0 : 3.0), bottomY = 3.0, bottomZ = 0.0;
	_model->getNodePosition("text1", bottomX, bottomY, bottomZ);

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
	_model->getNodePosition("scrollmin", minX, minY, minZ);

	// Create the "up" button
	_up = new WidgetButton(*_gui, getTag() + "#Up", "pb_scrl_up", "gui_scroll");
	_up->setPosition(minX, minY, 0.0);
	addSub(*_up);

	// Get bottom position
	float maxX, maxY, maxZ;
	_model->getNodePosition("scrollmax", maxX, maxY, maxZ);

	// Create the "down" button
	_down = new WidgetButton(*_gui, getTag() + "#Down", "pb_scrl_down", "gui_scroll");
	_down->setPosition(maxX, maxY - 10, 0.0);
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

	_scrollbar->setPosition(scrollX, scrollY, 0.0);
	addSub(*_scrollbar);
}

WidgetListBox::Mode WidgetListBox::getMode() const {
	return _mode;
}

void WidgetListBox::setMode(Mode mode) {
	_mode = mode;
}

void WidgetListBox::show() {
	NWNModelWidget::show();

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
	NWNModelWidget::hide();

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

	NWNModelWidget::setPosition(x, y, z);

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

void WidgetListBox::reserve(uint n) {
	assert(_locked);

	_items.reserve(n);
}

void WidgetListBox::add(WidgetListItem *item) {
	assert(_locked);

	if (!_items.empty())
		if (item->getHeight() != _items.front()->getHeight())
			throw Common::Exception("WidgetListBox item sizes mismatch");

	item->_itemNumber = _items.size();

	item->setTag(Common::UString::sprintf("%s#Item%d", getTag().c_str(), _items.size()));

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

		float itemY = _contentY - (_visibleItems.size() + 1) * itemHeight;
		item->setPosition(_contentX, itemY, _contentZ);

		_visibleItems.push_back(item);
		if (isVisible())
			_visibleItems.back()->show();
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

	GfxMan.lockFrame();

	for (uint i = 0; i < _visibleItems.size(); i++)
		_visibleItems[i]->hide();

	float itemHeight = _items.front()->getHeight();
	float itemY      = _contentY;
	for (uint i = 0; i < _visibleItems.size(); i++) {
		WidgetListItem *item = _items[_startItem + i];

		itemY -= itemHeight;

		item->setPosition(_contentX, itemY, _contentZ);
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

void WidgetListBox::scrollUp(uint n) {
	if (_visibleItems.empty())
		return;

	if (_startItem == 0)
		return;

	_startItem -= MIN<uint>(n, _startItem);

	updateVisible();
	updateScrollbarPosition();
}

void WidgetListBox::scrollDown(uint n) {
	if (_visibleItems.empty())
		return;

	if (_startItem + _visibleItems.size() >= _items.size())
		return;

	_startItem += MIN<uint>(n, _items.size() - _visibleItems.size() - _startItem);

	updateVisible();
	updateScrollbarPosition();
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

	if (state == SDL_BUTTON_WHEELUP) {
		scrollUp(1);
		return;
	}

	if (state == SDL_BUTTON_WHEELDOWN) {
		scrollDown(1);
		return;
	}

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


GUI::WidgetContext::WidgetContext(const Aurora::GFFStruct &s, Widget *p) {
	strct = &s;

	widget = 0;
	parent = p;

	type = (WidgetType) strct->getUint("Obj_Type", kWidgetTypeInvalid);
	if (type == kWidgetTypeInvalid)
		throw Common::Exception("Widget without a type");

	tag = strct->getString("Obj_Tag");

	model = strct->getString("Obj_ResRef");

	if (strct->hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = strct->getStruct("Obj_Caption");

		font = caption.getString("AurString_Font");

		uint32 strRef = caption.getUint("Obj_StrRef", 0xFFFFFFFF);
		if (strRef != 0xFFFFFFFF)
			text = TalkMan.getString(strRef);
		else
			text = caption.getString("AurString_Text");

		if (text.empty())
			text = " ";
	}
}


GUI::GUI() {
}

GUI::~GUI() {
}

void GUI::load(const Common::UString &resref) {
	_name = resref;

	Aurora::GFFFile *gff = 0;
	try {
		gff = loadGFF(resref, Aurora::kFileTypeGUI, MKID_BE('GUI '));

		loadWidget(gff->getTopLevel(), 0);

	} catch (Common::Exception &e) {
		delete gff;

		e.add("Can't load GUI \"%s\"", resref.c_str());
		throw;
	}

	delete gff;
}

void GUI::loadWidget(const Aurora::GFFStruct &strct, Widget *parent) {
	WidgetContext ctx(strct, parent);

	createWidget(ctx);

	addWidget(ctx.widget);

	if (ctx.parent) {
		if (ctx.strct->getString("Obj_Parent") != ctx.parent->getTag())
			throw Common::Exception("Parent's tag != Obj_Parent");

		parent->addChild(*ctx.widget);

		float pX, pY, pZ;
		parent->getPosition(pX, pY, pZ);

		float x = ctx.strct->getDouble("Obj_X") * 100.0 + pX;
		float y = ctx.strct->getDouble("Obj_Y") * 100.0 + pY;
		float z = ctx.strct->getDouble("Obj_Z") * 100.0 + pZ;

		ctx.widget->setPosition(x, y, z);
	} else {
		// We'll ignore these for now, centering the GUI
	}

	initWidget(ctx);

	// Create a caption/label and move the label to its destined position
	WidgetLabel *label = createCaption(ctx);
	if (label && ctx.strct->hasField("Obj_Caption")) {
		const Aurora::GFFStruct &caption = ctx.strct->getStruct("Obj_Caption");

		float alignH = caption.getDouble("AurString_AlignH");
		float alignV = caption.getDouble("AurString_AlignV");

		float labelX = ctx.strct->getDouble("Obj_Label_X") * 100.0;
		float labelY = ctx.strct->getDouble("Obj_Label_Y") * 100.0;
		float labelZ = ctx.strct->getDouble("Obj_Label_Z") * 100.0;

		if (ctx.type != kWidgetTypeLabel) {
			labelX += ctx.widget->getWidth () * alignV;
			labelY += ctx.widget->getHeight() * alignH;

			labelX -= label->getWidth () / 2;
			labelY -= label->getHeight() / 2;
		} else {
			labelY -= label->getHeight();

			labelX -= label->getWidth () * alignH;
			labelY -= label->getHeight() * alignV;
		}

		label->movePosition(labelX, labelY, labelZ);
	}

	// uint32 layer = strct.getUint("Obj_Layer");
	// bool locked = strct.getUint("Obj_Locked") != 0;

	// Go down to the children
	if (ctx.strct->hasField("Obj_ChildList")) {
		const Aurora::GFFList &children = ctx.strct->getList("Obj_ChildList");

		for (Aurora::GFFList::const_iterator c = children.begin(); c != children.end(); ++c)
			loadWidget(**c, ctx.widget);
	}
}

void GUI::createWidget(WidgetContext &ctx) {
	// ...BioWare...
	fixWidgetType(ctx.tag, ctx.type);

	if      (ctx.type == kWidgetTypeFrame)
		ctx.widget = new WidgetFrame(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCloseButton)
		ctx.widget = new WidgetClose(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeCheckBox)
		ctx.widget = new WidgetCheckBox(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypePanel)
		ctx.widget = new WidgetPanel(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeLabel)
		ctx.widget = new WidgetLabel(*this, ctx.tag, ctx.font, ctx.text);
	else if (ctx.type == kWidgetTypeSlider)
		ctx.widget = new WidgetSlider(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeEditBox)
		ctx.widget = new WidgetEditBox(*this, ctx.tag, ctx.model, ctx.font);
	else if (ctx.type == kWidgetTypeButton)
		ctx.widget = new WidgetButton(*this, ctx.tag, ctx.model);
	else if (ctx.type == kWidgetTypeListBox)
		ctx.widget = new WidgetListBox(*this, ctx.tag, ctx.model);
	else
		throw Common::Exception("No such widget type %d", ctx.type);

	NWNModelWidget *widgetModel = dynamic_cast<NWNModelWidget *>(ctx.widget);
	if (widgetModel)
		initWidget(ctx, *widgetModel);

	NWNTextWidget  *widgetText  = dynamic_cast<NWNTextWidget  *>(ctx.widget);
	if (widgetText)
		initWidget(ctx, *widgetText);
}

void GUI::initWidget(WidgetContext &ctx, NWNModelWidget &widget) {
}

void GUI::initWidget(WidgetContext &ctx, NWNTextWidget &widget) {
	if (!ctx.strct->hasField("Obj_Caption"))
		return;

	const Aurora::GFFStruct &caption = ctx.strct->getStruct("Obj_Caption");

	float r = caption.getDouble("AurString_ColorR", 1.0);
	float g = caption.getDouble("AurString_ColorG", 1.0);
	float b = caption.getDouble("AurString_ColorB", 1.0);
	float a = caption.getDouble("AurString_ColorA", 1.0);

	widget.setColor(r, g, b, a);
}

void GUI::initWidget(WidgetContext &ctx) {

	initWidget(*ctx.widget);
}

WidgetLabel *GUI::createCaption(WidgetContext &ctx) {
	if (ctx.type == kWidgetTypeLabel)
		return dynamic_cast<WidgetLabel *>(ctx.widget);

	return createCaption(*ctx.strct, ctx.widget);
}

WidgetLabel *GUI::createCaption(const Aurora::GFFStruct &strct, Widget *parent) {
	if (!strct.hasField("Obj_Caption"))
		return 0;

	const Aurora::GFFStruct &caption = strct.getStruct("Obj_Caption");

	Common::UString font = caption.getString("AurString_Font");

	Common::UString text;
	uint32 strRef = caption.getUint("Obj_StrRef", 0xFFFFFFFF);
	if (strRef != 0xFFFFFFFF)
		text = TalkMan.getString(strRef);

	WidgetLabel *label = new WidgetLabel(*this, parent->getTag() + "#Caption", font, text);

	float pX, pY, pZ;
	parent->getPosition(pX, pY, pZ);
	label->setPosition(pX, pY, pZ);

	float r = caption.getDouble("AurString_ColorR", 1.0);
	float g = caption.getDouble("AurString_ColorG", 1.0);
	float b = caption.getDouble("AurString_ColorB", 1.0);
	float a = caption.getDouble("AurString_ColorA", 1.0);

	label->setColor(r, g, b, a);

	initWidget(*label);

	parent->addChild(*label);
	addWidget(label);

	return label;
}

void GUI::fixWidgetType(const Common::UString &tag, WidgetType &type) {
}

void GUI::initWidget(Widget &widget) {
}

WidgetFrame *GUI::getFrame(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetFrame *frame = dynamic_cast<WidgetFrame *>(widget);
	if (!frame && vital)
		throw Common::Exception("Vital frame widget \"%s\" doesn't exist", tag.c_str());

	return frame;
}

WidgetClose *GUI::getClose(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetClose *closeButton = dynamic_cast<WidgetClose *>(widget);
	if (!closeButton && vital)
		throw Common::Exception("Vital close button widget \"%s\" doesn't exist", tag.c_str());

	return closeButton;
}

WidgetCheckBox *GUI::getCheckBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetCheckBox *checkBox = dynamic_cast<WidgetCheckBox *>(widget);
	if (!checkBox && vital)
		throw Common::Exception("Vital check box widget \"%s\" doesn't exist", tag.c_str());

	return checkBox;
}

WidgetPanel *GUI::getPanel(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetPanel *panel = dynamic_cast<WidgetPanel *>(widget);
	if (!panel && vital)
		throw Common::Exception("Vital panel widget \"%s\" doesn't exist", tag.c_str());

	return panel;
}

WidgetLabel *GUI::getLabel(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetLabel *label = dynamic_cast<WidgetLabel *>(widget);
	if (!label && vital)
		throw Common::Exception("Vital label widget \"%s\" doesn't exist", tag.c_str());

	return label;
}

WidgetSlider *GUI::getSlider(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetSlider *slider = dynamic_cast<WidgetSlider *>(widget);
	if (!slider && vital)
		throw Common::Exception("Vital slider widget \"%s\" doesn't exist", tag.c_str());

	return slider;
}

WidgetEditBox *GUI::getEditBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetEditBox *editBox = dynamic_cast<WidgetEditBox *>(widget);
	if (!editBox && vital)
		throw Common::Exception("Vital edit box widget \"%s\" doesn't exist", tag.c_str());

	return editBox;
}

WidgetButton *GUI::getButton(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetButton *button = dynamic_cast<WidgetButton *>(widget);
	if (!button && vital)
		throw Common::Exception("Vital button widget \"%s\" doesn't exist", tag.c_str());

	return button;
}

WidgetListBox *GUI::getListBox(const Common::UString &tag, bool vital) {
	Widget *widget = getWidget(tag, vital);
	if (!widget)
		return 0;

	WidgetListBox *listBox = dynamic_cast<WidgetListBox *>(widget);
	if (!listBox && vital)
		throw Common::Exception("Vital listBox widget \"%s\" doesn't exist", tag.c_str());

	return listBox;
}

} // End of namespace NWN

} // End of namespace Engines

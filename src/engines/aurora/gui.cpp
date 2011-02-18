/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/gui.cpp
 *  A GUI.
 */

#include "common/error.h"

#include "graphics/graphics.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/model.h"

#include "events/events.h"

#include "engines/aurora/gui.h"
#include "engines/aurora/model.h"

namespace Engines {

Widget::Widget(const Common::UString &model, const Common::UString &font,
               const Common::UString &text) :
	_model(0), _text(0), _state(0),
	_active(false), _visible(false), _disabled(false),
	_x(0.0), _y(0.0), _z(0.0), _textX(0.0), _textY(0.0),
	_r(1.0), _g(1.0), _b(1.0), _a(1.0) {

	// Load model
	if (!model.empty())
		_model = loadModelGUI(model);

	// Load text
	if (!font.empty())
		_text = new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a);
}

Widget::~Widget() {
	delete _text;
	delete _model;
}

const Common::UString &Widget::getTag() const {
	return _tag;
}

bool Widget::isActive() const {
	return _active;
}

bool Widget::isVisible() const {
	return _visible;
}

bool Widget::isDisabled() const {
	return _disabled;
}

void Widget::show() {
	if (_visible)
		// Already shown, nothing to do
		return;

	_visible = true;

	// Show text and model
	if (_model) _model->show();
	if (_text ) _text ->show();

	// Show children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->show();
}

void Widget::hide() {
	if (!_visible)
		// Already hidden, nothing to do
		return;

	_visible = false;

	// Hide text and model
	if (_model) _model->hide();
	if (_text ) _text ->hide();

	// Hide children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->hide();
}

int Widget::getState() const {
	return _state;
}

void Widget::setState(int state) {
	_state = state;
}

void Widget::setPosition(float x, float y, float z) {
	_x = x; _y = y; _z = z;

	if (_model)
		_model->setPosition(_x, _y, _z);
}

void Widget::setTextPosition(float x, float y) {
	_textX = x; _textY = y;

	if (_text)
	_text->setPosition(_x + _textX, _y + _textY);
}

void Widget::move(float x, float y, float z) {
	_x += x;
	_y += y;
	_z += z;

	if (_model)
		_model->setPosition(_x, _y, _z);
}

void Widget::moveText(float x, float y) {
	_textX += x;
	_textY += y;

	if (_text)
	_text->setPosition(_x + _textX, _y + _textY);
}

void Widget::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
}

float Widget::getWidth() const {
	// The widget's bounds are defined either by its model, or
	// if not existent, its text.

	if (_model)
		return _model->getWidth();
	if (_text)
		return _text->getWidth();

	return 0.0;
}

float Widget::getHeight() const {
	// The widget's bounds are defined either by its model, or
	// if not existent, its text.

	if (_model)
		return _model->getHeight();
	if (_text)
		return _text->getHeight();

	return 0.0;
}

float Widget::getTextWidth() const {
	if (_text)
		return _text->getWidth();

	return 0.0;
}

float Widget::getTextHeight() const {
	if (_text)
		return _text->getHeight();

	return 0.0;
}

void Widget::setText(const Common::UString &text) {
	if (_text)
		_text->set(text);
}

void Widget::setTextColor(float r, float g, float b, float a) {
	if (_text)
		_text->setColor(_r = r, _g = g, _b = b, _a = a);
}

void Widget::setDisabled(bool disabled) {
	if (_disabled == disabled)
		// State won't change, nothing to do
		return;

	_disabled = disabled;

	// Shade/Unshade the text
	_a = _disabled ? (_a * 0.6) : (_a / 0.6);
	if (_text)
		_text->setColor(_r, _g, _b, _a);

	// Disable/Enable children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->setDisabled(disabled);
}

void Widget::enter() {
}

void Widget::leave() {
}

void Widget::mouseMove(uint8 state, float x, float y) {
}

void Widget::mouseDown(uint8 state, float x, float y) {
}

void Widget::mouseUp(uint8 state, float x, float y) {
}

void Widget::addChild(Widget &widget) {
	if (&widget != this)
		_children.push_back(&widget);
}

void Widget::addGroupMember(Widget &widget) {
	if (&widget != this)
		_groupMembers.push_back(&widget);
}

bool Widget::hasGroupMembers() const {
	return !_groupMembers.empty();
}

void Widget::signalGroupMemberActive() {
}

void Widget::setActive(bool active) {
	if (_active == active)
		// State won't change, nothing to do
		return;

	_active = active;

	// Signal our group members that we're active now
	if (_active)
		for (std::list<Widget *>::iterator it = _groupMembers.begin(); it != _groupMembers.end(); ++it)
			(*it)->signalGroupMemberActive();
}

void Widget::setTag(const Common::UString &tag) {
	_tag = tag;

	if (_model)
		_model->setTag(tag);
}


GUI::GUI() : _currentWidget(0), _returnCode(0) {
}

GUI::~GUI() {
	// Delete all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		delete *widget;
}

void GUI::show() {
	// Show all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		(*widget)->show();
}

void GUI::hide() {
	// Hide all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		(*widget)->hide();
}

int GUI::run() {
	_returnCode = 0;

	// Run as long as we don't have a return code
	while (_returnCode == 0) {
		// But return immediately when an engine quit was requested
		if (EventMan.quitRequested())
			return 0;

		// Handle events
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if      (event.type == Events::kEventMouseMove)
				mouseMove(event);
			else if (event.type == Events::kEventMouseDown)
				mouseDown(event);
			else if (event.type == Events::kEventMouseUp)
				mouseUp(event);
		}

		// Call the periodic run callback
		callbackRun();

		// Delay for a while
		if (!EventMan.quitRequested() && (_returnCode != 0))
			EventMan.delay(10);
	}

	return _returnCode;
}

void GUI::callbackRun() {
}

void GUI::callbackActive(Widget &widget) {
}

void GUI::addWidget(const Common::UString &tag, Widget *widget) {
	if (!widget)
		return;

	widget->setTag(tag);

	_widgets.push_back(widget);
	_widgetMap[tag] = widget;
}

bool GUI::hasWidget(const Common::UString &tag) const {
	return getWidget(tag) != 0;
}

Widget *GUI::getWidget(const Common::UString &tag, bool vital) {
	// Look up the widget in the map
	WidgetMap::iterator widget = _widgetMap.find(tag);
	if (widget == _widgetMap.end()) {
		if (vital)
			throw Common::Exception("Vital widget \"%s\" doesn't exist", tag.c_str());

		return 0;
	}

	return widget->second;
}

const Widget *GUI::getWidget(const Common::UString &tag, bool vital) const {
	// Look up the widget in the map
	WidgetMap::const_iterator widget = _widgetMap.find(tag);
	if (widget == _widgetMap.end()) {
		if (vital)
			throw Common::Exception("Vital widget \"%s\" doesn't exist", tag.c_str());

		return 0;
	}

	return widget->second;
}

void GUI::declareGroup(const std::list<Widget *> &group) {
	// Mutually add each widget to each widget's group member list

	for (std::list<Widget *>::const_iterator a = group.begin(); a != group.end(); ++a)
		for (std::list<Widget *>::const_iterator b = group.begin(); b != group.end(); ++b)
			(*a)->addGroupMember(**b);
}

int GUI::sub(GUI &gui) {
	// Change the current widget to nothing
	changedWidget(0);

	// Show the sub GUI
	gui.show();
	hide();

	// Run the sub GUI
	int code = gui.run();

	// Hide the sub GUI
	show();
	gui.hide();

	// Update the mouse position
	updateMouse();

	return code;
}

void GUI::updateMouse() {
	// Change the current widget to nothing
	changedWidget(0);

	// Fabricate a mouse move event at the current position
	int x, y, state;
	state = CursorMan.getPosition(x, y);

	Events::Event event;
	event.motion.state = state;
	event.motion.x     = x;
	event.motion.y     = y;

	// Trigger a mouse move
	mouseMove(event);
}

Widget *GUI::getWidgetAt(float x, float y) {
	// Get the GFX object tag at the position
	const Common::UString &tag = GfxMan.getObjectAt(x, y);

	// And return that widget with that tag
	return getWidget(tag);
}

void GUI::changedWidget(Widget *widget) {
	// Leave the now obsolete current widget
	if (_currentWidget)
		_currentWidget->leave();

	// Update the current widget
	_currentWidget = widget;

	// Enter the new current widget
	if (_currentWidget)
		_currentWidget->enter();
}

void GUI::checkWidgetActive(Widget *widget) {
	if (!widget)
		// No widget => not active => return
		return;

	if (!widget->isActive())
		// Not active => return
		return;

	// Call the active callback
	callbackActive(*widget);

	// We now handled that active trigger, reset the active state to false
	widget->setActive(false);
}

void GUI::mouseMove(const Events::Event &event) {
	Widget *widget = getWidgetAt(event.motion.x, event.motion.y);

	if (event.motion.state != 0) {
		// Moves with a mouse button pressed sends move events to the current widget
		mouseMove(_currentWidget, event);

		checkWidgetActive(_currentWidget);
	} else
		// Moves without a mouse button can change the current widget
		if (widget != _currentWidget)
			changedWidget(widget);
}

void GUI::mouseDown(const Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	Widget *widget = getWidgetAt(event.button.x, event.button.y);
	if (widget != _currentWidget)
		changedWidget(widget);

	mouseDown(_currentWidget, event);
}

void GUI::mouseUp(const Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	Widget *widget = getWidgetAt(event.button.x, event.button.y);
	if (widget != _currentWidget) {
		changedWidget(widget);
		return;
	}

	mouseUp(_currentWidget, event);

	checkWidgetActive(_currentWidget);
}

float GUI::toGUIX(int x) {
	float sW = GfxMan.getScreenWidth();

	return (x - (sW / 2.0)) / 100.0;
}

float GUI::toGUIY(int y) {
	float sH = GfxMan.getScreenHeight();

	return ((sH - y) - (sH / 2.0)) / 100.0;
}

void GUI::mouseMove(Widget *widget, const Events::Event &event) {
	if (widget)
		widget->mouseMove(event.motion.state, toGUIX(event.motion.x), toGUIY(event.motion.y));
}

void GUI::mouseDown(Widget *widget, const Events::Event &event) {
	if (widget)
		widget->mouseDown(event.button.state, toGUIX(event.button.x), toGUIY(event.button.y));
}

void GUI::mouseUp(Widget *widget, const Events::Event &event) {
	if (widget)
		widget->mouseUp(event.button.state, toGUIX(event.button.x), toGUIY(event.button.y));
}

} // End of namespace Engines

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

#include "events/events.h"

#include "engines/aurora/gui.h"

/** Time between clicks to still be considered a double-click. */
static const uint32 kDoubleClickTime = 500;

namespace Engines {

Widget::Widget(const Common::UString &tag) : _tag(tag),
	_owner(0), _active(false), _visible(false), _disabled(false), _invisible(false),
	_x(0.0), _y(0.0), _z(0.0),
	_lastClickButton(0), _lastClickTime(0), _lastClickX(0.0), _lastClickY(0.0) {

}

Widget::~Widget() {
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

bool Widget::isInvisible() const {
	return _invisible;
}

void Widget::show() {
	if (_visible)
		// Already shown, nothing to do
		return;

	// Reset the double-click info
	_lastClickButton = 0;
	_lastClickTime   = 0;
	_lastClickX      = 0.0;
	_lastClickY      = 0.0;

	if (!_invisible)
		_visible = true;

	// Show children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->show();
	for (std::list<Widget *>::iterator it = _subWidgets.begin(); it != _subWidgets.end(); ++it)
		(*it)->show();
}

void Widget::hide() {
	if (!_visible)
		// Already hidden, nothing to do
		return;

	_visible = false;

	// Hide children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->hide();
	for (std::list<Widget *>::iterator it = _subWidgets.begin(); it != _subWidgets.end(); ++it)
		(*it)->hide();
}

void Widget::setPosition(float x, float y, float z) {
	_x = x;
	_y = y;
	_z = z;
}

void Widget::movePosition(float x, float y, float z) {
	setPosition(_x + x, _y + y, _z + z);
}

void Widget::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
}

float Widget::getWidth() const {
	return 0.0;
}

float Widget::getHeight() const {
	return 0.0;
}

void Widget::setDisabled(bool disabled) {
	if (_disabled == disabled)
		// State won't change, nothing to do
		return;

	_disabled = disabled;

	// Disable/Enable children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->setDisabled(disabled);
	for (std::list<Widget *>::iterator it = _subWidgets.begin(); it != _subWidgets.end(); ++it)
		(*it)->setDisabled(disabled);
}

void Widget::setInvisible(bool invisible) {
	if (_invisible == invisible)
		// State won't change, nothing to do
		return;

	_invisible = invisible;

	// Invisible the children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->setInvisible(invisible);
	for (std::list<Widget *>::iterator it = _subWidgets.begin(); it != _subWidgets.end(); ++it)
		(*it)->setInvisible(invisible);
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

void Widget::mouseDblClick(uint8 state, float x, float y) {
}

void Widget::subActive(Widget &widget) {
}

void Widget::addSub(Widget &widget) {
	_subWidgets.push_back(&widget);

	widget._owner = this;
}

void Widget::addChild(Widget &widget) {
	if (&widget != this)
		_children.push_back(&widget);
}

void Widget::addGroupMember(Widget &widget) {
	if (&widget != this)
		_groupMembers.push_back(&widget);
}

void Widget::signalGroupMemberActive() {
	_active = false;
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

int GUI::run(int startCode) {
	_startCode  = startCode;
	_returnCode = 0;

	// Run as long as we don't have a return code
	while (_returnCode == 0) {
		// Call the periodic run callback
		callbackRun();
		if (_returnCode != 0)
			break;

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

		// Delay for a while
		if (!EventMan.quitRequested() && (_returnCode != 0))
			EventMan.delay(10);
	}

	return _returnCode;
}

void GUI::callbackRun() {
	_startCode = 0;
}

void GUI::callbackActive(Widget &widget) {
}

void GUI::addWidget(Widget *widget) {
	if (!widget)
		return;

	_widgets.push_back(widget);
	_widgetMap[widget->getTag()] = widget;

	// Add the widget's sub-widgets
	for (std::list<Widget *>::const_iterator it = widget->_subWidgets.begin(); it != widget->_subWidgets.end(); ++it) {
		_widgets.push_back(*it);
		_widgetMap[(*it)->getTag()] = *it;
	}
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

int GUI::sub(GUI &gui, int startCode) {
	// Change the current widget to nothing
	changedWidget(0);

	// Show the sub GUI
	gui.show();
	hide();

	// Run the sub GUI
	int code = gui.run(startCode);

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

	if (!widget->isActive()) {
		// Not active, check if the owner's active instead

		if (widget->_owner)
			checkWidgetActive(widget->_owner);

		return;
	}

	if (widget->_owner) {
		// This is a subwidget, call the owner's active callback
		widget->_owner->subActive(*widget);

		// Check whether the owner's active now
		checkWidgetActive(widget->_owner);

	} else
		// This is a standalone widget, call the GUI's active callback
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
	if ((event.button.button != SDL_BUTTON_LMASK) &&
	    (event.button.button != SDL_BUTTON_WHEELUP) &&
	    (event.button.button != SDL_BUTTON_WHEELDOWN))
		// We only care about left mouse button presses, and the wheel
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

	return (x - (sW / 2.0));
}

float GUI::toGUIY(int y) {
	float sH = GfxMan.getScreenHeight();

	return ((sH - y) - (sH / 2.0));
}

void GUI::mouseMove(Widget *widget, const Events::Event &event) {
	if (widget)
		widget->mouseMove(event.motion.state, toGUIX(event.motion.x), toGUIY(event.motion.y));
}

void GUI::mouseDown(Widget *widget, const Events::Event &event) {
	if (widget)
		widget->mouseDown(event.button.button, toGUIX(event.button.x), toGUIY(event.button.y));
}

void GUI::mouseUp(Widget *widget, const Events::Event &event) {
	if (widget) {
		uint8 button = event.button.button;
		float x      = toGUIX(event.button.x);
		float y      = toGUIY(event.button.y);

		widget->mouseUp(button, x, y);

		uint32 curTime = EventMan.getTimestamp();
		if (((curTime - widget->_lastClickTime) < kDoubleClickTime) &&
		    (widget->_lastClickButton == button) &&
		    (widget->_lastClickX = x) && (widget->_lastClickY = x)) {

			widget->mouseDblClick(button, x, y);
		}

		widget->_lastClickButton = button;
		widget->_lastClickTime   = curTime;
		widget->_lastClickX      = x;
		widget->_lastClickY      = y;
	}
}

} // End of namespace Engines

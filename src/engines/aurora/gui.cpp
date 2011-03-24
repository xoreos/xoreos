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

#include "events/events.h"

#include "graphics/graphics.h"

#include "graphics/aurora/cursorman.h"

#include "engines/aurora/gui.h"
#include "engines/aurora/widget.h"

/** Time between clicks to still be considered a double-click. */
static const uint32 kDoubleClickTime = 500;

namespace Engines {

GUI::GUI() : _currentWidget(0), _returnCode(0), _x(0.0), _y(0.0), _z(0.0) {
}

GUI::~GUI() {
	// Delete all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget) {
		delete *widget;
		*widget = 0;
	}

	_widgets.clear();
}

void GUI::show() {
	// Show all widgets
	for (WidgetList::iterator w = _widgets.begin(); w != _widgets.end(); ++w) {
		Widget &widget = **w;

		if (!widget._owner)
			widget.show();
	}
}

void GUI::hide() {
	// Hide all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		(*widget)->hide();
}

int GUI::run(int startCode) {
	_startCode  = startCode;
	_returnCode = 0;

	EventMan.flushEvents();

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
		while (EventMan.pollEvent(event))
			evaluateEvent(event);

		// Delay for a while
		if (!EventMan.quitRequested() && (_returnCode != 0))
			EventMan.delay(10);
	}

	return _returnCode;
}

int GUI::evaluateEvent(const Events::Event &event) {
	if      (event.type == Events::kEventMouseMove)
		mouseMove(event);
	else if (event.type == Events::kEventMouseDown)
		mouseDown(event);
	else if (event.type == Events::kEventMouseUp)
		mouseUp(event);

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

	assert(!widget->getTag().empty());

	_widgets.push_back(widget);
	_widgetMap[widget->getTag()] = widget;
}

void GUI::removeWidget(Widget *widget) {
	if (!widget)
		return;

	widget->hide();

	for (WidgetList::iterator i = _widgets.begin(); i != _widgets.end(); ++i) {
		if (*i == widget) {
			_widgets.erase(i);
			break;
		}
	}

	WidgetMap::iterator w = _widgetMap.find(widget->getTag());
	if (w != _widgetMap.end())
		_widgetMap.erase(w);

	if (widget->_parent)
		widget->_parent->removeChild(*widget);
	if (widget->_owner)
		widget->_owner->removeSub(*widget);

	for (std::list<Widget *>::iterator i = widget->_groupMembers.begin(); i != widget->_groupMembers.end(); ++i)
		(*i)->removeGroupMember(*widget);

	delete widget;
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
			if (*a && *b)
				(*a)->addGroupMember(**b);
}

int GUI::sub(GUI &gui, int startCode, bool showSelf) {
	// Change the current widget to nothing
	changedWidget(0);

	// Show the sub GUI
	if (startCode == 0)
		gui.show();
	hide();

	// Run the sub GUI
	int code = gui.run(startCode);

	// Hide the sub GUI
	if (showSelf)
		show();
	gui.hide();

	// Update the mouse position
	updateMouse();

	return code;
}

void GUI::setPosition(float x, float y, float z) {
	for (WidgetList::iterator w = _widgets.begin(); w != _widgets.end(); ++w) {
		Widget &widget = **w;

		if (widget._parent)
			continue;

		float wX, wY, wZ;
		widget.getPosition(wX, wY, wZ);

		wX -= _x;
		wY -= _y;
		wZ -= _z;

		widget.setPosition(wX + x, wY + y, wZ + z);
	}

	_x = x;
	_y = y;
	_z = z;
}

void GUI::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
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

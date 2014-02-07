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

/** @file engines/aurora/gui.cpp
 *  A GUI.
 */

#include "common/error.h"
#include "common/threads.h"

#include "events/events.h"
#include "events/requests.h"

#include "graphics/graphics.h"
#include "graphics/renderable.h"
#include "graphics/guiman.h"
#include "graphics/cursorman.h"

#include "engines/aurora/gui.h"
#include "engines/aurora/widget.h"

/** Time between clicks to still be considered a double-click. */
static const uint32 kDoubleClickTime = 500;

namespace Engines {

GUI::GUI() : _visible(false), _currentWidget(0), _returnCode(0), _x(0.0), _y(0.0), _z(0.0) {
}

GUI::~GUI() {
	destroy();
}

void GUI::destroy() {
	if (!Common::isMainThread()) {
		Events::MainThreadFunctor<void> functor(boost::bind(&GUI::destroy, this));

		return RequestMan.callInMainThread(functor);
	}

	// Delete all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget) {
		delete *widget;
		*widget = 0;
	}

	_widgets.clear();
}

void GUI::setVisible(bool visible) {
	LOCK_FRAME();

	for (WidgetList::iterator w = _widgets.begin(); w != _widgets.end(); ++w) {
		Widget &widget = **w;

		if (!widget._owner)
			widget.setVisible(visible);
	}

	_visible = visible;
}

int GUI::run(int startCode) {
	_startCode  = startCode;
	_returnCode = 0;

	EventMan.flushEvents();

	removeFocus();
	updateMouse();

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
			addEvent(event);

		processEventQueue();

		// Delay for a while
		if (!EventMan.quitRequested() && (_returnCode == 0))
			EventMan.delay(10);
	}

	return _returnCode;
}

void GUI::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

int GUI::processEventQueue() {
	bool hasMove = false;

	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if      (e->type == Events::kEventMouseMove)
			hasMove = true;
		else if (e->type == Events::kEventMouseDown)
			mouseDown(*e);
		else if (e->type == Events::kEventMouseUp)
			mouseUp(*e);
		else if (e->type == Events::kEventMouseWheel)
			mouseWheel(*e);
	}

	_eventQueue.clear();

	if (hasMove)
		updateMouse();

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

	const std::vector<Common::UString> ids = widget->getIDs();
	for (std::vector<Common::UString>::const_iterator i = ids.begin(); i != ids.end(); ++i)
		_widgetIDMap[*i] = widget;
}

void GUI::removeWidget(Widget *widget) {
	if (!widget)
		return;

	widget->setVisible(false);

	for (WidgetList::iterator i = _widgets.begin(); i != _widgets.end(); ++i) {
		if (*i == widget) {
			_widgets.erase(i);
			break;
		}
	}

	WidgetMap::iterator w = _widgetMap.find(widget->getTag());
	if (w != _widgetMap.end())
		_widgetMap.erase(w);

	const std::vector<Common::UString> ids = widget->getIDs();
	for (std::vector<Common::UString>::const_iterator i = ids.begin(); i != ids.end(); ++i) {
		WidgetIDMap::iterator wI = _widgetIDMap.find(*i);
		if (wI != _widgetIDMap.end())
			_widgetIDMap.erase(wI);
	}

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
	removeFocus();

	GfxMan.lockFrame();

	// Show the sub GUI
	if (startCode == 0)
		gui.setVisible(true);
	setVisible(false);

	GfxMan.unlockFrame();

	// Run the sub GUI
	int code = gui.run(startCode);

	GfxMan.lockFrame();

	// Hide the sub GUI
	if (showSelf)
		setVisible(true);
	gui.setVisible(false);

	GfxMan.unlockFrame();

	// Update the mouse position
	removeFocus();
	updateMouse();

	return code;
}

void GUI::setPosition(float x, float y, float z) {
	if (_visible)
		GfxMan.lockFrame();

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

	if (_visible)
		GfxMan.unlockFrame();
}

void GUI::getPosition(float &x, float &y, float &z) const {
	x = _x;
	y = _y;
	z = _z;
}

void GUI::removeFocus() {
	changedWidget(0);
}

void GUI::updateMouse() {
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
	float distance;
	Graphics::Renderable *renderable = GUIMan.getRenderableAt(x, y, distance);
	if (!renderable)
		return 0;

	WidgetIDMap::iterator w = _widgetIDMap.find(renderable->getID());
	if (w == _widgetIDMap.end())
		return 0;

	return w->second;
}

void GUI::changedWidget(Widget *widget) {
	LOCK_FRAME();

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
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses.
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

	updateMouse();
}

void GUI::mouseWheel(const Events::Event &event) {
	int x,y;
	SDL_GetMouseState(&x, &y);
	Widget *widget = getWidgetAt(x, y);

	if (widget != _currentWidget)
		changedWidget(widget);

	mouseWheel(_currentWidget, event);
}

static float screenXToGUIX(float x) {
	return x - GfxMan.getScreenWidth() / 2.0;
}

static float screenYToGUIY(float y) {
	return (GfxMan.getScreenHeight() / 2.0) - y;
}

void GUI::mouseMove(Widget *widget, const Events::Event &event) {
	if (!widget)
		return;

	LOCK_FRAME();

	widget->mouseMove(event.motion.state, screenXToGUIX(event.motion.x), screenYToGUIY(event.motion.y));
}

void GUI::mouseDown(Widget *widget, const Events::Event &event) {
	if (!widget)
		return;

	LOCK_FRAME();

	widget->mouseDown(event.button.button, screenXToGUIX(event.button.x), screenYToGUIY(event.button.y));
}

void GUI::mouseUp(Widget *widget, const Events::Event &event) {
	if (!widget)
		return;

	LOCK_FRAME();

	uint8 button = event.button.button;
	float x      = event.button.x;
	float y      = event.button.y;

	widget->mouseUp(button, screenXToGUIX(x), screenYToGUIY(y));

	uint32 curTime = EventMan.getTimestamp();
	if (((curTime - widget->_lastClickTime) < kDoubleClickTime) &&
	    (widget->_lastClickButton == button) &&
	    (widget->_lastClickX == x) && (widget->_lastClickY == y)) {

		widget->mouseDblClick(button, screenXToGUIX(x), screenYToGUIY(y));
	}

	widget->_lastClickButton = button;
	widget->_lastClickTime   = curTime;
	widget->_lastClickX      = x;
	widget->_lastClickY      = y;
}

void GUI::mouseWheel(Widget *widget, const Events::Event &event) {
	if (!widget)
		return;

	LOCK_FRAME();

	widget->mouseWheel(event.wheel.type, event.wheel.x, event.wheel.y);
}

} // End of namespace Engines

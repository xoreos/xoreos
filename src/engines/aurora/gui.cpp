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
 *  A GUI.
 */

#include <cassert>

#include "src/common/system.h"
#include "src/common/error.h"

#include "src/events/events.h"

#include "src/graphics/graphics.h"
#include "src/graphics/renderable.h"

#include "src/graphics/aurora/cursorman.h"

#include "src/engines/aurora/gui.h"
#include "src/engines/aurora/widget.h"
#include "src/engines/aurora/console.h"

/** Time between clicks to still be considered a double-click. */
static const uint32_t kDoubleClickTime = 500;

namespace Engines {

GUI::GUI(Console *console) : _console(console),
	_currentWidget(0), _startCode(kStartCodeNone), _returnCode(kReturnCodeNone),
	_sub(0), _x(0.0f), _y(0.0f), _z(0.0f) {

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
	GfxMan.lockFrame();

	// Show all widgets
	for (WidgetList::iterator w = _widgets.begin(); w != _widgets.end(); ++w) {
		Widget &widget = **w;

		if (!widget._owner)
			widget.show();
	}

	for (std::list<GUI *>::iterator iter = _childGUIs.begin(); iter != _childGUIs.end(); ++iter) {
		(*iter)->show();
	}

	_visible = true;

	GfxMan.unlockFrame();
}

void GUI::hide() {
	GfxMan.lockFrame();

	// Hide all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget)
		(*widget)->hide();

	for (std::list<GUI *>::iterator iter = _childGUIs.begin(); iter != _childGUIs.end(); ++iter) {
		(*iter)->hide();
	}

	_visible = false;

	GfxMan.unlockFrame();
}

uint32_t GUI::run(uint32_t startCode) {
	_startCode  = startCode;
	_returnCode = kReturnCodeNone;

	EventMan.flushEvents();

	removeFocus();
	updateMouse();

	// Run as long as we don't have a return code
	while (_returnCode == kReturnCodeNone) {

		std::list<GUI *> childGUIs = _childGUIs;

		// Call the periodic run callback
		callbackRun();

		// Call the periodic run callback of the child GUIs
		for (std::list<GUI *>::iterator iter = childGUIs.begin(); iter != childGUIs.end(); ++iter) {
			(*iter)->callbackRun();
		}

		if (_returnCode != kReturnCodeNone)
			break;

		// But return immediately when an engine quit was requested
		if (EventMan.quitRequested())
			return kReturnCodeNone;

		// Handle events
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			addEvent(event);
			for (std::list<GUI *>::iterator iter = childGUIs.begin(); iter != childGUIs.end(); ++iter) {
				(*iter)->addEvent(event);
			}
		}

		processEventQueue();
		for (std::list<GUI *>::iterator iter = childGUIs.begin(); iter != childGUIs.end(); ++iter) {
			(*iter)->processEventQueue();
		}

		// If the _returnCode changed of a child GUI we propagate it to the main GUI
		for (std::list<GUI *>::iterator iter = childGUIs.begin(); iter != childGUIs.end(); ++iter) {
			if ((*iter)->_returnCode == kReturnCodeNone)
				continue;

			if ((*iter)->_returnCode != _returnCode) {
				_returnCode = (*iter)->_returnCode;
				(*iter)->_returnCode = 0;
			}
		}

		// Delay for a while
		if (!EventMan.quitRequested() && (_returnCode != kReturnCodeNone))
			EventMan.delay(10);
	}

	return _returnCode;
}

void GUI::abort() {
	_returnCode = kReturnCodeAbort;

	if (_sub)
		_sub->abort();
}

void GUI::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

uint32_t GUI::processEventQueue() {
	bool hasMove = false;

	for (std::list<Events::Event>::const_iterator e = _eventQueue.begin();
	     e != _eventQueue.end(); ++e) {

		if (EventMan.quitRequested() || (_returnCode != kReturnCodeNone)) {
			removeFocus();
			hasMove = false;
			break;
		}

		// Handle debug console, if we have one
		if (_console) {
			if (e->type == Events::kEventKeyDown) {
				if ((e->key.keysym.sym == SDLK_d) && (e->key.keysym.mod & KMOD_CTRL)) {
					removeFocus();
					_console->show();
					continue;
				}
			}

			if (_console->isVisible()) {
				_console->processEvent(*e);

				if (!_console->isVisible())
					hasMove = true;

				continue;
			}
		}

		if      (e->type == Events::kEventMouseMove)
			hasMove = true;
		else if (e->type == Events::kEventMouseDown)
			mouseDown(*e);
		else if (e->type == Events::kEventMouseUp)
			mouseUp(*e);
		else if (e->type == Events::kEventMouseWheel)
			mouseWheel(*e);
		else if (e->type == Events::kEventTextInput)
			textInput(*e);
		else if (e->type == Events::kEventKeyDown)
			keyDown(*e);
		else if (e->type == Events::kEventKeyUp)
			keyUp(*e);
	}

	_eventQueue.clear();

	if (hasMove)
		updateMouse();

	return _returnCode;
}

Common::UString GUI::getName() const {
	return "";
}

void GUI::callbackRun() {
	_startCode = kStartCodeNone;
}

void GUI::callbackActive(Widget &UNUSED(widget)) {
}

void GUI::callbackTextInput(const Common::UString &UNUSED(text)) {
}

void GUI::callbackKeyInput(const Events::Key &UNUSED(key), const Events::EventType &UNUSED(type)) {
}

void GUI::addChild(GUI *gui) {
	_childGUIs.push_back(gui);
	gui->show();
}

void GUI::removeChild(GUI *gui) {
	gui->hide();
	_childGUIs.remove(gui);
}

void GUI::addWidget(Widget *widget) {
	if (!widget)
		return;

	assert(!widget->getTag().empty());
	if (hasWidget(widget->getTag())) {
		if (getWidget(widget->getTag()) != widget) {
			throw Common::Exception("Widget with the same tag, \"%s\", already exists", widget->getTag().c_str());
		} else {
			return;
		}
	}

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

void GUI::clearWidgets() {
	// Delete all widgets
	for (WidgetList::iterator widget = _widgets.begin(); widget != _widgets.end(); ++widget) {
		delete *widget;
		*widget = 0;
	}

	_widgets.clear();
	_widgetMap.clear();
}

bool GUI::empty() {
	return _widgets.empty();
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

uint32_t GUI::sub(GUI &gui, uint32_t startCode, bool showSelf, bool hideSelf) {
	GfxMan.lockFrame();

	_sub = &gui;

	removeFocus();

	// Show the sub GUI
	if (startCode == 0)
		gui.show();

	if (hideSelf)
		hide();

	// Move the gui a bit before the root gui
	if (!hideSelf) {
		float x, y, z;
		getPosition(x, y, z);
		gui.setPosition(x, y, z - 100.0f);
	}

	GfxMan.unlockFrame();

	// Run the sub GUI
	uint32_t code = gui.run(startCode);

	GfxMan.lockFrame();

	// Hide the sub GUI
	if (hideSelf && showSelf)
		show();
	gui.hide();

	// Update the mouse position
	removeFocus();
	updateMouse();

	_sub = 0;

	GfxMan.unlockFrame();

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

void GUI::mouseDown() {
}

void GUI::mouseUp() {
}

Widget *GUI::getWidgetAt(float x, float y) {
	// Get the GFX object at the position
	Graphics::Renderable *obj = GfxMan.getObjectAt(x, y);
	if (!obj)
		return 0;

	// And return the widget with the same tag
	return getWidget(obj->getTag());
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
	if (event.button.button != SDL_BUTTON_LMASK)
	        // We only care about left mouse button presses.
	        return;

	mouseDown();
	Widget *widget = getWidgetAt(event.button.x, event.button.y);
	if (widget != _currentWidget)
		changedWidget(widget);

	mouseDown(_currentWidget, event);
}

void GUI::mouseUp(const Events::Event &event) {
	if (event.button.button != SDL_BUTTON_LMASK)
		// We only care about left mouse button presses
		return;

	mouseUp();
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

void GUI::textInput(const Events::Event &event) {
	callbackTextInput(event.text.text);
}

void GUI::keyDown(const Events::Event &event) {
	callbackKeyInput(Events::Key(event.key.keysym.sym), Events::kEventKeyDown);
}

void GUI::keyUp(const Events::Event &event) {
	callbackKeyInput(Events::Key(event.key.keysym.sym), Events::kEventKeyUp);
}

float GUI::toGUIX(int x) {
	float sW = WindowMan.getWindowWidth();

	return (x - (sW / 2.0f));
}

float GUI::toGUIY(int y) {
	float sH = WindowMan.getWindowHeight();

	return ((sH - y) - (sH / 2.0f));
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
		uint8_t button = event.button.button;
		float x      = toGUIX(event.button.x);
		float y      = toGUIY(event.button.y);

		widget->mouseUp(button, x, y);

		uint32_t curTime = EventMan.getTimestamp();
		if (((curTime - widget->_lastClickTime) < kDoubleClickTime) &&
		    (widget->_lastClickButton == button) &&
		    (widget->_lastClickX == x) && (widget->_lastClickY == y)) {

			widget->mouseDblClick(button, x, y);
		}

		widget->_lastClickButton = button;
		widget->_lastClickTime   = curTime;
		widget->_lastClickX      = x;
		widget->_lastClickY      = y;
	}
}

void GUI::mouseWheel(Widget *widget, const Events::Event &event) {
	if (widget) {
		widget->mouseWheel(event.wheel.type, event.wheel.x, event.wheel.y);
	}

}

} // End of namespace Engines

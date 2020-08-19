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

#ifndef ENGINES_AURORA_GUI_H
#define ENGINES_AURORA_GUI_H

#include <list>
#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/ustring.h"

#include "src/events/types.h"

namespace Engines {

class Widget;
class Console;

/** A GUI. */
class GUI : boost::noncopyable {
public:
	static const uint32_t kStartCodeNone   = 0;
	static const uint32_t kReturnCodeNone  = 0;
	static const uint32_t kReturnCodeAbort = 0xFFFFFFFF;


	GUI(Console *console = 0);
	virtual ~GUI();

	bool isVisible() const { return _visible; }

	virtual void show(); ///< Show the GUI.
	virtual void hide(); ///< Hide the GUI.

	/** Run the GUI. */
	uint32_t run(uint32_t startCode = kStartCodeNone);
	/** Abort the currently running GUI. */
	virtual void abort();

	/** Add a single event for consideration into the GUI event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	uint32_t processEventQueue();

	virtual Common::UString getName() const;


protected:
	Console *_console;

	Widget *_currentWidget; ///< The widget the mouse is currently on.

	uint32_t _startCode;  ///< The GUI's start code.
	uint32_t _returnCode; ///< The GUI's return code.

	GUI *_sub; ///< The currently running sub GUI.

	bool _visible { false };


	/** Add a widget. */
	void addWidget(Widget *widget);
	/** Remove a widget. */
	void removeWidget(Widget *widget);

	/** Clear all widgets. */
	void clearWidgets();
	/** Check if the gui is currently empty. */
	bool empty();

	/** Does this specific widget exist within the GUI? */
	bool hasWidget(const Common::UString &tag) const;

	/** Return a widget in the GUI. */
	Widget *getWidget(const Common::UString &tag, bool vital = false);
	/** Return a widget in the GUI. */
	const Widget *getWidget(const Common::UString &tag, bool vital = false) const;

	/** Put these widgets together into a group. */
	void declareGroup(const std::list<Widget *> &group);

	/** Forcefully remove the focus from the current widget. */
	void removeFocus();

	/** Force an update of the mouse position. */
	void updateMouse();

	/** The mouse state has changed. */
	virtual void mouseUp();
	virtual void mouseDown();

	/** Open up a sub GUI. */
	uint32_t sub(GUI &gui, uint32_t startCode = kStartCodeNone, bool showSelf = true, bool hideSelf = true);

	/** Set the GUI's position. */
	void setPosition(float x, float y, float z);

	/** Get the GUI's position. */
	void getPosition(float &x, float &y, float &z) const;

	/** Callback that's triggered periodically in the run() method. */
	virtual void callbackRun();
	/** Callback that's triggered when a widget was activated. */
	virtual void callbackActive(Widget &widget);
	/** Callback that's triggered when a text input is received. */
	virtual void callbackTextInput(const Common::UString &text);
	/** Callback that's triggered when a key is pressed or released. */
	virtual void callbackKeyInput(const Events::Key &key, const Events::EventType &type);

	/** Add a child GUI object to this GUI. Ownership of the pointer is not transferred. */
	void addChild(GUI *gui);
	/** Remove a child GUI object from this GUI. Pointer will not be deallocated. */
	void removeChild(GUI *gui);

private:
	typedef std::list<Widget *> WidgetList;
	typedef std::map<Common::UString, Widget *> WidgetMap;

	std::list<GUI *> _childGUIs;

	WidgetList _widgets;   ///< All widgets in the GUI.
	WidgetMap  _widgetMap; ///< All widgets in the GUI, index by their tag.

	float _x; ///< The GUI X position.
	float _y; ///< The GUI Y position.
	float _z; ///< The GUI Z position.

	std::list<Events::Event> _eventQueue; ///< The GUI event queue.

	/** Return the widget at that position. */
	Widget *getWidgetAt(float x, float y);

	void changedWidget(Widget *widget);     ///< The current widget has changed.
	void checkWidgetActive(Widget *widget); ///< Check if a widget was activated.

	void mouseMove(const Events::Event &event); ///< Mouse move event triggered.
	void mouseDown(const Events::Event &event); ///< Mouse down event triggered.
	void mouseUp(const Events::Event &event);   ///< Mouse up event triggered.
	void mouseWheel(const Events::Event &event); ///< Mouse wheel event triggered.
	void textInput(const Events::Event &event); ///< Text input event received.
	void keyDown(const Events::Event &event); ///< Key down event triggered.
	void keyUp(const Events::Event &event); ///< Key up event triggeered.

	float toGUIX(int x); // Convert an event X coordinate to a GUI X coordinate
	float toGUIY(int y); // Convert an event Y coordinate to a GUI Y coordinate

	/** Send a mouse move event to the widget. */
	void mouseMove(Widget *widget, const Events::Event &event);
	/** Send a mouse down event to the widget. */
	void mouseDown(Widget *widget, const Events::Event &event);
	/** Send a mouse up event to the widget. */
	void mouseUp(Widget *widget, const Events::Event &event);
	/** Send a mouse wheel event to the widget. */
	void mouseWheel(Widget *widget, const Events::Event &event);

	friend class Widget;
};

} // End of namespace Engines

#endif // ENGINES_AURORA_GUI_H

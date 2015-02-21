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

#include "src/common/ustring.h"

#include "src/events/types.h"

namespace Engines {

class Widget;

/** A GUI. */
class GUI {
public:
	GUI();
	virtual ~GUI();

	virtual void show(); ///< Show the GUI.
	virtual void hide(); ///< Hide the GUI.

	int run(int startCode = 0); ///< Run the GUI.

	/** Add a single event for consideration into the GUI event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	int processEventQueue();


protected:
	Widget *_currentWidget; ///< The widget the mouse is currently on.

	int _startCode;  ///< The GUI's start code.
	int _returnCode; ///< The GUI's return code.

	/** Add a widget. */
	void addWidget(Widget *widget);
	/** Remove a widget. */
	void removeWidget(Widget *widget);

	/** Does this specific widget exist within the GUI? */
	bool hasWidget(const Common::UString &tag) const;

	/** Return a widget in the GUI. */
	Widget *getWidget(const Common::UString &tag, bool vital = false);
	/** Return a widget in the GUI. */
	const Widget *getWidget(const Common::UString &tag, bool vital = false) const;

	/** Put these widgets together into a group. */
	void declareGroup(const std::list<Widget *> &group);

	/** Forcifully remove the focus from the current widget. */
	void removeFocus();

	/** Force an update of the mouse position. */
	void updateMouse();

	/** The mouse state has changed. */
	virtual void mouseUp();
	virtual void mouseDown();

	/** Open up a sub GUI. */
	int sub(GUI &gui, int startCode = 0, bool showSelf = true);

	/** Set the GUI's position. */
	void setPosition(float x, float y, float z);

	/** Get the GUI's position. */
	void getPosition(float &x, float &y, float &z) const;

	/** Callback that's triggered periodically in the run() method. */
	virtual void callbackRun();
	/** Callback that's triggered when a widget was activated. */
	virtual void callbackActive(Widget &widget);

private:
	typedef std::list<Widget *> WidgetList;
	typedef std::map<Common::UString, Widget *> WidgetMap;

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

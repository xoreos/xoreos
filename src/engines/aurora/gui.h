/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/aurora/gui.h
 *  A GUI.
 */

#ifndef ENGINES_AURORA_GUI_H
#define ENGINES_AURORA_GUI_H

#include <list>
#include <map>

#include "common/ustring.h"

#include "events/types.h"

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

	/** Force an update of the mouse position. */
	void updateMouse();

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

	/** Return the widget at that position. */
	Widget *getWidgetAt(float x, float y);

	void changedWidget(Widget *widget);     ///< The current widget has changed.
	void checkWidgetActive(Widget *widget); ///< Check if a widget was activated.

	void mouseMove(const Events::Event &event); ///< Mouse move event triggered.
	void mouseDown(const Events::Event &event); ///< Mouse down event triggered.
	void mouseUp  (const Events::Event &event); ///< Mouse up event triggered.

	float toGUIX(int x); // Convert an event X coordinate to a GUI X coordinate
	float toGUIY(int y); // Convert an event Y coordinate to a GUI Y coordinate

	/** Send a mouse move event to the widget. */
	void mouseMove(Widget *widget, const Events::Event &event);
	/** Send a mouse down event to the widget. */
	void mouseDown(Widget *widget, const Events::Event &event);
	/** Send a mouse up event to the widget. */
	void mouseUp  (Widget *widget, const Events::Event &event);
	/** Send a mouse move event to the widget. */

	friend class Widget;
};

} // End of namespace Engines

#endif // ENGINES_AURORA_GUI_H

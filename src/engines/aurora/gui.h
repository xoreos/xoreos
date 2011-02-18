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

#include "graphics/aurora/types.h"

namespace Engines {

/** A widget in a GUI. */
class Widget {
public:
	Widget(const Common::UString &model, const Common::UString &font,
	       const Common::UString &text);
	virtual ~Widget();

	const Common::UString &getTag() const; ///< Get the widget's tag.

	bool isActive  () const; ///< Was the widget activated?
	bool isVisible () const; ///< Is the widget visible?
	bool isDisabled() const; ///< Is the widget disabled?

	void show(); ///< Show the widget.
	void hide(); ///< Hide the widget.

	int getState() const; ///< Get the widget's state.

	virtual void setState(int state); ///< Set the widget's state.

	/** Set the widget's position. */
	void setPosition(float x, float y, float z);
	/** Set the widget's text's position, relative to the widget's position. */
	void setTextPosition(float x, float y);

	/** Move the widget, relative to its current position. */
	void move(float x, float y, float z);
	/** Move the widget's text, relative to its current position. */
	void moveText(float x, float y);

	/** Get the widget's position. */
	void getPosition(float &x, float &y, float &z) const;

	float getWidth () const; ///< Get the widget's width.
	float getHeight() const; ///< Get the widget's height.

	float getTextWidth () const; ///< Get the widget's text's width.
	float getTextHeight() const; ///< Get the widget's text's height.

	/** Set the widget's text. */
	void setText(const Common::UString &text);
	/** Set the widget's text's color. */
	void setTextColor(float r, float g, float b, float a);

	void setDisabled(bool disabled); ///< Disable/Enable the widget.

	virtual void enter(); ///< The mouse entered the widget.
	virtual void leave(); ///< The mouse left the widget.

	/** The mouse was moved over the widget. */
	virtual void mouseMove(uint8 state, float x, float y);
	/** A mouse button was pressed on the widget. */
	virtual void mouseDown(uint8 state, float x, float y);
	/** A mouse button was released on the widget. */
	virtual void mouseUp  (uint8 state, float x, float y);

	void addChild(Widget &widget);       ///< Add a child to the widget.
	void addGroupMember(Widget &widget); ///< Add a fellow group member to the widget.

protected:
	Common::UString _tag; ///< The widget's tag.

	std::list<Widget *> _children;     ///< The widget's children.
	std::list<Widget *> _groupMembers; ///< The widget's fellow group members.

	Graphics::Aurora::Model *_model; ///< The widget's model.
	Graphics::Aurora::Text  *_text;  ///< The widget's text.

	int _state; ///< The widget's state.

	/** Does the widget have any fellow group members? */
	bool hasGroupMembers() const;

	/** A fellow group member signaled that it is now active. */
	virtual void signalGroupMemberActive();

	void setActive(bool active); ///< The the widget's active state.

private:
	bool _active;   ///< Was the widget activated?
	bool _visible;  ///< Is the widget visible?
	bool _disabled; ///< Is the widget disabled?

	float _x; ///< The widget X position.
	float _y; ///< The widget Y position.
	float _z; ///< The widget Z position.

	float _textX; ///< The widget's text X position.
	float _textY; ///< The widget's text Y position.

	float _r; ///< The widget text's color's red compontent.
	float _g; ///< The widget text's color's green compontent.
	float _b; ///< The widget text's color's blue compontent.
	float _a; ///< The widget text's color's alpha compontent.

	/** Set the widget's tag. */
	void setTag(const Common::UString &tag);

	friend class GUI;
};

/** A GUI. */
class GUI {
public:
	GUI();
	virtual ~GUI();

	virtual void show(); ///< Show the GUI.
	virtual void hide(); ///< Hide the GUI.

	int run(); ///< Run the GUI.

protected:
	Widget *_currentWidget; ///< The widget the mouse is currently on.

	int _returnCode; ///< The GUI's return code.

	/** Add a widget. */
	void addWidget(const Common::UString &tag, Widget *widget);

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
	int sub(GUI &gui);

	/** Callback that's triggered periodically in the run() method. */
	virtual void callbackRun();
	/** Callback that's triggered when a widget was activated. */
	virtual void callbackActive(Widget &widget);

private:
	typedef std::list<Widget *> WidgetList;
	typedef std::map<Common::UString, Widget *> WidgetMap;

	WidgetList _widgets;   ///< All widgets in the GUI.
	WidgetMap  _widgetMap; ///< All widgets in the GUI, index by their tag.

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
};

} // End of namespace Engines

#endif // ENGINES_AURORA_GUI_H

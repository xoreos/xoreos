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
 *  A widget in a GUI.
 */

#ifndef ENGINES_AURORA_WIDGET_H
#define ENGINES_AURORA_WIDGET_H

#include <list>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Engines {

class GUI;

/** A widget in a GUI. */
class Widget : boost::noncopyable {
public:
	Widget(GUI &gui, const Common::UString &tag);
	virtual ~Widget();

	const Common::UString &getTag() const; ///< Get the widget's tag.

	/** Set the widget's tag. */
	virtual void setTag(const Common::UString &tag);

	bool isActive   () const; ///< Was the widget activated?
	bool isVisible  () const; ///< Is the widget visible?
	bool isDisabled () const; ///< Is the widget disabled?
	bool isInvisible() const; ///< Is the widget invisible (never visible)?

	virtual void show(); ///< Show the widget.
	virtual void hide(); ///< Hide the widget.

	Widget *getParent();
	const Widget *getParent() const;

	/** Get the widget's child by tag. */
	Widget *getChild(const Common::UString &childTag);

	/** Set the widget's position. */
	virtual void setPosition(float x, float y, float z);

	/** Move the widget, relative to its current position. */
	virtual void movePosition(float x, float y, float z);

	/** Get the widget's position. */
	virtual void getPosition(float &x, float &y, float &z) const;

	virtual float getWidth () const; ///< Get the widget's width.
	virtual float getHeight() const; ///< Get the widget's height.

	virtual void setDisabled(bool disabled);   ///< Disable/Enable the widget.
	virtual void setInvisible(bool invisible); ///< Make the widget invisible.

	virtual void enter(); ///< The mouse entered the widget.
	virtual void leave(); ///< The mouse left the widget.

	/** The mouse was moved over the widget. */
	virtual void mouseMove(uint8 state, float x, float y);
	/** A mouse button was pressed on the widget. */
	virtual void mouseDown(uint8 state, float x, float y);
	/** A mouse button was released on the widget. */
	virtual void mouseUp(uint8 state, float x, float y);
	/** A mouse wheel was used on the widget. */
	virtual void mouseWheel(uint8 state, int x, int y);

	/** A mouse button was double-clicked on the widget. */
	virtual void mouseDblClick(uint8 state, float x, float y);

	/** A sub-widget was activated. */
	virtual void subActive(Widget &widget);

	void remove(); ///< Remove the widget from the GUI.

	virtual void addSub(Widget &widget);         ///< Add a sub-widget to the widget.
	virtual void addChild(Widget &widget);       ///< Add a child to the widget.
	virtual void addGroupMember(Widget &widget); ///< Add a fellow group member to the widget.

	virtual void removeSub(Widget &widget);         ///< Remove a sub-widget from the widget.
	virtual void removeChild(Widget &widget);       ///< Remove a child from the widget.
	virtual void removeGroupMember(Widget &widget); ///< Remove fellow group member from the widget.

protected:
	GUI *_gui; ///< The GUI the widget belongs to.

	Common::UString _tag; ///< The widget's tag.

	Widget *_parent; ///< The widget's parent, if any.
	Widget *_owner;  ///< The widget's owner, if any.

	std::list<Widget *> _subWidgets;   ///< The widget's sub-widgets.
	std::list<Widget *> _children;     ///< The widget's children.
	std::list<Widget *> _groupMembers; ///< The widget's fellow group members.

	/** A fellow group member signaled that it is now active. */
	virtual void signalGroupMemberActive();

	void setActive(bool active); ///< The widget's active state.

private:
	bool _active;    ///< Was the widget activated?
	bool _visible;   ///< Is the widget visible?
	bool _disabled;  ///< Is the widget disabled?
	bool _invisible; ///< Is the widget invisible (never visible)?

	float _x; ///< The widget X position.
	float _y; ///< The widget Y position.
	float _z; ///< The widget Z position.

	uint8  _lastClickButton;
	uint32 _lastClickTime;
	float  _lastClickX;
	float  _lastClickY;

	friend class GUI;
};

} // End of namespace Engines

#endif // ENGINES_AURORA_WIDGET_H

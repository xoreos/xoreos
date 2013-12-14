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

/** @file engines/aurora/widget.h
 *  A widget in a GUI.
 */

#include "engines/aurora/widget.h"
#include "engines/aurora/gui.h"

namespace Engines {

Widget::Widget(GUI &gui, const Common::UString &tag) : _gui(&gui), _tag(tag),
	_parent(0), _owner(0),
	_active(false), _visible(false), _disabled(false), _invisible(false),
	_x(0.0), _y(0.0), _z(0.0),
	_lastClickButton(0), _lastClickTime(0), _lastClickX(0.0), _lastClickY(0.0) {

}

Widget::~Widget() {
}

const Common::UString &Widget::getTag() const {
	return _tag;
}

void Widget::setTag(const Common::UString &tag) {
	assert(_tag.empty());

	_tag = tag;
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
}

void Widget::hide() {
	if (!_visible)
		// Already hidden, nothing to do
		return;

	_visible = false;

	// Hide children
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it)
		(*it)->hide();
}

Widget *Widget::getParent() {
	return _parent;
}

const Widget *Widget::getParent() const {
	return _parent;
}

void Widget::setPosition(float x, float y, float z) {
	for (std::list<Widget *>::iterator it = _children.begin(); it != _children.end(); ++it) {
		float sX, sY, sZ;
		(*it)->getPosition(sX, sY, sZ);

		sX -= _x;
		sY -= _y;
		sZ -= _z;

		(*it)->setPosition(sX + x, sY + y, sZ + z);
	}

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

void Widget::mouseWheel(uint8 state, int x, int y) {
}

void Widget::mouseDblClick(uint8 state, float x, float y) {
}

void Widget::subActive(Widget &widget) {
}

void Widget::addSub(Widget &widget) {
	_subWidgets.push_back(&widget);

	widget._owner = this;

	_gui->addWidget(&widget);
}

void Widget::addChild(Widget &widget) {
	if (&widget != this)
		_children.push_back(&widget);

	widget._parent = this;
}

void Widget::addGroupMember(Widget &widget) {
	if (&widget != this)
		_groupMembers.push_back(&widget);
}

void Widget::removeSub(Widget &widget) {
	for (std::list<Widget *>::iterator i = _subWidgets.begin(); i != _subWidgets.end(); ++i) {
		if (*i == &widget) {
			_subWidgets.erase(i);
			break;
		}
	}

	widget._owner = 0;
}

void Widget::removeChild(Widget &widget) {
	for (std::list<Widget *>::iterator i = _children.begin(); i != _children.end(); ++i) {
		if (*i == &widget) {
			_children.erase(i);
			break;
		}
	}

	widget._parent = 0;
}

void Widget::removeGroupMember(Widget &widget) {
	for (std::list<Widget *>::iterator i = _groupMembers.begin(); i != _groupMembers.end(); ++i) {
		if (*i == &widget) {
			_groupMembers.erase(i);
			break;
		}
	}
}

void Widget::remove() {
	hide();

	_gui->removeWidget(this);
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

} // End of namespace Engines

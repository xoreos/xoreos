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

/** @file graphics/aurora/cursorman.cpp
 *  The Aurora cursor manager.
 */

#include "common/util.h"
#include "common/error.h"

#include "graphics/aurora/cursorman.h"
#include "graphics/aurora/cursor.h"

#include "graphics/graphics.h"

DECLARE_SINGLETON(Graphics::Aurora::CursorManager)

namespace Graphics {

namespace Aurora {

CursorManager::CursorManager() : _hidden(false), _currentCursor(0) {
}

CursorManager::~CursorManager() {
	clear();
	showCursor();
}

bool CursorManager::isVisible() const {
	return !_hidden;
}

const Common::UString &CursorManager::getCurrentGroup() const {
	return _currentGroup;
}

const Common::UString &CursorManager::getCurrentState() const {
	return _currentState;
}

void CursorManager::clear() {
	Common::StackLock lock(_mutex);

	reset();

	for (CursorMap::iterator g = _cursors.begin(); g != _cursors.end(); ++g)
		for (StateMap::iterator c = g->second.begin(); c != g->second.end(); ++c)
			delete c->second;

	_cursors.clear();

	_currentGroup.clear();
	_currentState.clear();

	_defaultGroup.clear();
	_defaultState.clear();
}

bool CursorManager::add(const Common::UString &name, const Common::UString &group,
                        const Common::UString &state, int hotspotX, int hotspotY) {

	Common::StackLock lock(_mutex);

	Cursor *cursor = 0;
	try {

		cursor = new Cursor(name, hotspotX, hotspotY);

		CursorMap::iterator g = _cursors.find(group);
		if (g == _cursors.end()) {
			std::pair<CursorMap::iterator, bool> result;

			result = _cursors.insert(std::make_pair(group, StateMap()));

			g = result.first;
		}

		std::pair<StateMap::iterator, bool> result;

		result = g->second.insert(std::make_pair(state, cursor));
		if (!result.second)
			throw "Cursor already exists";

	} catch (Common::Exception &e) {
		delete cursor;

		e.add("Could not add cursor \"%s\" as \"%s\":\"%s\"",
		      name.c_str(), group.c_str(), state.c_str());
		Common::printException(e);
		return false;
	}

	return true;
}

void CursorManager::setDefault(const Common::UString &group, const Common::UString &state) {
	Common::StackLock lock(_mutex);

	_defaultGroup = group;
	_defaultState = state;
}

void CursorManager::reset() {
	Common::StackLock lock(_mutex);

	_currentGroup.clear();
	_currentState.clear();

	_currentCursor = 0;

	update();
}

void CursorManager::set() {
	Common::StackLock lock(_mutex);

	set(_defaultGroup, _defaultState);
}

void CursorManager::setGroup(const Common::UString &group) {
	Common::StackLock lock(_mutex);

	set(group, _defaultState);
}

void CursorManager::setState(const Common::UString &state) {
	Common::StackLock lock(_mutex);

	set(_defaultGroup, state);
}

void CursorManager::set(const Common::UString &group, const Common::UString &state) {
	Common::StackLock lock(_mutex);

	_currentGroup = group;
	_currentState = state;

	_currentCursor = find(_currentGroup, _currentState);

	update();
}

uint8 CursorManager::getPosition(int &x, int &y) const {
	return SDL_GetMouseState(&x, &y);
}

void CursorManager::setPosition(int x, int y) {
	GfxMan.setCursorPosition(x, y);
}

void CursorManager::toScreenCoordinates(int x, int y, float &sX, float &sY) {
	const float sW = GfxMan.getScreenWidth();
	const float sH = GfxMan.getScreenHeight();

	sX =       ((float) x)  - (sW / 2.0);
	sY = (sH - ((float) y)) - (sH / 2.0);
}

void CursorManager::fromScreenCoordinates(float sX, float sY, int &x, int &y) {
	const float sW = GfxMan.getScreenWidth();
	const float sH = GfxMan.getScreenHeight();

	x = (int)   sX + (sW / 2.0);
	y = (int) (-sY - (sH / 2.0)) + sH;
}

void CursorManager::hideCursor() {
	Common::StackLock lock(_mutex);

	_hidden = true;

	update();
}

void CursorManager::showCursor() {
	Common::StackLock lock(_mutex);

	_hidden = false;

	update();
}

Cursor *CursorManager::find(Common::UString &group, Common::UString &state, bool def) const {
	// Try to find the group. If not found, look for the default cursor.
	CursorMap::const_iterator g = _cursors.find(group);
	if (g == _cursors.end())
		return def ? 0 : find(group = _defaultGroup, state = _defaultState, true);

	// Try to find the state. If not found, use the first available.
	StateMap::const_iterator c = g->second.find(state);
	if (c == g->second.end()) {
		c = g->second.begin();

		if (c == g->second.end())
			return def ? 0 : find(group = _defaultGroup, state = _defaultState, true);
	}

	group = g->first;
	state = c->first;

	return c->second;
}

void CursorManager::update() {
	GfxMan.setCursor(_hidden ? 0 : _currentCursor);
	GfxMan.showCursor(!_hidden && !_currentCursor);
}

} // End of namespace Aurora

} // End of namespace Graphics

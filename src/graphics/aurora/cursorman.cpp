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
 *  The Aurora cursor manager.
 */

#include <memory>

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/cursor.h"

#include "src/graphics/graphics.h"
#include "src/graphics/windowman.h"

DECLARE_SINGLETON(Graphics::Aurora::CursorManager)

namespace Graphics {

namespace Aurora {

CursorManager::CursorManager() : _hidden(false), _currentCursor(0) {
}

CursorManager::~CursorManager() {
	try {
		clear();
		showCursor();
	} catch (...) {
	}
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
	std::lock_guard<std::recursive_mutex> lock(_mutex);

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

	std::lock_guard<std::recursive_mutex> lock(_mutex);

	try {
		std::unique_ptr<Cursor> cursor = std::make_unique<Cursor>(name, hotspotX, hotspotY);

		CursorMap::iterator g = _cursors.find(group);
		if (g == _cursors.end()) {
			std::pair<CursorMap::iterator, bool> result;

			result = _cursors.insert(std::make_pair(group, StateMap()));

			g = result.first;
		}

		std::pair<StateMap::iterator, bool> result;

		result = g->second.insert(std::make_pair(state, cursor.get()));
		if (!result.second)
			throw Common::Exception("Cursor already exists");

		cursor.release();
	} catch (...) {
		Common::exceptionDispatcherWarning("Could not add cursor \"%s\" as \"%s\":\"%s\"",
		                                   name.c_str(), group.c_str(), state.c_str());
		return false;
	}

	return true;
}

void CursorManager::setDefault(const Common::UString &group, const Common::UString &state) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_defaultGroup = group;
	_defaultState = state;
}

void CursorManager::reset() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_currentGroup.clear();
	_currentState.clear();

	_currentCursor = 0;

	update();
}

void CursorManager::set() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	set(_defaultGroup, _defaultState);
}

void CursorManager::setGroup(const Common::UString &group) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	set(group, _defaultState);
}

void CursorManager::setState(const Common::UString &state) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	set(_defaultGroup, state);
}

void CursorManager::set(const Common::UString &group, const Common::UString &state) {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_currentGroup = group;
	_currentState = state;

	_currentCursor = find(_currentGroup, _currentState);

	update();
}

uint8_t CursorManager::getPosition(int &x, int &y) const {
	return SDL_GetMouseState(&x, &y);
}

void CursorManager::setPosition(int x, int y) {
	WindowMan.setCursorPosition(x, y);
}

void CursorManager::toScreenCoordinates(int x, int y, float &sX, float &sY) {
	const float sW = WindowMan.getWindowWidth();
	const float sH = WindowMan.getWindowHeight();

	sX =       ((float) x)  - (sW / 2.0f);
	sY = (sH - ((float) y)) - (sH / 2.0f);
}

void CursorManager::fromScreenCoordinates(float sX, float sY, int &x, int &y) {
	const float sW = WindowMan.getWindowWidth();
	const float sH = WindowMan.getWindowHeight();

	x = (int)   sX + (sW / 2.0f);
	y = (int) (-sY - (sH / 2.0f)) + sH;
}

void CursorManager::hideCursor() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

	_hidden = true;

	update();
}

void CursorManager::showCursor() {
	std::lock_guard<std::recursive_mutex> lock(_mutex);

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
	WindowMan.showCursor(!_hidden && !_currentCursor);
}

} // End of namespace Aurora

} // End of namespace Graphics

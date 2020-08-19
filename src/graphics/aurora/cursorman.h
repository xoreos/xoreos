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

#ifndef GRAPHICS_AURORA_CURSORMAN_H
#define GRAPHICS_AURORA_CURSORMAN_H

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/singleton.h"
#include "src/common/mutex.h"

namespace Graphics {

namespace Aurora {

class Cursor;

/** The global Aurora cursor manager. */
class CursorManager : public Common::Singleton<CursorManager> {
public:
	CursorManager();
	~CursorManager();

	/** Remove all managed cursors. */
	void clear();

	/** Add a cursor.
	 *
	 *  @param name     The name of the cursor image resource.
	 *  @param group    The tag to identify the cursor group.
	 *  @param state    The state this cursor has within the cursor group.
	 *  @param hotspotX The X coordinate of the hotspot.
	 *  @param hotspotY The Y coordinate of the hotspot.
	 *
	 *  @return true if the cursor could be added to the manager.
	 */
	bool add(const Common::UString &name, const Common::UString &group,
	         const Common::UString &state = "", int hotspotX = -1, int hotspotY = -1);

	/** Register this cursor as the default cursor. */
	void setDefault(const Common::UString &group, const Common::UString &state = "");

	/** Set the cursor to a specific group and state. */
	void set(const Common::UString &group, const Common::UString &state = "");

	/** Set the cursor to another group, using the current state name. */
	void setGroup(const Common::UString &group);
	/** Set the cursor to a different state within the current group. */
	void setState(const Common::UString &state);

	/** Set the cursor to the registered default. */
	void set();

	/** Reset the cursor to the system's default. */
	void reset();

	void hideCursor(); ///< Hide the cursor completely.
	void showCursor(); ///< Unhide the cursor.

	/** Get the current cursor position. */
	uint8_t getPosition(int &x, int &y) const;
	/** Move the cursor to a specific position. */
	void  setPosition(int  x, int  y);

	/** Convert cursor position to screen position. */
	void toScreenCoordinates(int x, int y, float &sX, float &sY);
	/** Convert screen position to cursor position. */
	void fromScreenCoordinates(float sX, float sY, int &x, int &y);

	bool isVisible() const; ///< Is a cursor current visible?

	/** Return the current group. */
	const Common::UString &getCurrentGroup() const;
	/** Return the current state. */
	const Common::UString &getCurrentState() const;

private:
	typedef std::map<Common::UString, Cursor *> StateMap;
	typedef std::map<Common::UString, StateMap> CursorMap;

	CursorMap _cursors;

	Common::UString _currentGroup;
	Common::UString _currentState;

	Common::UString _defaultGroup;
	Common::UString _defaultState;

	bool _hidden;

	Cursor *_currentCursor;

	std::recursive_mutex _mutex;


	Cursor *find(Common::UString &group, Common::UString &state, bool def = false) const;

	void update();
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the cursor manager. */
#define CursorMan Graphics::Aurora::CursorManager::instance()

#endif // GRAPHICS_AURORA_CURSORMAN_H

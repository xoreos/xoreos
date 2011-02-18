/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/cursorman.h
 *  The Aurora cursor manager.
 */

#ifndef GRAPHICS_AURORA_CURSORMAN_H
#define GRAPHICS_AURORA_CURSORMAN_H

#include <map>

#include "common/types.h"
#include "common/ustring.h"
#include "common/singleton.h"
#include "common/mutex.h"

namespace Graphics {

namespace Aurora {

class Cursor;

/** The global Aurorar cursor manager. */
class CursorManager : public Common::Singleton<CursorManager> {
public:
	CursorManager();
	~CursorManager();

	void clear();

	/** Add a cursor with that tag and enabled state. */
	void add(const Common::UString &name, const Common::UString &tag, bool enabled);

	/** Set the cursor to a specific resource.
	 *
	 *  Will load the cursor if necessary.
	 *
	 *  @param name The name of the cursor resource.
	 *              If empty, reset to the system's default cursor.
	 */
	void set(const Common::UString &name = "");
	/** Set the cursor to a specific tag. */
	void set(const Common::UString &tag, bool enabled);

	uint8 getPosition(int &x, int &y) const;
	void  setPosition(int  x, int  y);

	void hideCursor();
	void showCursor();

private:
	/** A cursor tag. */
	struct Tag {
		Common::UString tag;
		bool enabled;

		Tag(const Common::UString &t, bool e);

		bool operator<(const Tag &t) const;
	};

	typedef std::map<Common::UString, Cursor *> CursorMap;
	typedef std::map<Tag, Cursor *> TagMap;

	CursorMap _cursors;
	TagMap _tags;

	bool _hidden;
	Cursor *_currentCursor;

	Common::Mutex _mutex;

	Cursor *get(const Common::UString &name);
	void set(Cursor *cursor);
};

} // End of namespace Aurora

} // End of namespace Graphics

/** Shortcut for accessing the cursor manager. */
#define CursorMan Graphics::Aurora::CursorManager::instance()

#endif // GRAPHICS_AURORA_CURSORMAN_H

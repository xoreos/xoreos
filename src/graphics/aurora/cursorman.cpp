/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

CursorManager::Tag::Tag(const Common::UString &t, bool e) : tag(t), enabled(e) {
}

bool CursorManager::Tag::operator<(const Tag &t) const {
	return (tag < t.tag) || (enabled < t.enabled);
}


CursorManager::CursorManager() : _hidden(false), _currentCursor(0) {
}

CursorManager::~CursorManager() {
	for (CursorMap::iterator cursor = _cursors.begin(); cursor != _cursors.end(); ++cursor)
		delete cursor->second;
}

void CursorManager::clear() {
	Common::StackLock lock(_mutex);

	set();

	for (CursorMap::iterator cursor = _cursors.begin(); cursor != _cursors.end(); ++cursor)
		delete cursor->second;

	_cursors.clear();
	_tags.clear();
}

void CursorManager::add(const Common::UString &name,
		const Common::UString &tag, bool enabled) {

	Tag t(tag, enabled);
	if (_tags.find(t) != _tags.end())
		throw Common::Exception("Cursor \"%s\":%d already exists", tag.c_str(), enabled);

	Cursor *c = get(name);
	_tags.insert(std::make_pair(t, c));
}

void CursorManager::set(const Common::UString &name) {
	if (!name.empty())
		_currentCursor = get(name);
	else
		_currentCursor = 0;

	if (!_hidden) {
		set(_currentCursor);
		GfxMan.showCursor(_currentCursor == 0);
	}
}

void CursorManager::set(const Common::UString &tag, bool enabled) {
	TagMap::const_iterator t = _tags.find(Tag(tag, enabled));
	if (t == _tags.end())
		throw Common::Exception("No such cursor \"%s\":%d", tag.c_str(), enabled);

	_currentCursor = t->second;

	if (!_hidden) {
		set(_currentCursor);
		GfxMan.showCursor(_currentCursor == 0);
	}
}

void CursorManager::set(Cursor *cursor) {
	GfxMan.setCursor(cursor);
}

Cursor *CursorManager::get(const Common::UString &name) {
	CursorMap::iterator cursor = _cursors.find(name);
	if (cursor != _cursors.end())
		return cursor->second;

	return _cursors.insert(std::make_pair(name, new Cursor(name))).first->second;
}

void CursorManager::hideCursor() {
	_hidden = true;

	GfxMan.setCursor();
	GfxMan.showCursor(false);
}

void CursorManager::showCursor() {
	_hidden = false;

	if (_currentCursor)
		GfxMan.setCursor(_currentCursor);
	else
		GfxMan.showCursor(true);
}

} // End of namespace Aurora

} // End of namespace Graphics

/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/charstore.h
 *  NWN character store.
 */

#ifndef ENGINES_NWN_CHARSTORE_H
#define ENGINES_NWN_CHARSTORE_H

#include <list>
#include <map>

#include "common/singleton.h"
#include "common/thread.h"
#include "common/ustring.h"
#include "common/mutex.h"

#include "engines/nwn/types.h"
#include "engines/nwn/creature.h"

namespace Engines {

namespace NWN {

/** The global NWN character store, holding all available player characters. */
class CharacterStore : public Common::Singleton<CharacterStore>, Common::Thread {
public:
	CharacterStore();
	~CharacterStore();

	void clear();

	void addDirectory(const Common::UString &dir);

	void sync();

	uint getCharacters(std::list<CharacterID> &chars);

	CharacterID addCharacter(Creature &character);
	void removeCharacter(CharacterID &id);

private:
	typedef std::map<Common::UString, uint> CountMap;
	typedef std::list<Common::UString> TodoList;

	CountMap _countMap;

	CharacterID::CharMap _charMap;

	TodoList _todoList;

	Common::Mutex _mutex;

	Common::Semaphore _started;
	Common::Mutex _finished;

	bool _isRunning;

	void threadMethod();

	void index();

	uint &getCount(const Common::UString &fullName);

	uint parseFileNumber(const Common::UString &file);

	CharacterKey    createCharacterKey(const Creature &character, uint n);
	Common::UString createFilename(const Creature &character, uint n);

	void save(const Creature &character, const Common::UString &filename);

	void check(const Creature &character);
};

} // End of namespace NWN

} // End of namespace Engines

/** Shortcut for accessing the 2da registry. */
#define CharStore ::Engines::NWN::CharacterStore::instance()

#endif // ENGINES_NWN_CHARSTORE_H

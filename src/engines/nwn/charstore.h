/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
};

} // End of namespace NWN

} // End of namespace Engines

/** Shortcut for accessing the 2da registry. */
#define CharStore ::Engines::NWN::CharacterStore::instance()

#endif // ENGINES_NWN_CHARSTORE_H

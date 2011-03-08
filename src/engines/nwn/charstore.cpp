/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/charstore.cpp
 *  NWN character store.
 */

#include "common/error.h"
#include "common/file.h"
#include "common/filepath.h"
#include "common/filelist.h"

#include "engines/nwn/charstore.h"

DECLARE_SINGLETON(Engines::NWN::CharacterStore)

namespace Engines {

namespace NWN {

CharacterStore::CharacterStore() : _started(0), _isRunning(false) {
}

CharacterStore::~CharacterStore() {
	clear();
}

void CharacterStore::clear() {
	sync();

	Common::StackLock lock(_mutex);

	for (CharacterID::CharMap::iterator c = _charMap.begin(); c != _charMap.end(); ++c)
		delete c->second;

	_charMap.clear();
	_todoList.clear();
}

void CharacterStore::addDirectory(const Common::UString &dir) {
	if (dir.empty())
		return;

	Common::StackLock lock(_mutex);

	Common::FileList dirList;
	dirList.addDirectory(dir);

	dirList.getFileNames(_todoList);

	if (_isRunning)
		// Thread is already running, we're finished
		return;

	if (!createThread())
		throw Common::Exception("Failed creating character store thread");

	_started.lock();
}

void CharacterStore::sync() {
	_mutex.lock();

	if (_todoList.empty())
		// Todo list is empty, we are synced
		return;

	_mutex.unlock();

	// Wait for the finish signal
	_finished.lock();

	assert(_todoList.empty());

	// Reset the mutex
	_finished.unlock();
}

uint CharacterStore::getCharacters(std::list<CharacterID> &chars) {
	sync();

	Common::StackLock lock(_mutex);

	uint n = 0;
	for (CharacterID::CharMap::iterator c = _charMap.begin(); c != _charMap.end(); ++c) {
		chars.push_back(CharacterID(c));
		++n;
	}

	return n;
}

CharacterID CharacterStore::addCharacter(Creature &character) {
	sync();

	Common::StackLock lock(_mutex);

	uint &count = getCount(character.getFullName());

	count++;

	CharacterKey    charKey  = createCharacterKey(character, count);
	Common::UString fileName = createFilename(character, count);

	save(character, fileName);

	std::pair<CharacterID::CharMap::iterator, bool> result;
	result = _charMap.insert(std::make_pair(charKey, &character));

	assert(result.second);

	CharacterID id;

	id.set(result.first);

	return id;
}

void CharacterStore::removeCharacter(CharacterID &id) {
	sync();

	if (id.empty())
		return;

	Common::StackLock lock(_mutex);

	// Delete the character
	delete id._char->second;

	// Remove the character from the character map
	_charMap.erase(id._char);

	// Clear the ID
	id.clear();
}

void CharacterStore::index() {
	Common::StackLock lock(_mutex);

	if (_todoList.empty())
		// No new file to index
		return;

	Common::UString fileName = _todoList.front();

	_todoList.pop_front();

	if (Common::FilePath::getExtension(fileName) != ".bic")
		// Not a player character
		return;

	Creature *character = 0;
	try {
		Common::File file;
		if (!file.open(fileName))
			throw Common::Exception("No such file");

		character = new Creature;
		character->loadCharacter(file);

		check(*character);

	} catch (Common::Exception &e) {
		delete character;

		e.add("Can't load character \"%s\"", Common::FilePath::getStem(fileName).c_str());
		printException(e, "WARNING: ");
		return;
	}

	uint fileNumber = parseFileNumber(Common::FilePath::getStem(fileName));

	CharacterKey charKey = createCharacterKey(*character, fileNumber);

	uint &count = getCount(character->getFullName());
	count = MAX(count, fileNumber);

	std::pair<CharacterID::CharMap::iterator, bool> result;
	result = _charMap.insert(std::make_pair(charKey, character));

	assert(result.second);
}

uint &CharacterStore::getCount(const Common::UString &fullName) {
	CountMap::iterator count = _countMap.find(fullName);
	if (count == _countMap.end()) {
		std::pair<CountMap::iterator, bool> result;

		result = _countMap.insert(std::make_pair(fullName, 0));

		count = result.first;
	}

	return count->second;
}

uint CharacterStore::parseFileNumber(const Common::UString &file) {
	assert(!file.empty());

	Common::UString::iterator numberIt;
	for (numberIt = --file.end(); numberIt != file.begin(); --numberIt) {
		uint32 c = *numberIt;

		if ((c < '0') || (c > '9')) {
			++numberIt;
			break;
		}
	}

	if (numberIt == file.end())
		return 0;

	uint n;
	int matched = sscanf(Common::UString(numberIt, file.end()).c_str(), "%d", &n);

	assert(matched == 1);
	return n;
}

CharacterKey CharacterStore::createCharacterKey(const Creature &character, uint n) {
	return CharacterKey(character.getFullName(), n);
}

Common::UString CharacterStore::createFilename(const Creature &character, uint n) {
	assert(n <= 99999);

	Common::UString fileName;
	Common::UString nString;
	if (n > 0)
		nString = Common::UString::sprintf("%d", n);

	uint nameLength = 16 - nString.size();
	const Common::UString &fullName = character.getFullName();
	for (Common::UString::iterator f = fullName.begin(); f != fullName.end() && nameLength > 0;
       ++f, --nameLength) {

		uint32 c = Common::UString::tolower(*f);

		if (c != ' ')
			fileName += c;
	}

	return fileName + nString + ".bic";
}

void CharacterStore::threadMethod() {
	_isRunning = true;

	_finished.lock();
	_started.unlock();

	while (!_killThread) {
		index();

		if (_todoList.empty())
			break;
	}

	_isRunning = false;
	_finished.unlock();
}

void CharacterStore::save(const Creature &character, const Common::UString &filename) {
	warning("TODO: CharacterStore::save()");
}

void CharacterStore::check(const Creature &character) {
	if (!character.isPC())
		throw Common::Exception("Is not a player character");
	if (character.isDM())
		throw Common::Exception("Is a dungeon master");
}

} // End of namespace NWN

} // End of namespace Engines

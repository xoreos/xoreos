/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/types.cpp
 *  Basic NWN type definitions.
 */

#include "engines/nwn/types.h"

namespace Engines {

namespace NWN {

CharacterKey::CharacterKey(const Common::UString &na, uint nu) : name(na), number(nu) {
}

bool CharacterKey::operator<(const CharacterKey &key) const {
	int cmp = name.strcmp(key.name);

	if (cmp < 0)
		return true;
	if (cmp > 0)
		return false;

	return number < key.number;
}


CharacterID::CharacterID() : _empty(true) {
}

CharacterID::~CharacterID() {
}

CharacterID::CharacterID(CharMap::iterator c) : _empty(false), _char(c) {
}

bool CharacterID::empty() const {
	return _empty;
}

uint CharacterID::getNumber() const {
	return _char->first.number;
}

const Creature &CharacterID::operator*() const {
	assert(!_empty);

	return *_char->second;
}

const Creature *CharacterID::operator->() const {
	assert(!_empty);

	return _char->second;
}

void CharacterID::set(CharMap::iterator c) {
	_char  = c;
	_empty = false;
}

void CharacterID::clear() {
	_empty = true;
}

} // End of namespace NWN

} // End of namespace Engines

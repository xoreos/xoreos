/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/stringmap.cpp
 *  A map to quickly match strings from a list.
 */

#include <cstring>

#include "common/stringmap.h"
#include "common/util.h"

namespace Common {

StringMap::StringMap(const char **strings, int count, bool onlyFirstWord) : _onlyFirstWord(onlyFirstWord) {
	for (int i = 0; i < count; i++)
		_map.insert(std::make_pair(Common::UString(strings[i]), i));
}

int StringMap::find(const char *str, const char **match) const {
	Common::UString sStr = str;

	if (_onlyFirstWord) {
		Common::UString::iterator space = sStr.findFirst(' ');
		if (space != sStr.end())
			sStr.truncate(space);
	}

	StrMap::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str + s->first.size() + 1;
	return s->second;
}

int StringMap::find(const Common::UString &str, const char **match) const {
	Common::UString sStr = str;

	if (_onlyFirstWord) {
		Common::UString::iterator space = sStr.findFirst(' ');
		if (space != sStr.end())
			sStr.truncate(space);
	}

	StrMap::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str.c_str() + s->first.size() + 1;
	return s->second;
}

} // End of namespace Common

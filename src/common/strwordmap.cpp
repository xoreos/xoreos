/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strwordmap.cpp
 *  A map to quickly match word strings from a list.
 */

#include <cstring>

#include "common/strwordmap.h"
#include "common/util.h"

namespace Common {

StrWordMap::StrWordMap(const char **strings, int count) {
	for (int i = 0; i < count; i++)
		_map.insert(std::make_pair(Common::UString(strings[i]), i));
}

int StrWordMap::find(const char *str, const char **match) const {
	Common::UString sStr = str;

	Common::UString::iterator space = sStr.findFirst(' ');
	if (space != sStr.end())
		sStr.truncate(space);

	std::map<Common::UString, int>::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str + s->first.size() + 1;
	return s->second;
}

int StrWordMap::find(const Common::UString &str, const char **match) const {
	Common::UString sStr = str;

	Common::UString::iterator space = sStr.findFirst(' ');
	if (space != sStr.end())
		sStr.truncate(space);

	std::map<Common::UString, int>::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str.c_str() + s->first.size() + 1;
	return s->second;
}

} // End of namespace Common

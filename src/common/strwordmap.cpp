/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strwordmap.h
 *  A map to quickly match word strings from a list.
 */

#include <cstring>

#include "common/strwordmap.h"
#include "common/util.h"

namespace Common {

StrWordMap::StrWordMap(const char **strings, int count) {
	for (int i = 0; i < count; i++)
		_map.insert(std::make_pair(std::string(strings[i]), i));
}

int StrWordMap::find(const char *str, const char **match) const {
	std::string sStr = str;

	const char *space = std::strchr(str, ' ');
	if (space)
		sStr.resize(space - str);

	std::map<std::string, int>::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str + s->first.size() + 1;
	return s->second;
}

int StrWordMap::find(const std::string &str, const char **match) const {
	std::string sStr = str;

	const char *space = std::strchr(str.c_str(), ' ');
	if (space)
		sStr.resize(space - str.c_str());

	std::map<std::string, int>::const_iterator s = _map.find(sStr);
	if (s == _map.end())
		return -1;

	if (match)
		*match = str.c_str() + s->first.size() + 1;
	return s->second;
}

} // End of namespace Common

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

#ifndef COMMON_STRWORDMAP_H
#define COMMON_STRWORDMAP_H

#include <map>

#include "common/ustring.h"

namespace Common {

class StrWordMap {
public:
	/** Build a string map to match a list of word strings against. */
	StrWordMap(const char **strings, int count);

	/** Match a string against the map.
	 *
	 *  @param  str The string to match against.
	 *  @param  match If != 0, the position after the match will be stored here.
	 *  @return The index of the matched string in the original list, or -1 if not found.
	 */
	int find(const char *str, const char **match) const;

	/** Match a string against the map.
	 *
	 *  @param  str The string to match against.
	 *  @param  match If != 0, the position after the match will be stored here.
	 *  @return The index of the matched string in the original list, or -1 if not found.
	 */
	int find(const Common::UString &str, const char **match) const;

private:
	std::map<Common::UString, int> _map; ///< The map.
};

} // End of namespace Common

#endif // COMMON_STRWORDMAP_H

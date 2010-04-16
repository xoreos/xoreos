/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strutil.h
 *  Utility templates and functions for working with strings.
 */

#ifndef COMMON_STRUTIL_H
#define COMMON_STRUTIL_H

#include <string>
#include <sstream>

#include "common/types.h"

namespace Common {

class SeekableReadStream;

/** Replace all occurences of one character in a string with another. */
void replaceAll(std::string &str, char what, char with);

/** Convert/Parse a string into different types. */
template<typename T> bool stringConvert(const std::string &str, T &v) {
	std::stringstream ss(str);

	if ((ss >> v).fail())
		return false;

	return true;
}

template<bool &> bool stringConvert(const std::string &str, bool &v) {
	int i;
	if (!stringConvert(str, i))
		return false;

	v = (i == 1);
	return true;
}

template<std::string &> bool stringConvert(const std::string &str, std::string &v) {
	v = str;
	return true;
}

std::string sprintf(const char *s, ...);

/** Print a quick hex dump of the given data. */
void printDataHex(Common::SeekableReadStream &stream);
/** Print a quick hex dump of the given data. */
void printDataHex(const byte *data, uint32 size);

} // End of namespace Common

#endif // COMMON_STRUTIL_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/ustring.cpp
 *  Unicode string handling.
 */

#include "common/ustring.h"

namespace Common {

UString::UString(const UString &str) {
	*this = str;
}

UString::UString(const std::string &str) {
	*this = str;
}

UString::UString(const char *str) {
	*this = str;
}

UString::~UString() {
}

UString &UString::operator=(const UString &str) {
	_string = str._string;

	return *this;
}

UString &UString::operator=(const std::string &str) {
	// Create the string for the valid utf8 portion of the string.
	// Will work for clean non-extended ASCII strings.
	_string = std::string(str.begin(), utf8::find_invalid(str.begin(), str.end()));

	return *this;
}

UString &UString::operator=(const char *str) {
	*this = std::string(str);

	return *this;
}

uint32 UString::size() const {
	// Calculate the "distance" in characters from the beginning and end
	return utf8::distance(_string.begin(), _string.end());
}

bool UString::empty() const {
	return _string.empty();
}

const char *UString::c_str() const {
	return _string.c_str();
}

UString::iterator UString::begin() const {
	return iterator(_string.begin(), _string.begin(), _string.end());
}

UString::iterator UString::end() const {
	return iterator(_string.end(), _string.begin(), _string.end());
}

} // End of namespace Common

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

#include <iconv.h>

#include "common/ustring.h"
#include "common/error.h"
#include "common/singleton.h"
#include "common/util.h"


namespace Common {

class ConversionManager : public Singleton<ConversionManager> {
public:
	ConversionManager() : _fromLatin9((iconv_t) -1) {
		_fromLatin9 = iconv_open("UTF-8", "ISO-8859-15");
		if (_fromLatin9 == ((iconv_t) -1))
			throw Common::Exception("Failed to initialize ISO-8859-15 -> UTF-8 conversion");
	}

	~ConversionManager() {
		if (_fromLatin9 != ((iconv_t) -1))
			iconv_close(_fromLatin9);
	}

	std::string fromLatin9(byte *data, uint32 n) {
		if (_fromLatin9 == ((iconv_t) -1))
			throw Common::Exception("No iconv context");

		size_t inBytes  = n;
		size_t outBytes = n * 4; // Should be enough;

		byte *convData = new byte[outBytes];
		byte *outBuf = convData;

		// Reset the converter's state
		iconv(_fromLatin9, 0, 0, 0, 0);

		// Convert
		if (iconv(_fromLatin9, (char **) &data, &inBytes, (char **) &outBuf, &outBytes) == ((size_t) -1))
			warning("Failed completely converting a latin9 string");

		// And this should be our converted string
		std::string convStr((const char *) convData, (n * 4) - outBytes);

		delete[] convData;

		return convStr;
	}

private:
	iconv_t _fromLatin9;
};

}

#define ConvMan Common::ConversionManager::instance()

DECLARE_SINGLETON(Common::ConversionManager)

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

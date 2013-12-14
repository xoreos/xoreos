/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/ustring.h
 *  Unicode string handling.
 */

#ifndef COMMON_USTRING_H
#define COMMON_USTRING_H

#include <string>
#include <sstream>
#include <vector>

#include <boost/functional/hash.hpp>

#include "common/types.h"

#include "utf8cpp/utf8.h"

namespace Common {

class SeekableReadStream;

/** A class holding an UTF-8 string.
 *
 *  WARNING:
 *  Copy constructors and assignment operators copying from std::string and
 *  const char * assume the data is either clean (non-extended) ASCII or
 *  already UTF-8.
 */
class UString {
public:
	typedef utf8::iterator<std::string::const_iterator> iterator;

	// Case insensitive compare
	struct iless : std::binary_function<UString, UString, bool> {
		bool operator() (const UString &str1, const UString &str2) const {
			return str1.lessIgnoreCase(str2);
		}
	};

	UString(const UString &str);
	UString(const std::string &str);
	UString(const char *str = "");
	UString(const char *str, int n);
	UString(iterator sBegin, iterator sEnd);
	~UString();

	UString &operator=(const UString &str);
	UString &operator=(const std::string &str);
	UString &operator=(const char *str);

	bool operator==(const UString &str) const;
	bool operator!=(const UString &str) const;
	bool operator<(const UString &str) const;
	bool operator>(const UString &str) const;

	UString operator+(const UString &str) const;
	UString operator+(const std::string &str) const;
	UString operator+(const char *str) const;
	UString operator+(uint32 c) const;

	UString &operator+=(const UString &str);
	UString &operator+=(const std::string &str);
	UString &operator+=(const char *str);
	UString &operator+=(uint32 c);

	int strcmp(const UString &str) const;
	int stricmp(const UString &str) const;

	bool equals(const UString &str) const;
	bool equalsIgnoreCase(const UString &str) const;

	bool less(const UString &str) const;
	bool lessIgnoreCase(const UString &str) const;

	/** Swap the contents of the string with this string's. */
	void swap(UString &str);

	/** Clear the string's contents. */
	void clear();

	/** Return the size of the string, in characters. */
	uint32 size() const;

	/** Is the string empty? */
	bool empty() const;

	/** Return the (utf8 encoded) string data. */
	const char *c_str() const;

	iterator begin() const;
	iterator end() const;

	iterator findFirst(uint32 c) const;

	bool beginsWith(const UString &with) const;
	bool endsWith(const UString &with) const;

	bool contains(const UString &what) const;

	void truncate(const iterator &it);
	void truncate(uint32 n);

	void trimLeft();
	void trimRight();
	void trim();

	/** Replace all occurences of a character with another character. */
	void replaceAll(uint32 what, uint32 with);

	/** Convert the string to lowercase. */
	void tolower();
	/** Convert the string to uppercase. */
	void toupper();

	/** Convert an iterator into a numerical position. */
	iterator getPosition(uint32 n)    const;
	/** Convert a numerical position into an iterator. */
	uint32   getPosition(iterator it) const;

	/** Insert character c in front of this position. */
	void insert(iterator pos, uint32 c);
	/** Insert character c in front of this position. */
	void insert(iterator pos, const char* c);
	/** Replace the character at this position with c. */
	void replace(iterator pos, uint32 c);
	/** Replace the character at this position with c. */
	void replace(iterator pos, const char* c);
	/** Erase the character within this range. */
	void erase(iterator from, iterator to);
	/** Erase the character at this position. */
	void erase(iterator pos);

	void split(iterator splitPoint, UString &left, UString &right, bool remove = false) const;

	UString substr(iterator from, iterator to) const;

	/** Parse a string into different types. */
	template<typename T> bool parse(T &v, int skip = 0) const {
		std::stringstream ss(_string.c_str() + skip);

		if ((ss >> v).fail())
			return false;

		return true;
	}

	/** Parse a string into a bool. */
	template<bool &> bool parse(bool &v, int skip = 0) const {
		int i;
		if (!parse(i, skip))
			return false;

		v = (i == 1);
		return true;
	}

	/** Read clean non-extended ASCII out of a stream. */
	void readASCII(SeekableReadStream &stream, bool colorCodes = false);
	/** Read clean non-extended ASCII out of a stream. */
	void readFixedASCII(SeekableReadStream &stream, uint32 length, bool colorCodes = false);
	/** Read a line of clean non-extended ASCII out of a stream. */
	void readLineASCII(SeekableReadStream &stream, bool colorCodes = false);

	/** Read Latin9 out of a stream. */
	void readLatin9(SeekableReadStream &stream, bool colorCodes = false);
	/** Read Latin9 out of a stream. */
	void readFixedLatin9(SeekableReadStream &stream, uint32 length, bool colorCodes = false);
	/** Read a line of Latin9 out of a stream. */
	void readLineLatin9(SeekableReadStream &stream, bool colorCodes = false);

	/** Read UTF-16LE out of a stream. */
	void readUTF16LE(SeekableReadStream &stream);
	/** Read UTF-16LE out of a stream. */
	void readFixedUTF16LE(SeekableReadStream &stream, uint32 length);
	/** Read a line of UTF-16LE out of a stream. */
	void readLineUTF16LE(SeekableReadStream &stream);

	/** Read UTF-16BE out of a stream. */
	void readUTF16BE(SeekableReadStream &stream);
	/** Read UTF-16BE out of a stream. */
	void readFixedUTF16BE(SeekableReadStream &stream, uint32 length);
	/** Read a line of UTF-16BE out of a stream. */
	void readLineUTF16BE(SeekableReadStream &stream);

	/** Read UTF8 out of a stream. */
	void readUTF8(SeekableReadStream &stream);
	/** Read a line of UTF8 out of a stream. */
	void readLineUTF8(SeekableReadStream &stream);

	/** Formatted printer, works like sprintf(). */
	static UString sprintf(const char *s, ...);

	static uint32 split(const UString &text, uint32 delim, std::vector<UString> &texts);

	static void splitTextTokens(const UString &text, std::vector<UString> &tokens);

	static uint32 tolower(uint32 c);
	static uint32 toupper(uint32 c);

	static bool isASCII(uint32 c); ///< Is the character an ASCII character?

	static bool isSpace(uint32 c); ///< Is the character an ASCII space character?
	static bool isDigit(uint32 c); ///< Is the character an ASCII digit character?
	static bool isAlpha(uint32 c); ///< Is the character an ASCII alphabetic character?
	static bool isAlNum(uint32 c); ///< Is the character an ASCII alphanumeric character?
	static bool isCntrl(uint32 c); ///< Is the character an ASCII control character?

	static uint32 fromUTF16(uint16 c);

private:
	std::string _string; ///< Internal string holding the actual data.

	uint32 _size;

	/** Read single-byte data. */
	void readSingleByte(SeekableReadStream &stream, std::vector<char> &data);
	/** Read single-byte data. */
	void readSingleByte(SeekableReadStream &stream, std::vector<char> &data, uint32 length);

	/** Read little-endian double-byte data. */
	void readDoubleByteLE(SeekableReadStream &stream, std::vector<uint16> &data);
	/** Read little-endian double-byte data. */
	void readDoubleByteLE(SeekableReadStream &stream, std::vector<uint16> &data, uint32 length);

	/** Read big-endian double-byte data. */
	void readDoubleByteBE(SeekableReadStream &stream, std::vector<uint16> &data);
	/** Read big-endian double-byte data. */
	void readDoubleByteBE(SeekableReadStream &stream, std::vector<uint16> &data, uint32 length);

	static void parseColorColors(std::vector<char> &data);

	void recalculateSize();
};


// Right-binding concatenation operators
static inline UString operator+(const std::string &left, const UString &right) {
	return UString(left) + right;
}

static inline UString operator+(const char *left, const UString &right) {
	return UString(left) + right;
}


// Hash functions

struct hashUStringCaseSensitive {
	std::size_t operator()(const UString &str) const {
		std::size_t seed = 0;

		for (UString::iterator it = str.begin(); it != str.end(); ++it)
			boost::hash_combine<uint32>(seed, *it);

		return seed;
	}
};

struct hashUStringCaseInsensitive {
	std::size_t operator()(const UString &str) const {
		std::size_t seed = 0;

		for (UString::iterator it = str.begin(); it != str.end(); ++it)
			boost::hash_combine<uint32>(seed, UString::tolower(*it));

		return seed;
	}
};

} // End of namespace Common

#endif // COMMON_USTRING_H

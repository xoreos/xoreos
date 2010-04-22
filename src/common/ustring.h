/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/ustring.h
 *  Unicode string handling.
 */

#ifndef COMMON_USTRING_H
#define COMMON_USTRING_H

#include <string>
#include <sstream>
#include <vector>

#include "common/types.h"

#include "utf8.h"

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

	UString(const UString &str);
	UString(const std::string &str);
	UString(const char *str = "");
	UString(const char *str, int n);
	~UString();

	UString &operator=(const UString &str);
	UString &operator=(const std::string &str);
	UString &operator=(const char *str);

	bool operator==(const UString &str) const;
	bool operator!=(const UString &str) const;
	bool operator<(const UString &str) const;
	bool operator<=(const UString &str) const;
	bool operator>(const UString &str) const;
	bool operator>=(const UString &str) const;

	UString operator+(const UString &str) const;
	UString operator+(const std::string &str) const;
	UString operator+(const char *str) const;
	UString operator+(uint32 c) const;

	UString &operator+=(const UString &str);
	UString &operator+=(const std::string &str);
	UString &operator+=(const char *str);
	UString &operator+=(uint32 c);

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

	void truncate(const iterator &it);
	void trim();

	/** Replace all occurences of a character with another character. */
	void replaceAll(uint32 what, uint32 with);

	/** Convert the string to lowercase. */
	void tolower();
	/** Convert the string to uppercase. */
	void toupper();

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
	void readASCII(SeekableReadStream &stream);
	/** Read clean non-extended ASCII out of a stream. */
	void readASCII(SeekableReadStream &stream, uint32 length);
	/** Read a line of clean non-extended ASCII out of a stream. */
	void readLineASCII(SeekableReadStream &stream);

	/** Read Latin9 out of a stream. */
	void readLatin9(SeekableReadStream &stream);
	/** Read Latin9 out of a stream. */
	void readLatin9(SeekableReadStream &stream, uint32 length);
	/** Read a line of Latin9 out of a stream. */
	void readLineLatin9(SeekableReadStream &stream);

	/** Read UTF-16LE out of a stream. */
	void readUTF16LE(SeekableReadStream &stream);
	/** Read UTF-16LE out of a stream. */
	void readUTF16LE(SeekableReadStream &stream, uint32 length);
	/** Read a line of UTF-16LE out of a stream. */
	void readLineUTF16LE(SeekableReadStream &stream);

	/** Read UTF-16BE out of a stream. */
	void readUTF16BE(SeekableReadStream &stream);
	/** Read UTF-16BE out of a stream. */
	void readUTF16BE(SeekableReadStream &stream, uint32 length);
	/** Read a line of UTF-16BE out of a stream. */
	void readLineUTF16BE(SeekableReadStream &stream);

	/** Formatted printer, works like sprintf(). */
	static UString sprintf(const char *s, ...);

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

	static uint32 tolower(uint32 c);
	static uint32 toupper(uint32 c);

	void recalculateSize();
};

} // End of namespace Common

#endif // COMMON_USTRING_H

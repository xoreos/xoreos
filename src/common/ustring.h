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
#include <vector>

#include "common/types.h"

#include "utf8.h"

namespace Common {

class SeekableReadStream;

/** A class holding an UTF-8 string.
 *
 *  WARNING:
 *  Copy constructors and assignment operators copying from std::string and
 *  const char * assume the data is clean (non-extended) ASCII.
 */
class UString {
public:
	typedef utf8::iterator<std::string::const_iterator> iterator;

	UString(const UString &str);
	UString(const std::string &str);
	UString(const char *str = "");
	~UString();

	UString &operator=(const UString &str);
	UString &operator=(const std::string &str);
	UString &operator=(const char *str);

	/** Return the size of the string, in characters.
	 *
	 *  Since this has to iterate through the whole string, this runs in O(n).
	 */
	uint32 size() const;

	/** Is the string empty?
	 *
	 *  Does not iterate over the whole string; runs in O(1).
	 */
	bool empty() const;

	/** Return the (utf8 encoded) string data. */
	const char *c_str() const;

	iterator begin() const;
	iterator end() const;

	/** Read clean non-extended ASCII out of a stream. */
	void readASCII(SeekableReadStream &stream);
	/** Read clean non-extended ASCII out of a stream. */
	void readASCII(SeekableReadStream &stream, uint32 length);

	/** Read Latin9 out of a stream. */
	void readLatin9(SeekableReadStream &stream);
	/** Read Latin9 out of a stream. */
	void readLatin9(SeekableReadStream &stream, uint32 length);

private:
	std::string _string; ///< Internal string holding the actual data.

	/** Read single-byte data. */
	void readSingleByte(SeekableReadStream &stream, std::vector<char> &data);
	/** Read single-byte data. */
	void readSingleByte(SeekableReadStream &stream, std::vector<char> &data, uint32 length);
};

} // End of namespace Common

#endif // COMMON_USTRING_H

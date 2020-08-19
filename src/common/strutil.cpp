/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  Utility templates and functions for working with strings and streams.
 */

#include <cassert>
#include <cctype>
#include <climits>
#include <cerrno>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include <memory>

#include "src/common/system.h"
#include "src/common/strutil.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/memreadstream.h"

namespace Common {

void printDataHex(SeekableReadStream &stream, size_t size) {
	size_t pos = stream.pos();

	size = MIN<size_t>(stream.size() - pos, size);

	if (size == 0)
		return;

	uint32_t offset = 0;
	byte rowData[16];

	while (size > 0) {
		// At max 16 bytes printed per row
		uint32_t n = MIN<size_t>(size, 16);
		if (stream.read(rowData, n) != n)
			throw Exception(kReadError);

		// Print an offset
		std::fprintf(stderr, "%08X  ", offset);

		// 2 "blobs" of each 8 bytes per row
		for (uint32_t i = 0; i < 2; i++) {
			for (uint32_t j = 0; j < 8; j++) {
				uint32_t m = i * 8 + j;

				if (m < n)
					// Print the data
					std::fprintf(stderr, "%02X ", rowData[m]);
				else
					// Last row, data count not aligned to 16
					std::fprintf(stderr, "   ");
			}

			// Separate the blobs by an extra space
			std::fprintf(stderr, " ");
		}

		std::fprintf(stderr, "|");

		// If the data byte is a printable character, print it. If not, substitute a '.'
		for (uint32_t i = 0; i < n; i++)
			std::fprintf(stderr, "%c", std::isprint(rowData[i]) ? rowData[i] : '.');

		std::fprintf(stderr, "|\n");

		size   -= n;
		offset += n;
	}

	// Seek back
	stream.seek(pos);
}

void printDataHex(const byte *data, size_t size) {
	if (!data || (size == 0))
		return;

	MemoryReadStream stream(data, size);
	printDataHex(stream);
}

static bool tagToString(uint32_t tag, bool trim, UString &str) {
	tag = TO_BE_32(tag);

	const char *tS = reinterpret_cast<const char *>(&tag);
	if (!std::isprint(tS[0]) || !std::isprint(tS[1]) || !std::isprint(tS[2]) || !std::isprint(tS[3]))
		return false;

	str = UString::format("%c%c%c%c", tS[0], tS[1], tS[2], tS[3]);
	if (trim)
		str.trim();

	return true;
}

UString debugTag(uint32_t tag, bool trim) {
	UString str;
	if (tagToString(tag, trim, str))
		return UString::format("0x%08X ('%s')", FROM_BE_32(tag), str.c_str());

	return UString::format("0x%08X", FROM_BE_32(tag));
}

// Helper functions for parseString()

static void errorOnSign(const char *str) {
	if (strchr(str, '-') != 0)
		errno = ERANGE;
}

static inline void parse(const char *nptr, char **endptr, signed long long &value) {
	value = strtoll(nptr, endptr, 0);
}

static inline void parse(const char *nptr, char **endptr, unsigned long long &value) {
	errorOnSign(nptr);

	value = strtoull(nptr, endptr, 0);
}

static inline void parse(const char *nptr, char **endptr, signed long &value) {
	value = strtol(nptr, endptr, 0);
}

static inline void parse(const char *nptr, char **endptr, unsigned long &value) {
	errorOnSign(nptr);

	value = strtoul(nptr, endptr, 0);
}

static inline void parse(const char *nptr, char **endptr, signed int &value) {
	signed long tmp = strtol(nptr, endptr, 0);
	if ((tmp < INT_MIN) || (tmp > INT_MAX))
		errno = ERANGE;

	value = (signed int) tmp;
}

static inline void parse(const char *nptr, char **endptr, unsigned int &value) {
	errorOnSign(nptr);

	unsigned long tmp = strtoul(nptr, endptr, 0);
	if (tmp > UINT_MAX)
		errno = ERANGE;

	value = (unsigned int) tmp;
}

static inline void parse(const char *nptr, char **endptr, signed short &value) {
	signed long tmp = strtol(nptr, endptr, 0);
	if ((tmp < SHRT_MIN) || (tmp > SHRT_MAX))
		errno = ERANGE;

	value = (signed short) tmp;
}

static inline void parse(const char *nptr, char **endptr, unsigned short &value) {
	errorOnSign(nptr);

	unsigned long tmp = strtoul(nptr, endptr, 0);
	if (tmp > USHRT_MAX)
		errno = ERANGE;

	value = (unsigned short) tmp;
}

static inline void parse(const char *nptr, char **endptr, signed char &value) {
	signed long tmp = strtol(nptr, endptr, 0);
	if ((tmp < SCHAR_MIN) || (tmp > SCHAR_MAX))
		errno = ERANGE;

	value = (signed char) tmp;
}

static inline void parse(const char *nptr, char **endptr, unsigned char &value) {
	errorOnSign(nptr);

	unsigned long tmp = strtoul(nptr, endptr, 0);
	if (tmp > UCHAR_MAX)
		errno = ERANGE;

	value = (unsigned char) tmp;
}

static inline void parse(const char *nptr, char **endptr, float &value) {
	value = strtof(nptr, endptr);
}

static inline void parse(const char *nptr, char **endptr, double &value) {
	value = strtod(nptr, endptr);
}


template<typename T> void parseString(const UString &str, T &value, bool allowEmpty) {
	if (str.empty()) {
		if (allowEmpty)
			return;

		throw Exception("Trying to parse an empty string");
	}

	const char *nptr = str.c_str();
	char *endptr = 0;

	errno = 0;

	T newValue;
	parse(nptr, &endptr, newValue);

	while (endptr && isspace(*endptr))
		endptr++;

	if (endptr && (*endptr != '\0'))
		throw Exception("Can't convert \"%s\" to type of size %u", str.c_str(), (uint)sizeof(T));
	if (errno == ERANGE)
		throw Exception("\"%s\" out of range for type of size %u", str.c_str(), (uint)sizeof(T));

	value = newValue;
}

template<> void parseString(const UString &str, bool &value, bool allowEmpty) {
	if (str.empty()) {
		if (allowEmpty)
			return;

		throw Exception("Trying to parse an empty string");
	}

	// Valid true values are "true", "yes", "y", "on" and "1"

	value =
		(str.equalsIgnoreCase("true") ||
		 str.equalsIgnoreCase("yes")  ||
		 str.equalsIgnoreCase("y")    ||
		 str.equalsIgnoreCase("on")   ||
		 str == "1") ?
		true : false;
}

template void parseString<  signed char     >(const UString &str,   signed char      &value, bool allowEmpty);
template void parseString<unsigned char     >(const UString &str, unsigned char      &value, bool allowEmpty);
template void parseString<  signed short    >(const UString &str,   signed short     &value, bool allowEmpty);
template void parseString<unsigned short    >(const UString &str, unsigned short     &value, bool allowEmpty);
template void parseString<  signed int      >(const UString &str,   signed int       &value, bool allowEmpty);
template void parseString<unsigned int      >(const UString &str, unsigned int       &value, bool allowEmpty);
template void parseString<  signed long     >(const UString &str,   signed long      &value, bool allowEmpty);
template void parseString<unsigned long     >(const UString &str, unsigned long      &value, bool allowEmpty);
template void parseString<  signed long long>(const UString &str,   signed long long &value, bool allowEmpty);
template void parseString<unsigned long long>(const UString &str, unsigned long long &value, bool allowEmpty);

template void parseString<float             >(const UString &str, float              &value, bool allowEmpty);
template void parseString<double            >(const UString &str, double             &value, bool allowEmpty);


template<typename T> UString composeString(T value) {
	/* Create a string representation of the value, in decimal notation.
	 *
	 * Build up the string digit by digit, least significant digit first
	 * (thus filling up the string back to front), by repeatedly dividing
	 * the value by 10.
	 */

	/* Remember whether the value is negative. We use that afterwards to
	 * prepend the negative sign. */
	const bool isNegative = value < 0;

	/* Our string buffer and a pointer to the start of the string. We
	 * fill back to front, so it points at the end of the buffer now. */
	char buf[64], *strStart = buf + sizeof(buf) - 1;

	/* Start by putting the final string terminator into the string. */
	*strStart-- = '\0';

	/* Collect all the digits, back to front.
	 *
	 * Note that the value might be negative [1]. The sign of the result
	 * of the %-operator on negative numbers is implementation-defined,
	 * so we ABS() the result (0-9) back to positive.
	 *
	 * In UTF-8 (as well as ASCII, but we only care about UTF-8 here),
	 * the digits are continuous, so we can just add this remainder to
	 * '0' to get the UTF-8 codepoint for the digit in question.
	 *
	 * [1] We also don't just want to negate a negative number to make
	 * it positive: this would break with INT8_MIN, INT16_MIN, etc.,
	 * because -INT8_MIN (128) is not a valid int8_t value. */
	do {
		*strStart-- = ABS(value % 10) + '0';
	} while ((value /= 10) && (strStart != buf));

	/* Sanity check; shouldn't happen because the buffer is big enough. */
	if (strStart == buf)
		throw Exception("Buffer overrun in composeString()");

	/* Write the sign, if the value was negative. */
	if (isNegative)
		*strStart-- = '-';

	/* We've moved one past the actual start of the string now. */
	strStart++;

	return UString(strStart);
}

template<> UString composeString(bool value) {
	return value ? "true" : "false";
}

template<> UString composeString(float value) {
	return UString::format("%f", value);
}

template<> UString composeString(double value) {
	return UString::format("%lf", value);
}

template UString composeString<  signed char     >(  signed char      value);
template UString composeString<unsigned char     >(unsigned char      value);
template UString composeString<  signed short    >(  signed short     value);
template UString composeString<unsigned short    >(unsigned short     value);
template UString composeString<  signed int      >(  signed int       value);
template UString composeString<unsigned int      >(unsigned int       value);
template UString composeString<  signed long     >(  signed long      value);
template UString composeString<unsigned long     >(unsigned long      value);
template UString composeString<  signed long long>(  signed long long value);
template UString composeString<unsigned long long>(unsigned long long value);

size_t searchBackwards(SeekableReadStream &haystack, const byte *needle, size_t needleSize,
                       size_t maxReadBack) {

	if (needleSize == 0 || maxReadBack == 0)
		return SIZE_MAX;

	assert(maxReadBack >= needleSize);

	static const size_t kReadBufferSize = 0x400;

	const size_t sizeFile = haystack.size();
	const size_t maxBack  = MIN<size_t>(maxReadBack, sizeFile);

	std::unique_ptr<byte[]> buf = std::make_unique<byte[]>(kReadBufferSize + needleSize);

	size_t backRead = needleSize;
	while (backRead < maxBack) {
		backRead = MIN<size_t>(maxBack, backRead + kReadBufferSize);

		const size_t readPos  = sizeFile - backRead;
		const size_t readSize = MIN<size_t>(kReadBufferSize + needleSize, sizeFile - readPos);

		try {
			haystack.seek(readPos);
		} catch (...) {
			break;
		}

		if (haystack.read(buf.get(), readSize) != readSize)
			break;

		for (size_t i = (readSize - (needleSize - 1)); i-- > 0; )
			if (!memcmp(buf.get() + i, needle, needleSize))
				return readPos + i;
	}

	return SIZE_MAX;
}

} // End of namespace Common

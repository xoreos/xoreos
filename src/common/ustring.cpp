/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <cstdarg>
#include <cstdio>
#include <cctype>

#include <iconv.h>

#include "common/ustring.h"
#include "common/error.h"
#include "common/singleton.h"
#include "common/stream.h"
#include "common/util.h"

namespace Common {

static int readSingleByte(SeekableReadStream &stream, uint32 &c) {
	c = stream.readByte();
	return 1;
}

static int readDoubleByteLE(SeekableReadStream &stream, uint32 &c) {
	c = stream.readUint16LE();
	return 2;
}

static int readDoubleByteBE(SeekableReadStream &stream, uint32 &c) {
	c = stream.readUint16BE();
	return 2;
}

template<typename T>
static void readLine(SeekableReadStream &stream, std::vector<T> &data,
		int (*readFunc)(SeekableReadStream &, uint32 &c)) {

	uint32 c = 0;
	int cLen = 0;

	// If end-of-file occurs before any characters are read, return
	if (stream.eos() || stream.err()) {
		data.push_back(0);
		return;
	}

	// Loop as long as the line has not ended
	while (c != '\n') {
		(*readFunc)(stream, c);

		// If an error occurs, return
		if (stream.eos() || stream.err())
			break;

		// Check for CR or CR/LF
		// * DOS and Windows use CRLF line breaks
		// * Unix and OS X use LF line breaks
		// * Macintosh before OS X used CR line breaks
		if (c == '\r') {
			// Look at the next char -- is it LF? If not, seek back
			cLen = (*readFunc)(stream, c);

			if (stream.err())
				break; // error: the buffer contents are indeterminate

			if (stream.eos()) {
				// The CR was the last character in the file.
				// Reset the eos() flag since we successfully finished a line
				stream.clearErr();
			} else if (c != '\n')
				stream.seek(-cLen, SEEK_CUR);

			// Treat CR & CR/LF as plain LF
			c = '\n';
		}

		data.push_back((T) c);
	}

	// Remove any end-of-line characters and 0-terminate the string
	if (!data.empty() && (data.back() == '\n'))
		data.back() = '\0';
	else
		data.push_back('\0');
}

/** A manager handling string encoding conversions. */
class ConversionManager : public Singleton<ConversionManager> {
public:
	ConversionManager() : _fromLatin9((iconv_t) -1) {
		_fromLatin9 = iconv_open("UTF-8", "ISO-8859-15");
		if (_fromLatin9 == ((iconv_t) -1))
			throw Exception("Failed to initialize ISO-8859-15 -> UTF-8 conversion");
	}

	~ConversionManager() {
		if (_fromLatin9 != ((iconv_t) -1))
			iconv_close(_fromLatin9);
	}

	std::string fromLatin9(byte *data, uint32 n) {
		if (_fromLatin9 == ((iconv_t) -1))
			throw Exception("No iconv context");

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

UString::UString(const char *str, int n) {
	*this = std::string(str, n);
}

UString::UString(iterator sBegin, iterator sEnd) {
	for (; sBegin != sEnd; ++sBegin)
		*this += *sBegin;
}

UString::~UString() {
}

UString &UString::operator=(const UString &str) {
	_string = str._string;
	_size   = str._size;

	return *this;
}

UString &UString::operator=(const std::string &str) {
	_string = str;

	recalculateSize();

	return *this;
}

UString &UString::operator=(const char *str) {
	*this = std::string(str);

	return *this;
}

bool UString::operator==(const UString &str) const {
	return strcmp(str) == 0;
}

bool UString::operator!=(const UString &str) const {
	return strcmp(str) != 0;
}

bool UString::operator<(const UString &str) const {
	return strcmp(str) < 0;
}

bool UString::operator>(const UString &str) const {
	return strcmp(str) > 0;
}

UString UString::operator+(const UString &str) const {
	UString tmp(*this);

	tmp += str;

	return tmp;
}

UString UString::operator+(const std::string &str) const {
	UString tmp(*this);

	tmp += str;

	return tmp;
}

UString UString::operator+(const char *str) const {
	UString tmp(*this);

	tmp += str;

	return tmp;
}

UString UString::operator+(uint32 c) const {
	UString tmp(*this);

	tmp += c;

	return tmp;
}

UString &UString::operator+=(const UString &str) {
	_string += str._string;
	_size   += str._size;

	return *this;
}

UString &UString::operator+=(const std::string &str) {
	UString ustr(str);

	return *this += ustr;
}

UString &UString::operator+=(const char *str) {
	UString ustr(str);

	return *this += ustr;
}

UString &UString::operator+=(uint32 c) {
	try {
		utf8::append(c, std::back_inserter(_string));
	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	_size++;

	return *this;
}

int UString::strcmp(const UString &str) const {
	UString::iterator it1 = begin();
	UString::iterator it2 = str.begin();
	for (; (it1 != end()) && (it2 != str.end()); ++it1, ++it2) {
		uint32 c1 = *it1;
		uint32 c2 = *it2;

		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return  1;
	}

	if ((it1 == end()) && (it2 == str.end()))
		return 0;

	if (it1 == end())
		return -1;

	return 1;
}

int UString::stricmp(const UString &str) const {
	UString::iterator it1 = begin();
	UString::iterator it2 = str.begin();
	for (; (it1 != end()) && (it2 != str.end()); ++it1, ++it2) {
		uint32 c1 = tolower(*it1);
		uint32 c2 = tolower(*it2);

		if (c1 < c2)
			return -1;
		if (c1 > c2)
			return  1;
	}

	if ((it1 == end()) && (it2 == str.end()))
		return 0;

	if (it1 == end())
		return -1;

	return 1;
}

bool UString::equals(const UString &str) const {
	return strcmp(str) == 0;
}

bool UString::equalsIgnoreCase(const UString &str) const {
	return stricmp(str) == 0;
}

bool UString::less(const UString &str) const {
	return strcmp(str) < 0;
}

bool UString::lessIgnoreCase(const UString &str) const {
	return stricmp(str) < 0;
}

void UString::swap(UString &str) {
	_string.swap(str._string);

	SWAP(_size, str._size);
}

void UString::clear() {
	_string.clear();
	_size = 0;
}

uint32 UString::size() const {
	return _size;
}

bool UString::empty() const {
	return _string.empty() || (_string[0] == '\0');
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

UString::iterator UString::findFirst(uint32 c) const {
	for (iterator it = begin(); it != end(); ++it)
		if (*it == c)
			return it;

	return end();
}

bool UString::beginsWith(const UString &with) const {
	if (with.empty())
		return true;

	if (empty())
		return false;

	UString::iterator myIt   = begin();
	UString::iterator withIt = with.begin();

	while ((myIt != end()) && (withIt != with.end()))
		if (*myIt++ != *withIt++)
			return false;

	if ((myIt == end()) && (withIt != with.end()))
		return false;

	return true;
}

bool UString::endsWith(const UString &with) const {
	if (with.empty())
		return true;

	if (empty())
		return false;

	UString::iterator myIt   = --end();
	UString::iterator withIt = --with.end();

	while ((myIt != begin()) && (withIt != with.begin()))
		if (*myIt-- != *withIt--)
			return false;

	if (withIt == with.begin())
		return (*myIt == *withIt);

	return false;
}

bool UString::contains(const UString &what) const {
	return _string.find(what._string) != std::string::npos;
}

void UString::truncate(const iterator &it) {
	Common::UString temp;

	for (iterator i = begin(); i != it; ++i)
		temp += *i;

	*this = temp;
}

void UString::truncate(uint32 n) {
	if (n >= _size)
		return;

	Common::UString temp;

	for (iterator it = begin(); n > 0; ++it, n--)
		temp += *it;

	*this = temp;
}

void UString::trim() {
	if (_string.empty())
		// Nothing to do
		return;

	// Find the last space, from the end
	iterator itEnd = --end();
	for (; itEnd != begin(); --itEnd) {
		uint32 c = *itEnd;
		if ((c != '\0') && (c != ' ')) {
			++itEnd;
			break;
		}
	}

	if (itEnd == begin()) {
		uint32 c = *itEnd;
		if ((c != '\0') && (c != ' '))
			++itEnd;
	}

	// Find the first non-space
	iterator itStart = begin();
	for (; itStart != itEnd; ++itStart)
		if (*itStart != ' ')
			break;

	_string = std::string(itStart.base(), itEnd.base());
	recalculateSize();
}

void UString::trimLeft() {
	if (_string.empty())
		// Nothing to do
		return;

	// Find the first non-space
	iterator itStart = begin();
	for (; itStart != end(); ++itStart)
		if (*itStart != ' ')
			break;

	_string = std::string(itStart.base(), end().base());
	recalculateSize();
}

void UString::trimRight() {
	if (_string.empty())
		// Nothing to do
		return;

	// Find the last space, from the end
	iterator itEnd = --end();
	for (; itEnd != begin(); --itEnd) {
		uint32 c = *itEnd;
		if ((c != '\0') && (c != ' ')) {
			++itEnd;
			break;
		}
	}

	if (itEnd == begin()) {
		uint32 c = *itEnd;
		if ((c != '\0') && (c != ' '))
			++itEnd;
	}

	_string = std::string(begin().base(), itEnd.base());
	recalculateSize();
}

void UString::replaceAll(uint32 what, uint32 with) {
	try {

		// The new string with characters replaced
		std::string newString;
		newString.reserve(_string.size());

		// Run through the whole string
		std::string::iterator it = _string.begin();
		while (it != _string.end()) {
			std::string::iterator prev = it;

			// Get the codepoint
			uint32 c = utf8::next(it, _string.end());

			if (c != what) {
				// It's not what we're looking for, copy it
				for (; prev != it; ++prev)
					newString.push_back(*prev);
			} else
				// It's what we're looking for, insert the replacement instead
				utf8::append(with, std::back_inserter(newString));

		}

		// And set the new string's contents
		_string.swap(newString);

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}
}

void UString::tolower() {
	try {

		// The new string with characters replaced
		std::string newString;
		newString.reserve(_string.size());

		// Run through the whole string
		std::string::iterator it = _string.begin();
		while (it != _string.end()) {
			std::string::iterator prev = it;

			// Get the codepoint
			uint32 c = utf8::next(it, _string.end());

			// And append the lowercase version to the new string
			utf8::append(UString::tolower(c), std::back_inserter(newString));
		}

		// And set the new string's contents
		_string.swap(newString);

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}
}

void UString::toupper() {
	try {

		// The new string with characters replaced
		std::string newString;
		newString.reserve(_string.size());

		// Run through the whole string
		std::string::iterator it = _string.begin();
		while (it != _string.end()) {
			std::string::iterator prev = it;

			// Get the codepoint
			uint32 c = utf8::next(it, _string.end());

			// And append the uppercase version to the new string
			utf8::append(UString::toupper(c), std::back_inserter(newString));
		}

		// And set the new string's contents
		_string.swap(newString);

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}
}

void UString::erase(uint32 n) {
	if (n >= _size)
		return;

	Common::UString temp;

	uint32 i = 0;
	for (iterator it = begin(); it != end(); ++it, i++)
		if (i != n)
			temp += *it;

	*this = temp;
}

void UString::readASCII(SeekableReadStream &stream) {
	clear();

	std::vector<char> data;
	readSingleByte(stream, data);
	if (data.empty())
		return;

	_string = (const char *) &data[0];
	recalculateSize();
}

void UString::readASCII(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<char> data;
	readSingleByte(stream, data, length);
	if (data.empty())
		return;

	_string = (const char *) &data[0];
	recalculateSize();
}

void UString::readLineASCII(SeekableReadStream &stream) {
	clear();

	std::vector<char> data;
	readLine(stream, data, &Common::readSingleByte);
	if (data.empty())
		return;

	_string = (const char *) &data[0];
	recalculateSize();
}

void UString::readLatin9(SeekableReadStream &stream) {
	clear();

	std::vector<char> data;
	readSingleByte(stream, data);
	if (data.empty())
		return;

	_string = ConvMan.fromLatin9((byte *) &data[0], strlen((const char *) &data[0]));
	recalculateSize();
}

void UString::readLatin9(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<char> data;
	readSingleByte(stream, data, length);
	if (data.empty())
		return;

	_string = ConvMan.fromLatin9((byte *) &data[0], strlen((const char *) &data[0]));
	recalculateSize();
}

void UString::readLineLatin9(SeekableReadStream &stream) {
	clear();

	std::vector<char> data;
	readLine(stream, data, &Common::readSingleByte);

	if (data.empty())
		return;

	_string = ConvMan.fromLatin9((byte *) &data[0], strlen((const char *) &data[0]));
	recalculateSize();
}

void UString::readUTF16LE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readDoubleByteLE(stream, data);
	if (data.empty())
		return;

	try {

		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readUTF16LE(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<uint16> data;
	readDoubleByteLE(stream, data, length);
	if (data.empty())
		return;

	try {
		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readLineUTF16LE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readLine(stream, data, &Common::readDoubleByteLE);
	if (data.empty())
		return;

	try {

		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readUTF16BE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readDoubleByteBE(stream, data);
	if (data.empty())
		return;

	try {

		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readUTF16BE(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<uint16> data;
	readDoubleByteBE(stream, data, length);
	if (data.empty())
		return;

	try {

		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readLineUTF16BE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readLine(stream, data, &Common::readDoubleByteBE);
	if (data.empty())
		return;

	try {

		utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));

	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	recalculateSize();
}

void UString::readUTF8(SeekableReadStream &stream) {
	readASCII(stream);
}

void UString::readLineUTF8(SeekableReadStream &stream) {
	readLineASCII(stream);
}

void UString::readSingleByte(SeekableReadStream &stream, std::vector<char> &data) {
	char c;
	while ((c = stream.readByte()) != 0)
		data.push_back(c);

	if (stream.err())
		throw Exception(kReadError);

	data.push_back('\0');
}

void UString::readSingleByte(SeekableReadStream &stream, std::vector<char> &data, uint32 length) {
	data.resize(length + 1);

	if (stream.read((byte *) &data[0], length) != length)
		throw Exception(kReadError);

	data.push_back('\0');
}

void UString::readDoubleByteLE(SeekableReadStream &stream, std::vector<uint16> &data) {
	uint16 c;
	while ((c = stream.readUint16LE()) != 0)
		data.push_back(c);

	if (stream.err())
		throw Exception(kReadError);

	data.push_back('\0');
}

void UString::readDoubleByteLE(SeekableReadStream &stream, std::vector<uint16> &data, uint32 length) {
	data.reserve(length + 1);

	while (length-- > 0)
		data.push_back(stream.readUint16LE());

	if (stream.err())
		throw Exception(kReadError);

	data.push_back('\0');
}

void UString::readDoubleByteBE(SeekableReadStream &stream, std::vector<uint16> &data) {
	uint16 c;
	while ((c = stream.readUint16BE()) != 0)
		data.push_back(c);

	if (stream.err())
		throw Exception(kReadError);

	data.push_back('\0');
}

void UString::readDoubleByteBE(SeekableReadStream &stream, std::vector<uint16> &data, uint32 length) {
	data.reserve(length + 1);

	while (length-- > 0)
		data.push_back(stream.readUint16BE());

	if (stream.err())
		throw Exception(kReadError);

	data.push_back('\0');
}

UString UString::sprintf(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	return UString(buf);
}

void UString::recalculateSize() {
	try {
		// Calculate the "distance" in characters from the beginning and end
		_size = utf8::distance(_string.begin(), _string.end());
	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}
}

// NOTE: If we ever need uppercase<->lowercase mappings for non-ASCII
//       characters: http://www.unicode.org/reports/tr21/tr21-5.html

uint32 UString::tolower(uint32 c) {
	if (!isASCII(c))
		// We don't know how to lowercase that
		return c;

	return std::tolower(c);
}

uint32 UString::toupper(uint32 c) {
	if (!isASCII(c))
		// We don't know how to uppercase that
		return c;

	return std::toupper(c);
}

bool UString::isASCII(uint32 c) {
	return (c & 0xFFFFFF80) == 0;
}

bool UString::isSpace(uint32 c) {
	return isASCII(c) && std::isspace(c);
}

bool UString::isDigit(uint32 c) {
	return isASCII(c) && std::isdigit(c);
}

bool UString::isAlpha(uint32 c) {
	return isASCII(c) && std::isalpha(c);
}

bool UString::isAlNum(uint32 c) {
	return isASCII(c) && std::isalnum(c);
}

bool UString::isCntrl(uint32 c) {
	return isASCII(c) && std::iscntrl(c);
}

uint32 UString::fromUTF16(uint16 c) {
	std::string utf8result;

	try {
		utf8::utf16to8(&c, &c + 1, std::back_inserter(utf8result));
	} catch (const std::exception &se) {
		Exception e(se.what());
		throw e;
	}

	return *iterator(utf8result.begin(), utf8result.begin(), utf8result.end());
}

} // End of namespace Common

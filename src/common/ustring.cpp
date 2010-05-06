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

UString::~UString() {
}

UString &UString::operator=(const UString &str) {
	_string = str._string;
	_size   = str._size;

	return *this;
}

UString &UString::operator=(const std::string &str) {
	std::string::const_iterator itEnd = utf8::find_invalid(str.begin(), str.end());

	if (itEnd != str.end())
		warning("Invalid UTF8 string: \"%s\"", str.c_str());

	// Create the string for the valid utf8 portion of the string.
	// Will work for clean non-extended ASCII strings, too.
	_string = std::string(str.begin(), itEnd);

	recalculateSize();

	return *this;
}

UString &UString::operator=(const char *str) {
	*this = std::string(str);

	return *this;
}

bool UString::operator==(const UString &str) const {
	return _string == str._string;
}

bool UString::operator!=(const UString &str) const {
	return _string != str._string;
}

bool UString::operator<(const UString &str) const {
	return _string < str._string;
}

bool UString::operator<=(const UString &str) const {
	return _string <= str._string;
}

bool UString::operator>(const UString &str) const {
	return _string > str._string;
}

bool UString::operator>=(const UString &str) const {
	return _string >= str._string;
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
	utf8::append(c, std::back_inserter(_string));

	_size++;

	return *this;
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

UString::iterator UString::findFirst(uint32 c) const {
	for (iterator it = begin(); it != end(); ++it)
		if (*it == c)
			return it;

	return end();
}

bool UString::beginsWith(const Common::UString &with) const {
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

bool UString::endsWith(const Common::UString &with) const {
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

void UString::truncate(const iterator &it) {
	_string.resize(std::distance((std::string::const_iterator) _string.begin(), it.base()));
	recalculateSize();
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

	iterator itStart = begin();
	for (; itStart != itEnd; ++itStart)
		if (*itStart != ' ')
			break;

	_string = std::string(itStart.base(), itEnd.base());
	recalculateSize();
}

void UString::replaceAll(uint32 what, uint32 with) {
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
}

void UString::tolower() {
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
}

void UString::toupper() {
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

	_string = ConvMan.fromLatin9((byte *) &data[0], data.size());
	recalculateSize();
}

void UString::readLatin9(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<char> data;
	readSingleByte(stream, data, length);
	if (data.empty())
		return;

	_string = ConvMan.fromLatin9((byte *) &data[0], data.size());
	recalculateSize();
}

void UString::readLineLatin9(SeekableReadStream &stream) {
	clear();

	std::vector<char> data;
	readLine(stream, data, &Common::readSingleByte);

	if (data.empty())
		return;

	_string = ConvMan.fromLatin9((byte *) &data[0], data.size());
	recalculateSize();
}

void UString::readUTF16LE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readDoubleByteLE(stream, data);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
}

void UString::readUTF16LE(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<uint16> data;
	readDoubleByteLE(stream, data, length);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
}

void UString::readLineUTF16LE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readLine(stream, data, &Common::readDoubleByteLE);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
}

void UString::readUTF16BE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readDoubleByteBE(stream, data);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
}

void UString::readUTF16BE(SeekableReadStream &stream, uint32 length) {
	clear();

	std::vector<uint16> data;
	readDoubleByteBE(stream, data, length);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
}

void UString::readLineUTF16BE(SeekableReadStream &stream) {
	clear();

	std::vector<uint16> data;
	readLine(stream, data, &Common::readDoubleByteBE);
	if (data.empty())
		return;

	utf8::utf16to8(data.begin(), data.end(), std::back_inserter(_string));
	recalculateSize();
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
	// Calculate the "distance" in characters from the beginning and end
	_size = utf8::distance(_string.begin(), _string.end());
}

uint32 UString::tolower(uint32 c) {
	if ((c & 0xFFFFFF80) != 0)
		// It's not a clean ASCII character, so we don't know how to lowercase it
		return c;

	return std::tolower(c);
}

uint32 UString::toupper(uint32 c) {
	if ((c & 0xFFFFFF80) != 0)
		// It's not a clean ASCII character, so we don't know how to uppercase it
		return c;

	return std::toupper(c);
}

} // End of namespace Common

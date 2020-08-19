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
 *  Unicode string handling.
 */

#include <cstdarg>
#include <cstdio>
#include <cctype>
#include <cstring>

#include <boost/algorithm/string/replace.hpp>

#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/util.h"

namespace Common {

UString::UString() : _size(0) {
}

UString::UString(const UString &str) {
	*this = str;
}

UString::UString(const std::string &str) {
	*this = str;
}

UString::UString(const char *str) {
	*this = str;
}

UString::UString(const char *str, size_t n) {
	*this = std::string(str, n);
}

UString::UString(uint32_t c, size_t n) : _size(0) {
	while (n-- > 0)
		*this += c;
}

UString::UString(iterator sBegin, iterator sEnd) : _size(0) {
	for (; (sBegin != sEnd) && *sBegin; ++sBegin)
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

UString UString::operator+(uint32_t c) const {
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

UString &UString::operator+=(uint32_t c) {
	try {
		utf8::append(c, std::back_inserter(_string));
	} catch (const std::exception &se) {
		Exception e(se);
		throw e;
	}

	_size++;

	return *this;
}

int UString::strcmp(const UString &str) const {
	try {
		UString::iterator it1 = begin();
		UString::iterator it2 = str.begin();
		for (; (it1 != end()) && (it2 != str.end()); ++it1, ++it2) {
			const uint32_t c1 = *it1;
			const uint32_t c2 = *it2;

			if (c1 < c2)
				return -1;
			if (c1 > c2)
				return  1;
		}

		if ((it1 == end()) && (it2 == str.end()))
			return 0;

		if (it1 == end())
			return -1;

	} catch (...) {
	}

	return 1;
}

int UString::stricmp(const UString &str) const {
	try {
		UString::iterator it1 = begin();
		UString::iterator it2 = str.begin();
		for (; (it1 != end()) && (it2 != str.end()); ++it1, ++it2) {
			const uint32_t c1 = toLower(*it1);
			const uint32_t c2 = toLower(*it2);

			if (c1 < c2)
				return -1;
			if (c1 > c2)
				return  1;
		}

		if ((it1 == end()) && (it2 == str.end()))
			return 0;

		if (it1 == end())
			return -1;

	} catch (...) {
	}

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

	std::swap(_size, str._size);
}

void UString::clear() {
	_string.clear();
	_size = 0;
}

size_t UString::size() const {
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

UString::iterator UString::findFirst(uint32_t c) const {
	for (iterator it = begin(); it != end(); ++it)
		if (*it == c)
			return it;

	return end();
}

UString::iterator UString::findFirst(const UString &what) const {
	size_t index = _string.find(what._string);
	if (index != std::string::npos) {
		std::string::const_iterator it = _string.begin();
		std::advance(it, index);
		return iterator(it, _string.begin(), _string.end());
	}
	return end();
}

UString::iterator UString::findLast(uint32_t c) const {
	if (empty())
		return end();

	iterator it = end();
	do {
		--it;

		if (*it == c)
			return it;

	} while (it != begin());

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

bool UString::contains(uint32_t c) const {
	return findFirst(c) != end();
}

void UString::truncate(const iterator &it) {
	UString temp;

	for (iterator i = begin(); i != it; ++i)
		temp += *i;

	swap(temp);
}

void UString::truncate(size_t n) {
	if (n >= _size)
		return;

	UString temp;

	for (iterator it = begin(); n > 0; ++it, n--)
		temp += *it;

	swap(temp);
}

void UString::trim() {
	if (_string.empty())
		// Nothing to do
		return;

	// Find the last space, from the end
	iterator itEnd = --end();
	for (; itEnd != begin(); --itEnd) {
		uint32_t c = *itEnd;
		if (!isSpace(c) && (c != '\0')) {
			++itEnd;
			break;
		}
	}

	if (itEnd == begin()) {
		uint32_t c = *itEnd;
		if (!isSpace(c) && (c != '\0'))
			++itEnd;
	}

	// Find the first non-space
	iterator itStart = begin();
	for (; itStart != itEnd; ++itStart)
		if (!isSpace(*itStart))
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
		if (!isSpace(*itStart))
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
		uint32_t c = *itEnd;
		if (!isSpace(c) && (c != '\0')) {
			++itEnd;
			break;
		}
	}

	if (itEnd == begin()) {
		uint32_t c = *itEnd;
		if (!isSpace(c) && (c != '\0'))
			++itEnd;
	}

	_string = std::string(begin().base(), itEnd.base());
	recalculateSize();
}

void UString::replaceAll(uint32_t what, uint32_t with) {
	try {

		// The new string with characters replaced
		std::string newString;
		newString.reserve(_string.size());

		// Run through the whole string
		std::string::iterator it = _string.begin();
		while (it != _string.end()) {
			std::string::iterator prev = it;

			// Get the codepoint
			uint32_t c = utf8::next(it, _string.end());

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
		Exception e(se);
		throw e;
	}
}

void UString::replaceAll(const UString &what, const UString &with) {
	boost::replace_all(_string, what._string, with._string);
}

void UString::makeLower() {
	*this = toLower();
}

void UString::makeUpper() {
	*this = toUpper();
}

UString UString::toLower() const {
	UString str;

	str._string.reserve(_string.size());
	for (iterator it = begin(); it != end(); ++it)
		str += toLower(*it);

	return str;
}

UString UString::toUpper() const {
	UString str;

	str._string.reserve(_string.size());
	for (iterator it = begin(); it != end(); ++it)
		str += toUpper(*it);

	return str;
}

UString::iterator UString::getPosition(size_t n) const {
	iterator it = begin();
	for (size_t i = 0; (i < n) && (it != end()); i++, ++it);
	return it;
}

size_t UString::getPosition(iterator it) const {
	size_t n = 0;
	for (iterator i = begin(); i != it; ++i, n++);
	return n;
}

void UString::insert(iterator pos, uint32_t c) {
	if (pos == end()) {
		*this += c;
		return;
	}

	UString temp;

	iterator it;
	for (it = begin(); it != pos; ++it)
		temp += *it;

	temp += c;

	for ( ; it != end(); ++it)
		temp += *it;

	swap(temp);
}

void UString::insert(UString::iterator pos, const UString &str) {
	if (pos == end()) {
		*this += str;
		return;
	}

	UString temp;

	iterator it;
	for (it = begin(); it != pos; ++it)
		temp += *it;

	temp += str;

	for ( ; it != end(); ++it)
		temp += *it;

	swap(temp);
}

void UString::replace(iterator pos, uint32_t c) {
	if (pos == end()) {
		*this += c;
		return;
	}

	UString temp;

	iterator it;
	for (it = begin(); it != pos; ++it)
		temp += *it;

	temp += c;

	for (++it; it != end(); ++it)
		temp += *it;

	swap(temp);
}

void UString::replace(UString::iterator pos, const UString &str) {
	if (pos == end()) {
		*this += str;
		return;
	}

	UString temp;

	iterator it;
	for (it = begin(); it != pos; ++it)
		temp += *it;

	for (iterator it2 = str.begin(); it2 != str.end(); ++it2) {
		temp += *it2;

		if (it != end())
			++it;
	}

	for ( ; it != end(); ++it)
		temp += *it;

	swap(temp);
}

void UString::erase(iterator from, iterator to) {
	if (from == end())
		return;

	UString temp;

	iterator it = begin();
	for ( ; it != from; ++it)
		temp += *it;

	for ( ; it != to; ++it);

	for ( ; it != end(); ++it)
		temp += *it;

	swap(temp);
}

void UString::erase(iterator pos) {
	iterator to = pos;
	erase(pos, ++to);
}

void UString::split(iterator splitPoint, UString &left, UString &right, bool remove) const {
	left.clear();
	right.clear();

	if (splitPoint == begin()) {
		right = *this;
		return;
	}
	if (splitPoint == end()) {
		left = *this;
		return;
	}

	iterator it = begin();
	for ( ; it != splitPoint; ++it)
		left += *it;

	if (remove)
		++it;

	for ( ; it != end(); ++it)
		right += *it;
}

void UString::splitTextTokens(const UString &text, std::vector<UString> &tokens) {
	UString collect;

	int state = 0;
	for (iterator it = text.begin(); it != text.end(); ++it) {
		uint32_t c = *it;

		if (state == 0) {
			// Collecting non-tokens

			if (c == '<') {
				tokens.push_back(collect);

				collect.clear();
				collect += c;

				state = 1;
			} else
				collect += c;

		} else if (state == 1) {
			// Collecting tokens

			if        (c == '<') {
				// Start of a token within a token
				// Add what we've collected to the last non-token

				tokens.back() += collect;

				collect.clear();
				collect += c;

			} else if (c == '>') {
				// End of the token

				collect += c;
				tokens.push_back(collect);

				collect.clear();
				state = 0;

			} else {
				// Still within a token

				collect += c;
			}

		}

	}

	if (collect.empty())
		return;

	// What's now collected is no full token
	if (state == 0)
		tokens.push_back(collect);
	else if (state == 1)
		tokens.back() += collect;
}

UString UString::substr(iterator from, iterator to) const {
	UString sub;

	iterator it = begin();
	for ( ; it != from; ++it);

	for ( ; it != to; ++it)
		sub += *it;

	return sub;
}

UString UString::format(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	return UString(buf);
}

size_t UString::split(const UString &text, uint32_t delim, std::vector<UString> &texts) {
	size_t length = 0;

	UString t = text;

	iterator point;
	while ((point = t.findFirst(delim)) != t.end()) {
		UString left, right;

		t.split(point, left, right, true);

		if (!left.empty()) {
			length = MAX(length, left.size());
			texts.push_back(left);
		}

		t = right;
	}

	if (!t.empty()) {
		length = MAX(length, t.size());
		texts.push_back(t);
	}

	return length;
}

void UString::recalculateSize() {
	try {
		// Calculate the "distance" in characters from the beginning and end
		_size = utf8::distance(_string.begin(), _string.end());
	} catch (const std::exception &se) {
		Exception e(se);
		throw e;
	}
}

// NOTE: If we ever need uppercase<->lowercase mappings for non-ASCII
//       characters: http://www.unicode.org/reports/tr21/tr21-5.html

uint32_t UString::toLower(uint32_t c) {
	if (!isASCII(c))
		// We don't know how to lowercase that
		return c;

	return std::tolower(c);
}

uint32_t UString::toUpper(uint32_t c) {
	if (!isASCII(c))
		// We don't know how to uppercase that
		return c;

	return std::toupper(c);
}

bool UString::isASCII(uint32_t c) {
	return (c & 0xFFFFFF80) == 0;
}

bool UString::isSpace(uint32_t c) {
	return isASCII(c) && std::isspace(c);
}

bool UString::isDigit(uint32_t c) {
	return isASCII(c) && std::isdigit(c);
}

bool UString::isAlpha(uint32_t c) {
	return isASCII(c) && std::isalpha(c);
}

bool UString::isAlNum(uint32_t c) {
	return isASCII(c) && std::isalnum(c);
}

bool UString::isCntrl(uint32_t c) {
	return isASCII(c) && std::iscntrl(c);
}

uint32_t UString::fromUTF16(uint16_t c) {
	std::string utf8result;

	try {
		utf8::utf16to8(&c, &c + 1, std::back_inserter(utf8result));
	} catch (const std::exception &se) {
		Exception e(se);
		throw e;
	}

	return *iterator(utf8result.begin(), utf8result.begin(), utf8result.end());
}

} // End of namespace Common

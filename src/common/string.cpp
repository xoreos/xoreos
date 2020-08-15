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
 *  Utility templates and functions for working with strings.
 */

#include <cstdarg>
#include <cstring>

#include <memory>
#include <system_error>

#include "src/common/string.h"

#include "external/utf8cpp/utf8.h"

namespace Common {
namespace String {

std::string format(const char *format, ...) {
	va_list va;
	va_start(va, format);
	std::string result = formatV(format, va);
	va_end(va);
	return result;
}

std::string formatV(const char *format, va_list args) {
	std::string result;

	// Allocate a small amount on the stack to avoid using the heap if we can
	// help it.
	char stackBuf[1024];

	va_list copy;
	va_copy(copy, args);

	int size = vsnprintf(stackBuf, sizeof(stackBuf), format, args);
	if (size < 0) {
		// FIXME: errno is POSIX-like. Who knows what Windows does.
		throw std::system_error(errno, std::generic_category(), "Invalid formatting string");
	} if (static_cast<size_t>(size) >= sizeof(stackBuf)) {
		std::unique_ptr<char[]> buf = std::make_unique<char[]>(static_cast<unsigned int>(size) + 1);
		vsnprintf(buf.get(), size, format, copy);
		result = std::string(buf.get(), size);
	} else {
		result = std::string(stackBuf, size);
	}

	va_end(copy);
	return result;
}

uint32_t fromUTF16(uint16_t c) {
	std::string utf8result;
	utf8::utf16to8(&c, &c + 1, std::back_inserter(utf8result));
	return *utf8::iterator<std::string::const_iterator>(utf8result.begin(), utf8result.begin(), utf8result.end());
}

int compareIgnoreCase(const std::string &left, const std::string &right) {
	return compareIgnoreCase(left.c_str(), right.c_str());
}

int compareIgnoreCase(const char *left, const char *right) {
#ifdef HAVE_STRCASECMP
	return strcasecmp(left, right);
#else
	const unsigned char *leftPtr = reinterpret_cast<const unsigned char *>(left);
	const unsigned char *rightPtr = reinterpret_cast<const unsigned char *>(right);

	if (leftPtr == rightPtr)
		return 0;

	int result;
	for (;;) {
		result = toLower(*leftPtr) - toLower(*rightPtr++);
		if (result != 0)
			break;

		if (*leftPtr++ == '\0')
			break;
	}

	return result;
#endif
}

} // End of namespace String
} // End of namespace Common

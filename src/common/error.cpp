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
 *  Basic exceptions to throw.
 */

#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#include "src/common/error.h"
#include "src/common/util.h"

namespace Common {

StackException::StackException() {
}

StackException::StackException(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	_stack.push(buf);
}

StackException::StackException(const StackException &e) : _stack(e._stack) {
}

StackException::StackException(const std::exception &e) {
	add(e);
}

StackException::~StackException() throw() {
}

void StackException::add(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	_stack.push(buf);
}

void StackException::add(const std::exception &e) {
	add("%s", e.what());
}

const char *StackException::what() const throw() {
	if (_stack.empty())
		return "";

	return _stack.top().c_str();
}

bool StackException::empty() const {
	return _stack.empty();
}

StackException::Stack &StackException::getStack() {
	return _stack;
}


const Exception kOpenError("Can't open file");
const Exception kReadError("Read error");
const Exception kSeekError("Seek error");
const Exception kWriteError("Write error");


void printException(Exception &e, const UString &prefix) {
	try {
		Exception::Stack &stack = e.getStack();

		if (stack.empty()) {
			status("FATAL ERROR");
			return;
		}

		status("%s%s", prefix.c_str(), stack.top().c_str());

		stack.pop();

		while (!stack.empty()) {
			status("    Because: %s", stack.top().c_str());
			stack.pop();
		}
	} catch (...) {
		status("FATAL ERROR: Exception while printing exception stack");
		std::exit(1);
	}
}

static void exceptionDispatcher(const char *prefix, const char *reason = "") {
	try {
		try {
			throw;
		} catch (Exception &e) {
			if (reason[0] != 0)
				e.add("%s", reason);

			printException(e, prefix);
		} catch (std::exception &e) {
			Exception se(e);
			if (reason[0] != 0)
				se.add("%s", reason);

			printException(se, prefix);
		} catch (...) {
			if (reason[0] != 0) {
				Exception se("%s", reason);
				printException(se, prefix);
			}
		}
	} catch (...) {
	}
}

void exceptionDispatcherError(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	exceptionDispatcher("ERROR: ", buf);
}

void exceptionDispatcherError() {
	exceptionDispatcher("ERROR: ");
}

void exceptionDispatcherWarning(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	exceptionDispatcher("WARNING: ", buf);
}

void exceptionDispatcherWarning() {
	exceptionDispatcher("WARNING: ");
}

} // End of namespace Common

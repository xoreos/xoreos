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

/** @file common/error.cpp
 *  Basic exceptions to throw.
 */

#include <cstdarg>
#include <cstdio>

#include "common/error.h"
#include "common/util.h"

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

const char *StackException::what() const throw() {
	if (_stack.empty())
		return "";

	return _stack.top().c_str();
}

StackException::Stack &StackException::getStack() {
	return _stack;
}


const Exception kOpenError("Can't open file");
const Exception kReadError("Read error");
const Exception kSeekError("Seek error");
const Exception kWriteError("Write error");


void printException(Exception &e, const UString &prefix) {
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
}

} // End of namespace Common

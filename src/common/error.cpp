/* eos - A reimplementation of BioWare's Common engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Common, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/error.cpp
 *  Basic exceptions to throw.
 */

#include <cstdarg>
#include <cstdio>

#include "common/error.h"
#include "common/util.h"

namespace Common {

StackException::StackException(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
	va_end(va);

	_stack.push(buf);
}

StackException::StackException(const StackException &e) {
	_stack = e._stack;
}

void StackException::add(const char *s, ...) {
	char buf[STRINGBUFLEN];
	va_list va;

	va_start(va, s);
	std::vsnprintf(buf, STRINGBUFLEN, s, va);
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

} // End of namespace Common

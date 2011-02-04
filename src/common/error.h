/* eos - A reimplementation of BioWare's Common engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Common, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/error.h
 *  Basic exceptions to throw.
 */

#ifndef COMMON_ERROR_H
#define COMMON_ERROR_H

#include <stack>
#include <exception>

#include "common/ustring.h"

namespace Common {

class StackException : public std::exception {
public:
	typedef std::stack<UString> Stack;

	StackException(const char *s, ...);
	StackException(const StackException &e);
	~StackException() throw();

	void add(const char *s, ...);

	const char *what() const throw();

	Stack &getStack();

private:
	Stack _stack;
};

typedef StackException Exception;

extern const Exception kOpenError;
extern const Exception kReadError;
extern const Exception kSeekError;

void printException(Exception &e, const UString &prefix = "ERROR: ");

} // End of namespace Common

#endif // COMMON_ERROR_H

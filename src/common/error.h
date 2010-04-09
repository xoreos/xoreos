/* eos - A reimplementation of BioWare's Common engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

#include <string>
#include <stack>
#include <exception>

namespace Common {

class StackException : public std::exception {
public:
	typedef std::stack<std::string> Stack;

	StackException(const char *s, ...);

	void add(const char *s, ...);

	const char *what() const throw();

	Stack &getStack();

private:
	Stack _stack;
};

typedef StackException Exception;

} // End of namespace Common

#endif // COMMON_ERROR_H

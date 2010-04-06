/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Copied verbatim from ScummVM

/** @file common/noncopyable.h
 *  Simple base class for classes that can't be copied.
 */

#ifndef COMMON_NONCOPYABLE_H
#define COMMON_NONCOPYABLE_H

namespace Common {

/**
 * Subclass of NonCopyable can not be copied due to the fact that
 * we made the copy constructor and assigment operator private.
 */
class NonCopyable {
public:
	NonCopyable() {}
private:
	// Prevent copying instances by accident
	NonCopyable(const NonCopyable&);
	NonCopyable& operator=(const NonCopyable&);
};

} // End of namespace Common

#endif // COMMON_NONCOPYABLE_H

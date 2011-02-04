/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/strutil.h
 *  Utility templates and functions for working with strings.
 */

#ifndef COMMON_STRUTIL_H
#define COMMON_STRUTIL_H

#include "common/types.h"

namespace Common {

class SeekableReadStream;

/** Print a quick hex dump of the given data. */
void printDataHex(SeekableReadStream &stream);
/** Print a quick hex dump of the given data. */
void printDataHex(const byte *data, uint32 size);

} // End of namespace Common

#endif // COMMON_STRUTIL_H

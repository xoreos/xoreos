/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/uuid.h
 *  Utility functions for generating unique IDs.
 */

#ifndef COMMON_UUID_H
#define COMMON_UUID_H

#include "common/ustring.h"

namespace Common {

UString generateIDRandomString();

} // End of namespace Common

#endif // COMMON_UUID_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/error.h
 *  Basic exceptions to throw when handling files used in Bioware's Aurora engine.
 */

#ifndef AURORA_ERROR_H
#define AURORA_ERROR_H

#include "common/error.h"

namespace Aurora {

extern const Common::Exception kGFFFieldTypeError;

} // End of namespace Aurora

#endif // AURORA_ERROR_H

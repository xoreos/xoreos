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

// Inspired by ScummVM's debug channels

/** @file
 *  Utility functions for debug output.
 */

#ifndef COMMON_DEBUG_H
#define COMMON_DEBUG_H

#include "common/system.h"
#include "common/types.h"
#include "common/debugman.h"

/** Print a debug message, but only if the current debug level is at least
 *  the specified level and if the specified channel is enabled.
 *
 *  Automatically appends a newline.
 */
void debugC(uint32 level, uint32 channel, const char *s, ...) GCC_PRINTF(3, 4);

/** Print a debug message, but only if the current debug level is at least
 *  the specified level and if the specified channel is enabled.
 *
 *  Doesn't automatically append a newline.
 */
void debugCN(uint32 level, uint32 channel, const char *s, ...) GCC_PRINTF(3, 4);

#endif // COMMON_DEBUG_H

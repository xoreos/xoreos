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

#include "src/common/system.h"
#include "src/common/types.h"
#include "src/common/debugman.h"

/** Print a debug message, but only if the current debug level is at least
 *  the specified level for the specified channel.
 *
 *  Automatically appends a newline.
 *
 *  The debug message is printed to both stderr and the global log file
 *  (if a global log file has been opened). See Common::DebugManager for
 *  details.
 */
void debugC(Common::DebugChannel channel, uint32_t level, const char *s, ...) GCC_PRINTF(3, 4);

/** Print a debug message, but only if the current debug level is at least
 *  the specified level for the specified channel.
 *
 *  Doesn't automatically append a newline.
 *
 *  The debug message is printed to both stderr and the global log file
 *  (if a global log file has been opened). See Common::DebugManager for
 *  details.
 */
void debugCN(Common::DebugChannel channel, uint32_t level, const char *s, ...) GCC_PRINTF(3, 4);

#endif // COMMON_DEBUG_H

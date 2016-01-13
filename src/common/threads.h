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

/** @file
 *  Threading system helpers.
 *
 *  See also class Thread in thread.h.
 */

#ifndef COMMON_THREADS_H
#define COMMON_THREADS_H

namespace Common {

/** Initialize the global threading system.
 *
 *  This needs to be done before any threads are instantiated.
 */
void initThreads();
/** Was the global threading system initialized? */
bool initedThreads();

/** Returns true if called from the main thread, false otherwise. */
bool isMainThread();
/** Throws an Exception if called from a non-main thread. */
void enforceMainThread();

} // End of namespace Common

#endif // COMMON_THREADS_H

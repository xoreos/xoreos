/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/hash.h
 *  Utility hash functions.
 */

#ifndef COMMON_HASH_H
#define COMMON_HASH_H

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

/** The algorithm used for hashing. */
enum HashAlgo {
	kHashNone  = -1, ///< No hashing at all.
	kHashDJB2  =  0, ///< djb2 hash function by Daniel J. Bernstein.
	kHashFNV32 =  1, ///< 32bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo.
	kHashFNV64 =  2, ///< 64bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo.
	kHashMAX         ///< For range checks.
};

/** djb2 hash function by Daniel J. Bernstein. */
static inline uint32 hashStringDJB2(const Common::UString &string) {
	uint32 hash = 5381;

	for (Common::UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = ((hash << 5) + hash) + *it;

	return hash;
}

/** 32bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo. */
static inline uint32 hashStringFNV32(const Common::UString &string) {
	uint32 hash = 0x811C9DC5;

	for (Common::UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = (hash * 16777619) ^ *it;

	return hash;
}

/** 64bit Fowler–Noll–Vo hash by Glenn Fowler, Landon Curt Noll and Phong Vo. */
static inline uint64 hashStringFNV64(const Common::UString &string) {
	uint64 hash = 0xCBF29CE484222325LL;

	for (Common::UString::iterator it = string.begin(); it != string.end(); ++it)
		hash = (hash * 1099511628211) ^ *it;

	return hash;
}

static inline uint64 hashString(const Common::UString &string, HashAlgo algo) {
	switch (algo) {
		case kHashDJB2:
			return hashStringDJB2(string);

		case kHashFNV32:
			return hashStringFNV32(string);

		case kHashFNV64:
			return hashStringFNV64(string);

		default:
			break;
	}

	return 0;
}

} // End of namespace Common

#endif // COMMON_HASH_H

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
 *  Low-level macros and functions to handle different endianness portably.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#ifndef COMMON_ENDIAN_H
#define COMMON_ENDIAN_H

#include "src/common/system.h"
#include "src/common/types.h"

// Sanity check
#if !defined(XOREOS_LITTLE_ENDIAN) && !defined(XOREOS_BIG_ENDIAN)
	#error No endianness defined
#endif

// Compatibility with non-clang compilers
// https://www.boost.org/doc/libs/1_61_0/boost/endian/detail/intrinsic.hpp
#ifndef __has_builtin
	#define __has_builtin(x) 0
#endif

#define SWAP_CONSTANT_64(a) \
	((uint64_t)((((a) >> 56) & 0x000000FF) | \
	            (((a) >> 40) & 0x0000FF00) | \
	            (((a) >> 24) & 0x00FF0000) | \
	            (((a) >>  8) & 0xFF000000) | \
	            (((a) & 0xFF000000) <<  8) | \
	            (((a) & 0x00FF0000) << 24) | \
	            (((a) & 0x0000FF00) << 40) | \
	            (((a) & 0x000000FF) << 56) ))

#define SWAP_CONSTANT_32(a) \
	((uint32_t)((((a) >> 24) & 0x00FF) | \
	            (((a) >>  8) & 0xFF00) | \
	            (((a) & 0xFF00) <<  8) | \
	            (((a) & 0x00FF) << 24) ))

#define SWAP_CONSTANT_16(a) \
	((uint16_t)((((a) >>  8) & 0x00FF) | \
	            (((a) <<  8) & 0xFF00) ))

// Test for GCC >= 4.3.0 as this version added the bswap32/bswap64 builtin. Also test for availability in Clang.
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 3))) || \
	(defined(__clang__) && __has_builtin(__builtin_bswap32) && __has_builtin(__builtin_bswap64))

	FORCEINLINE uint64_t SWAP_BYTES_64(uint64_t a) {
		return __builtin_bswap64(a);
	}

	FORCEINLINE uint32_t SWAP_BYTES_32(uint32_t a) {
		return __builtin_bswap32(a);
	}

// Test for MSVC 7 or newer
#elif defined(_MSC_VER) && _MSC_VER >= 1300

	#include <cstdlib>

	FORCEINLINE uint64_t SWAP_BYTES_64(uint64_t a) {
		return _byteswap_uint64(a);
	}

	FORCEINLINE uint32_t SWAP_BYTES_32(uint32_t a) {
		return _byteswap_ulong(a);
	}

// Generic fallback
#else

	static inline uint64_t SWAP_BYTES_64(uint64_t a) {
		const uint16_t lowlow   = (uint16_t) a;
		const uint16_t low      = (uint16_t) (a >> 16);
		const uint16_t high     = (uint16_t) (a >> 32);
		const uint16_t highhigh = (uint16_t) (a >> 48);

		return (((uint64_t)(((uint32_t)(uint16_t)((lowlow >> 8) | (lowlow << 8)) << 16)
			     | (uint16_t)((low >> 8) | (low << 8)))) << 32)
			   | ((uint32_t)(uint16_t)((high >> 8) | (high << 8)) << 16)
			     | (uint16_t)((highhigh >> 8) | (highhigh << 8));
	}

	static inline uint32_t SWAP_BYTES_32(uint32_t a) {
		const uint16_t low = (uint16_t)a, high = (uint16_t)(a >> 16);
		return ((uint32_t)(uint16_t)((low >> 8) | (low << 8)) << 16)
			   | (uint16_t)((high >> 8) | (high << 8));
	}

#endif

// Test for GCC >= 4.8 and Clang with bswap16 support
#if (defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8))) || \
	(defined(__clang__) && __has_builtin(__builtin_bswap16))

	FORCEINLINE uint16_t SWAP_BYTES_16(const uint16_t a) {
		return __builtin_bswap16(a);
	}

// Test for MSVC 7 or newer
#elif defined(_MSC_VER) && _MSC_VER >= 1300

	FORCEINLINE uint16_t SWAP_BYTES_16(const uint16_t a) {
		return _byteswap_ushort(a);
	}

// Generic fallback
#else

	static inline uint16_t SWAP_BYTES_16(const uint16_t a) {
		return (a >> 8) | (a << 8);
	}

#endif

/**
 * A wrapper macro used around four character constants, like 'DATA', to
 * ensure portability. Typical usage: MKTAG('D','A','T','A').
 *
 * Why is this necessary? The C/C++ standard does not define the endianness to
 * be used for character constants. Hence if one uses multi-byte character
 * constants, a potential portability problem opens up.
 */
#define MKTAG(a0,a1,a2,a3) ((uint32_t)((a3) | ((a2) << 8) | ((a1) << 16) | ((a0) << 24)))

/** A wrapper macro used around two character constants, like 'MZ'. */
#define MKTAG_16(a0,a1) ((uint16_t)((a1) | ((a0) << 8)))

// Functions for reading/writing native Integers,
// this transparently handles the need for alignment

#if !defined(XOREOS_NEED_ALIGNMENT)

	FORCEINLINE uint16_t READ_UINT16(const void *ptr) {
		return *(static_cast<const uint16_t *>(ptr));
	}

	FORCEINLINE uint32_t READ_UINT32(const void *ptr) {
		return *(static_cast<const uint32_t *>(ptr));
	}

	FORCEINLINE uint64_t READ_UINT64(const void *ptr) {
		return *(static_cast<const uint64_t *>(ptr));
	}

	FORCEINLINE void WRITE_UINT16(void *ptr, uint16_t value) {
		*static_cast<uint16_t *>(ptr) = value;
	}

	FORCEINLINE void WRITE_UINT32(void *ptr, uint32_t value) {
		*static_cast<uint32_t *>(ptr) = value;
	}

	FORCEINLINE void WRITE_UINT64(void *ptr, uint64_t value) {
		*static_cast<uint64_t *>(ptr) = value;
	}

// test for GCC >= 4.0. these implementations will automatically use CPU-specific
// instructions for unaligned data when they are available (eg. MIPS)
#elif defined(__GNUC__) && (__GNUC__ >= 4)

	FORCEINLINE uint16_t READ_UINT16(const void *ptr) {
		struct Unaligned16 { uint16_t val; } __attribute__ ((__packed__));
		return static_cast<const Unaligned16 *>(ptr)->val;
	}

	FORCEINLINE uint32_t READ_UINT32(const void *ptr) {
		struct Unaligned32 { uint32_t val; } __attribute__ ((__packed__));
		return static_cast<const Unaligned32 *>(ptr)->val;
	}

	FORCEINLINE uint64_t READ_UINT64(const void *ptr) {
		struct Unaligned64 { uint64_t val; } __attribute__ ((__packed__));
		return static_cast<const Unaligned64 *>(ptr)->val;
	}

	FORCEINLINE void WRITE_UINT16(void *ptr, uint16_t value) {
		struct Unaligned16 { uint16_t val; } __attribute__ ((__packed__));
		static_cast<Unaligned16 *>(ptr)->val = value;
	}

	FORCEINLINE void WRITE_UINT32(void *ptr, uint32_t value) {
		struct Unaligned32 { uint32_t val; } __attribute__ ((__packed__));
		static_cast<Unaligned32 *>(ptr)->val = value;
	}

	FORCEINLINE void WRITE_UINT64(void *ptr, uint64_t value) {
		struct Unaligned64 { uint64_t val; } __attribute__ ((__packed__));
		static_cast<Unaligned64 *>(ptr)->val = value;
	}

// use software fallback by loading each byte explicitly
#else

	#if defined(XOREOS_LITTLE_ENDIAN)

		static inline uint16_t READ_UINT16(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint16_t)b[1] << 8) | (uint16_t)b[0];
		}
		static inline uint32_t READ_UINT32(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint32_t)b[3] << 24) | (uint32_t)(b[2] << 16) | (uint32_t)(b[1] << 8) | (uint32_t)(b[0]);
		}
		static inline uint64_t READ_UINT64(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint64_t)b[7] << 56) | ((uint64_t)b[6] << 48) | ((uint64_t)b[5] << 40) | ((uint64_t)b[4] << 32) |
			       ((uint64_t)b[3] << 24) | ((uint64_t)b[2] << 16) | ((uint64_t)b[1] <<  8) | ((uint64_t)b[0]);
		}
		static inline void WRITE_UINT16(void *ptr, uint16_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 0);
			b[1] = (uint8_t)(value >> 8);
		}
		static inline void WRITE_UINT32(void *ptr, uint32_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >>  0);
			b[1] = (uint8_t)(value >>  8);
			b[2] = (uint8_t)(value >> 16);
			b[3] = (uint8_t)(value >> 24);
		}
		static inline void WRITE_UINT64(void *ptr, uint64_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >>  0);
			b[1] = (uint8_t)(value >>  8);
			b[2] = (uint8_t)(value >> 16);
			b[3] = (uint8_t)(value >> 24);
			b[4] = (uint8_t)(value >> 32);
			b[5] = (uint8_t)(value >> 40);
			b[6] = (uint8_t)(value >> 48);
			b[7] = (uint8_t)(value >> 56);
		}

	#elif defined(XOREOS_BIG_ENDIAN)

		static inline uint16_t READ_UINT16(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint16_t)b[0] << 8) | (uint16_t)b[1];
		}
		static inline uint32_t READ_UINT32(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | ((uint32_t)b[3]);
		}
		static inline uint64_t READ_UINT64(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint64_t)b[0] << 56) | ((uint64_t)b[1] << 48) | ((uint64_t)b[2] << 40) | ((uint64_t)b[3] << 32) |
			       ((uint64_t)b[4] << 24) | ((uint64_t)b[5] << 16) | ((uint64_t)b[6] <<  8) | ((uint64_t)b[7]);
		}
		static inline void WRITE_UINT16(void *ptr, uint16_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 8);
			b[1] = (uint8_t)(value >> 0);
		}
		static inline void WRITE_UINT32(void *ptr, uint32_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 24);
			b[1] = (uint8_t)(value >> 16);
			b[2] = (uint8_t)(value >>  8);
			b[3] = (uint8_t)(value >>  0);
		}
		static inline void WRITE_UINT64(void *ptr, uint64_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 56);
			b[1] = (uint8_t)(value >> 48);
			b[2] = (uint8_t)(value >> 40);
			b[3] = (uint8_t)(value >> 32);
			b[4] = (uint8_t)(value >> 24);
			b[5] = (uint8_t)(value >> 16);
			b[6] = (uint8_t)(value >>  8);
			b[7] = (uint8_t)(value >>  0);
		}

	#endif

#endif

//  Map Functions for reading/writing BE/LE integers depending on native endianness
#if defined(XOREOS_LITTLE_ENDIAN)

	#define READ_LE_UINT16(a) READ_UINT16(a)
	#define READ_LE_UINT32(a) READ_UINT32(a)
	#define READ_LE_UINT64(a) READ_UINT64(a)

	#define WRITE_LE_UINT16(a, v) WRITE_UINT16(a, v)
	#define WRITE_LE_UINT32(a, v) WRITE_UINT32(a, v)
	#define WRITE_LE_UINT64(a, v) WRITE_UINT64(a, v)

	#define FROM_LE_16(a) ((uint16_t)(a))
	#define FROM_LE_32(a) ((uint32_t)(a))
	#define FROM_LE_64(a) ((uint64_t)(a))

	#define FROM_BE_16(a) SWAP_BYTES_16(a)
	#define FROM_BE_32(a) SWAP_BYTES_32(a)
	#define FROM_BE_64(a) SWAP_BYTES_64(a)

	#define TO_LE_16(a) ((uint16_t)(a))
	#define TO_LE_32(a) ((uint32_t)(a))
	#define TO_LE_64(a) ((uint64_t)(a))

	#define TO_BE_16(a) SWAP_BYTES_16(a)
	#define TO_BE_32(a) SWAP_BYTES_32(a)
	#define TO_BE_64(a) SWAP_BYTES_64(a)

	#define CONSTANT_LE_16(a) ((uint16_t)(a))
	#define CONSTANT_LE_32(a) ((uint32_t)(a))
	#define CONSTANT_LE_64(a) ((uint64_t)(a))

	#define CONSTANT_BE_16(a) SWAP_CONSTANT_16(a)
	#define CONSTANT_BE_32(a) SWAP_CONSTANT_32(a)
	#define CONSTANT_BE_64(a) SWAP_CONSTANT_64(a)

// if the unaligned load and the byteswap take a lot instructions its better to directly read and invert
	#if defined(XOREOS_NEED_ALIGNMENT) && !defined(__mips__)

		static inline uint16_t READ_BE_UINT16(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint16_t)b[0] << 8) | (uint16_t)b[1];
		}
		static inline uint32_t READ_BE_UINT32(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) | ((uint32_t)b[2] << 8) | ((uint32_t)b[3]);
		}
		static inline uint32_t READ_BE_UINT64(const void *ptr) {
			const uint8_t *b = static_cast<const uint8_t *>(ptr);
			return ((uint64_t)b[0] << 56) | ((uint64_t)b[1] << 48) | ((uint64_t)b[2] << 40) | ((uint64_t)b[3] << 32) |
			       ((uint64_t)b[4] << 24) | ((uint64_t)b[5] << 16) | ((uint64_t)b[6] <<  8) | ((uint64_t)b[7]);
		}
		static inline void WRITE_BE_UINT16(void *ptr, uint16_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 8);
			b[1] = (uint8_t)(value >> 0);
		}
		static inline void WRITE_BE_UINT32(void *ptr, uint32_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 24);
			b[1] = (uint8_t)(value >> 16);
			b[2] = (uint8_t)(value >>  8);
			b[3] = (uint8_t)(value >>  0);
		}
		static inline void WRITE_BE_UINT64(void *ptr, uint64_t value) {
			uint8_t *b = static_cast<uint8_t *>(ptr);
			b[0] = (uint8_t)(value >> 56);
			b[1] = (uint8_t)(value >> 48);
			b[2] = (uint8_t)(value >> 40);
			b[3] = (uint8_t)(value >> 32);
			b[4] = (uint8_t)(value >> 24);
			b[5] = (uint8_t)(value >> 16);
			b[6] = (uint8_t)(value >>  8);
			b[7] = (uint8_t)(value >>  0);
		}

	#else

		static inline uint16_t READ_BE_UINT16(const void *ptr) {
			return SWAP_BYTES_16(READ_UINT16(ptr));
		}
		static inline uint32_t READ_BE_UINT32(const void *ptr) {
			return SWAP_BYTES_32(READ_UINT32(ptr));
		}
		static inline uint64_t READ_BE_UINT64(const void *ptr) {
			return SWAP_BYTES_64(READ_UINT64(ptr));
		}
		static inline void WRITE_BE_UINT16(void *ptr, uint16_t value) {
			WRITE_UINT16(ptr, SWAP_BYTES_16(value));
		}
		static inline void WRITE_BE_UINT32(void *ptr, uint32_t value) {
			WRITE_UINT32(ptr, SWAP_BYTES_32(value));
		}
		static inline void WRITE_BE_UINT64(void *ptr, uint64_t value) {
			WRITE_UINT64(ptr, SWAP_BYTES_64(value));
		}

	#endif // if defined(XOREOS_NEED_ALIGNMENT)

#elif defined(XOREOS_BIG_ENDIAN)

	#define READ_BE_UINT16(a) READ_UINT16(a)
	#define READ_BE_UINT32(a) READ_UINT32(a)
	#define READ_BE_UINT64(a) READ_UINT64(a)

	#define WRITE_BE_UINT16(a, v) WRITE_UINT16(a, v)
	#define WRITE_BE_UINT32(a, v) WRITE_UINT32(a, v)
	#define WRITE_BE_UINT64(a, v) WRITE_UINT64(a, v)

	#define FROM_LE_16(a) SWAP_BYTES_16(a)
	#define FROM_LE_32(a) SWAP_BYTES_32(a)
	#define FROM_LE_64(a) SWAP_BYTES_64(a)

	#define FROM_BE_16(a) ((uint16_t)(a))
	#define FROM_BE_32(a) ((uint32_t)(a))
	#define FROM_BE_64(a) ((uint64_t)(a))

	#define TO_LE_16(a) SWAP_BYTES_16(a)
	#define TO_LE_32(a) SWAP_BYTES_32(a)
	#define TO_LE_64(a) SWAP_BYTES_64(a)

	#define TO_BE_16(a) ((uint16_t)(a))
	#define TO_BE_32(a) ((uint32_t)(a))
	#define TO_BE_64(a) ((uint64_t)(a))

	#define CONSTANT_LE_16(a) SWAP_CONSTANT_16(a)
	#define CONSTANT_LE_32(a) SWAP_CONSTANT_32(a)
	#define CONSTANT_LE_64(a) SWAP_CONSTANT_64(a)

	#define CONSTANT_BE_16(a) ((uint16_t)(a))
	#define CONSTANT_BE_32(a) ((uint32_t)(a))
	#define CONSTANT_BE_64(a) ((uint64_t)(a))

	// if the unaligned load and the byteswap take a lot instructions its better to directly read and invert
	#if defined(XOREOS_NEED_ALIGNMENT) && !defined(__mips__)

	static inline uint16_t READ_LE_UINT16(const void *ptr) {
		const uint8_t *b = static_cast<const uint8_t *>(ptr);
		return ((uint16_t)b[1] << 8) | (uint16_t)b[0];
	}
	static inline uint32_t READ_LE_UINT32(const void *ptr) {
		const uint8_t *b = static_cast<const uint8_t *>(ptr);
		return ((uint32_t)b[3] << 24) | ((uint32_t)b[2] << 16) | ((uint32_t)b[1] << 8) | ((uint32_t)b[0]);
	}
	static inline uint64_t READ_LE_UINT64(const void *ptr) {
		const uint8_t *b = static_cast<const uint8_t *>(ptr);
		return ((uint64_t)b[7] << 56) | ((uint64_t)b[6] << 48) | ((uint64_t)b[5] << 40) | ((uint64_t)b[4] << 32) |
		       ((uint64_t)b[3] << 24) | ((uint64_t)b[2] << 16) | ((uint64_t)b[1] <<  8) | ((uint64_t)b[0]);
	}
	static inline void WRITE_LE_UINT16(void *ptr, uint16_t value) {
		uint8_t *b = static_cast<uint8_t *>(ptr);
		b[0] = (uint8_t)(value >> 0);
		b[1] = (uint8_t)(value >> 8);
	}
	static inline void WRITE_LE_UINT32(void *ptr, uint32_t value) {
		uint8_t *b = static_cast<uint8_t *>(ptr);
		b[0] = (uint8_t)(value >>  0);
		b[1] = (uint8_t)(value >>  8);
		b[2] = (uint8_t)(value >> 16);
		b[3] = (uint8_t)(value >> 24);
	}
	static inline void WRITE_LE_UINT64(void *ptr, uint64_t value) {
		uint8_t *b = static_cast<uint8_t *>(ptr);
		b[0] = (uint8_t)(value >>  0);
		b[1] = (uint8_t)(value >>  8);
		b[2] = (uint8_t)(value >> 16);
		b[3] = (uint8_t)(value >> 24);
		b[4] = (uint8_t)(value >> 32);
		b[5] = (uint8_t)(value >> 40);
		b[6] = (uint8_t)(value >> 48);
		b[7] = (uint8_t)(value >> 56);
	}

	#else

	static inline uint16_t READ_LE_UINT16(const void *ptr) {
		return SWAP_BYTES_16(READ_UINT16(ptr));
	}
	static inline uint32_t READ_LE_UINT32(const void *ptr) {
		return SWAP_BYTES_32(READ_UINT32(ptr));
	}
	static inline uint64_t READ_LE_UINT64(const void *ptr) {
		return SWAP_BYTES_64(READ_UINT64(ptr));
	}
	static inline void WRITE_LE_UINT16(void *ptr, uint16_t value) {
		WRITE_UINT16(ptr, SWAP_BYTES_16(value));
	}
	static inline void WRITE_LE_UINT32(void *ptr, uint32_t value) {
		WRITE_UINT32(ptr, SWAP_BYTES_32(value));
	}
	static inline void WRITE_LE_UINT64(void *ptr, uint64_t value) {
		WRITE_UINT64(ptr, SWAP_BYTES_64(value));
	}

	#endif // if defined(XOREOS_NEED_ALIGNMENT)

#endif // if defined(XOREOS_LITTLE_ENDIAN)

#endif // COMMON_ENDIAN_H

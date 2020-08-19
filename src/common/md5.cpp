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
 *  Hashing/digesting using the MD5 algorithm.
 */

#include <cstring>

#include "src/common/md5.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"

namespace Common {

/* .--- MD5, based on the implementation by Alexander Peslyak ---.
 *
 * This is an MD5 digesting implementation based on the implementation by
 * Alexander Peslyak, better known as Solar Designer <solar at openwall.com>.
 * Alexander Peslyak's implementation was released to the public domain.
 *
 * <http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5>
 *
 * The original copyright note on md5.c reads as follows:
 *
 * This software was written by Alexander Peslyak in 2001.  No copyright is
 * claimed, and the software is hereby placed in the public domain.
 * In case this attempt to disclaim copyright and place the software in the
 * public domain is deemed null and void, then the software is
 * Copyright (c) 2001 Alexander Peslyak and it is hereby released to the
 * general public under the following terms:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted.
 *
 * There's ABSOLUTELY NO WARRANTY, express or implied.
 *
 * (This is a heavily cut-down "BSD license".)
 *
 * This differs from Colin Plumb's older public domain implementation in that
 * no exactly 32-bit integer data type is required (any 32-bit or wider
 * unsigned integer data type will do), there's no compile-time endianness
 * configuration, and the function prototypes match OpenSSL's.  No code from
 * Colin Plumb's implementation has been reused; this comment merely compares
 * the properties of the two independent implementations.
 *
 * The primary goals of this implementation are portability and ease of use.
 * It is meant to be fast, but not as fast as possible.  Some known
 * optimizations are not included to reduce source code size and avoid
 * compile-time configuration.
 */
struct MD5Context {
	uint32_t lo, hi;
	uint32_t a, b, c, d;
	byte buffer[64];
	uint32_t block[16];

	MD5Context() : lo(0), hi(0), a(0x67452301), b(0xEFCDAB89), c(0x98BADCFE), d(0x10325476) {
	}

	~MD5Context() {
		/* We don't care about security here, so we do *not* zeroize the buffers.
		 * Residuals of the hashing *will* be left in memory!
		 *
		 * WARNING: DO NOT USE THIS CODE IN SECURITY RELEVANT SITUATIONS!
		 */
	}
};

/*
 * The basic MD5 functions.
 *
 * F and G are optimized compared to their RFC 1321 definitions for
 * architectures that lack an AND-NOT instruction, just like in Colin Plumb's
 * implementation.
 */
#define F(x, y, z)     ((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)     ((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)     (((x) ^ (y)) ^ (z))
#define H2(x, y, z)    ((x) ^ ((y) ^ (z)))
#define I(x, y, z)     ((y) ^ ((x) | ~(z)))

/*
 * The MD5 transformation for all four rounds.
 */
#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xFFFFFFFF) >> (32 - (s)))); \
	(a) += (b);

/*
 * SET reads 4 input bytes in little-endian byte order and stores them
 * in a properly aligned word in host byte order.
 *
 * The check for little-endian architectures that tolerate unaligned
 * memory accesses is just an optimization.  Nothing will break if it
 * doesn't work.
 */
#if defined(__i386__) || defined(__x86_64__) || defined(__vax__)
#define SET(n) \
	(*const_cast<uint32_t *>(reinterpret_cast<const uint32_t *>(&ptr[(n) * 4])))
#define GET(n) \
	SET(n)
#else
#define SET(n) \
	(ctx.block[(n)] = \
	(uint32_t)ptr[(n) * 4] | \
	((uint32_t)ptr[(n) * 4 + 1] << 8) | \
	((uint32_t)ptr[(n) * 4 + 2] << 16) | \
	((uint32_t)ptr[(n) * 4 + 3] << 24))
#define GET(n) \
	(ctx.block[(n)])
#endif

/*
 * This processes one or more 64-byte data blocks, but does NOT update
 * the bit counters.  There are no alignment requirements.
 */
static const byte *md5Body(MD5Context &ctx, const byte *data, size_t size) {
	uint32_t a = ctx.a;
	uint32_t b = ctx.b;
	uint32_t c = ctx.c;
	uint32_t d = ctx.d;

	const byte *ptr = data;

	do {
		uint32_t saved_a = a;
		uint32_t saved_b = b;
		uint32_t saved_c = c;
		uint32_t saved_d = d;

/* Round 1 */
		STEP(F, a, b, c, d, SET(0), 0xD76AA478, 7)
		STEP(F, d, a, b, c, SET(1), 0xE8C7B756, 12)
		STEP(F, c, d, a, b, SET(2), 0x242070DB, 17)
		STEP(F, b, c, d, a, SET(3), 0xC1BDCEEE, 22)
		STEP(F, a, b, c, d, SET(4), 0xF57C0FAF, 7)
		STEP(F, d, a, b, c, SET(5), 0x4787C62A, 12)
		STEP(F, c, d, a, b, SET(6), 0xA8304613, 17)
		STEP(F, b, c, d, a, SET(7), 0xFD469501, 22)
		STEP(F, a, b, c, d, SET(8), 0x698098D8, 7)
		STEP(F, d, a, b, c, SET(9), 0x8B44F7AF, 12)
		STEP(F, c, d, a, b, SET(10), 0xFFFF5BB1, 17)
		STEP(F, b, c, d, a, SET(11), 0x895CD7BE, 22)
		STEP(F, a, b, c, d, SET(12), 0x6B901122, 7)
		STEP(F, d, a, b, c, SET(13), 0xFD987193, 12)
		STEP(F, c, d, a, b, SET(14), 0xA679438E, 17)
		STEP(F, b, c, d, a, SET(15), 0x49B40821, 22)

/* Round 2 */
		STEP(G, a, b, c, d, GET(1), 0xF61E2562, 5)
		STEP(G, d, a, b, c, GET(6), 0xC040B340, 9)
		STEP(G, c, d, a, b, GET(11), 0x265E5A51, 14)
		STEP(G, b, c, d, a, GET(0), 0xE9B6C7AA, 20)
		STEP(G, a, b, c, d, GET(5), 0xD62F105D, 5)
		STEP(G, d, a, b, c, GET(10), 0x02441453, 9)
		STEP(G, c, d, a, b, GET(15), 0xD8A1E681, 14)
		STEP(G, b, c, d, a, GET(4), 0xE7D3FBC8, 20)
		STEP(G, a, b, c, d, GET(9), 0x21E1CDE6, 5)
		STEP(G, d, a, b, c, GET(14), 0xC33707D6, 9)
		STEP(G, c, d, a, b, GET(3), 0xF4D50D87, 14)
		STEP(G, b, c, d, a, GET(8), 0x455A14ED, 20)
		STEP(G, a, b, c, d, GET(13), 0xA9E3E905, 5)
		STEP(G, d, a, b, c, GET(2), 0xFCEFA3F8, 9)
		STEP(G, c, d, a, b, GET(7), 0x676F02D9, 14)
		STEP(G, b, c, d, a, GET(12), 0x8D2A4C8A, 20)

/* Round 3 */
		STEP(H, a, b, c, d, GET(5), 0xFFFA3942, 4)
		STEP(H2, d, a, b, c, GET(8), 0x8771F681, 11)
		STEP(H, c, d, a, b, GET(11), 0x6D9D6122, 16)
		STEP(H2, b, c, d, a, GET(14), 0xFDE5380C, 23)
		STEP(H, a, b, c, d, GET(1), 0xA4BEEA44, 4)
		STEP(H2, d, a, b, c, GET(4), 0x4BDECFA9, 11)
		STEP(H, c, d, a, b, GET(7), 0xF6BB4B60, 16)
		STEP(H2, b, c, d, a, GET(10), 0xBEBFBC70, 23)
		STEP(H, a, b, c, d, GET(13), 0x289B7EC6, 4)
		STEP(H2, d, a, b, c, GET(0), 0xEAA127FA, 11)
		STEP(H, c, d, a, b, GET(3), 0xD4EF3085, 16)
		STEP(H2, b, c, d, a, GET(6), 0x04881D05, 23)
		STEP(H, a, b, c, d, GET(9), 0xD9D4D039, 4)
		STEP(H2, d, a, b, c, GET(12), 0xE6DB99E5, 11)
		STEP(H, c, d, a, b, GET(15), 0x1FA27CF8, 16)
		STEP(H2, b, c, d, a, GET(2), 0xC4AC5665, 23)

/* Round 4 */
		STEP(I, a, b, c, d, GET(0), 0xF4292244, 6)
		STEP(I, d, a, b, c, GET(7), 0x432AFF97, 10)
		STEP(I, c, d, a, b, GET(14), 0xAB9423A7, 15)
		STEP(I, b, c, d, a, GET(5), 0xFC93A039, 21)
		STEP(I, a, b, c, d, GET(12), 0x655B59C3, 6)
		STEP(I, d, a, b, c, GET(3), 0x8F0CCC92, 10)
		STEP(I, c, d, a, b, GET(10), 0xFFEFF47D, 15)
		STEP(I, b, c, d, a, GET(1), 0x85845DD1, 21)
		STEP(I, a, b, c, d, GET(8), 0x6FA87E4F, 6)
		STEP(I, d, a, b, c, GET(15), 0xFE2CE6E0, 10)
		STEP(I, c, d, a, b, GET(6), 0xA3014314, 15)
		STEP(I, b, c, d, a, GET(13), 0x4E0811A1, 21)
		STEP(I, a, b, c, d, GET(4), 0xF7537E82, 6)
		STEP(I, d, a, b, c, GET(11), 0xBD3AF235, 10)
		STEP(I, c, d, a, b, GET(2), 0x2AD7D2BB, 15)
		STEP(I, b, c, d, a, GET(9), 0xEB86D391, 21)

		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;

		ptr += 64;
	} while (size -= 64);

	ctx.a = a;
	ctx.b = b;
	ctx.c = c;
	ctx.d = d;

	return ptr;
}

static void md5Update(MD5Context &ctx, const byte *data, size_t size) {
	uint32_t saved_lo = ctx.lo;
	if ((ctx.lo = (saved_lo + size) & 0x1FFFFFFF) < saved_lo)
		ctx.hi++;
	ctx.hi += size >> 29;

	size_t used = saved_lo & 0x3F;

	if (used) {
		size_t available = 64 - used;

		if (size < available) {
			std::memcpy(&ctx.buffer[used], data, size);
			return;
		}

		std::memcpy(&ctx.buffer[used], data, available);
		data = data + available;
		size -= available;
		md5Body(ctx, ctx.buffer, 64);
	}

	if (size >= 64) {
		data = md5Body(ctx, data, size & ~(size_t)0x3F);
		size &= 0x3F;
	}

	std::memcpy(ctx.buffer, data, size);
}

static void md5Final(byte *result, MD5Context &ctx) {
	size_t used = ctx.lo & 0x3F;

	ctx.buffer[used++] = 0x80;

	size_t available = 64 - used;

	if (available < 8) {
		std::memset(&ctx.buffer[used], 0, available);
		md5Body(ctx, ctx.buffer, 64);
		used = 0;
		available = 64;
	}

	std::memset(&ctx.buffer[used], 0, available - 8);

	ctx.lo <<= 3;
	ctx.buffer[56] = ctx.lo;
	ctx.buffer[57] = ctx.lo >> 8;
	ctx.buffer[58] = ctx.lo >> 16;
	ctx.buffer[59] = ctx.lo >> 24;
	ctx.buffer[60] = ctx.hi;
	ctx.buffer[61] = ctx.hi >> 8;
	ctx.buffer[62] = ctx.hi >> 16;
	ctx.buffer[63] = ctx.hi >> 24;

	md5Body(ctx, ctx.buffer, 64);

	result[0] = ctx.a;
	result[1] = ctx.a >> 8;
	result[2] = ctx.a >> 16;
	result[3] = ctx.a >> 24;
	result[4] = ctx.b;
	result[5] = ctx.b >> 8;
	result[6] = ctx.b >> 16;
	result[7] = ctx.b >> 24;
	result[8] = ctx.c;
	result[9] = ctx.c >> 8;
	result[10] = ctx.c >> 16;
	result[11] = ctx.c >> 24;
	result[12] = ctx.d;
	result[13] = ctx.d >> 8;
	result[14] = ctx.d >> 16;
	result[15] = ctx.d >> 24;
}
// '--- MD5, based on the implementation by Alexander Peslyak ---'


void hashMD5(ReadStream &stream, std::vector<byte> &digest) {
	MD5Context ctx;

	byte buf[4096];
	while (!stream.eos()) {
		size_t bufRead = stream.read(buf, 4096);

		md5Update(ctx, buf, bufRead);
	}

	digest.resize(kMD5Length);
	md5Final(&digest[0], ctx);
}

void hashMD5(const byte *data, size_t dataLength, std::vector<byte> &digest) {
	MD5Context ctx;

	md5Update(ctx, data, dataLength);

	digest.resize(kMD5Length);
	md5Final(&digest[0], ctx);
}

void hashMD5(const UString &string, std::vector<byte> &digest) {
	hashMD5(reinterpret_cast<const byte *>(string.c_str()), std::strlen(string.c_str()), digest);
}

void hashMD5(const std::vector<byte> &data, std::vector<byte> &digest) {
	hashMD5(&data[0], data.size(), digest);
}


bool compareMD5Digest(ReadStream &stream, const std::vector<byte> &digest) {
	if (digest.size() != kMD5Length)
		return false;

	std::vector<byte> newDigest;
	hashMD5(stream, newDigest);

	return std::memcmp(&digest[0], &newDigest[0], kMD5Length) == 0;
}

bool compareMD5Digest(const byte *data, size_t dataLength, const std::vector<byte> &digest) {
	if (digest.size() != kMD5Length)
		return false;

	std::vector<byte> newDigest;
	hashMD5(data, dataLength, newDigest);

	return std::memcmp(&digest[0], &newDigest[0], kMD5Length) == 0;
}

bool compareMD5Digest(const UString &string, const std::vector<byte> &digest) {
	return compareMD5Digest(reinterpret_cast<const byte *>(string.c_str()), std::strlen(string.c_str()), digest);
}

bool compareMD5Digest(const std::vector<byte> &data, const std::vector<byte> &digest) {
	return compareMD5Digest(&data[0], data.size(), digest);
}

} // End of namespace Common

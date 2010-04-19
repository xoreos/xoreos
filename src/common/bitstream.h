/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/bitstream.h
 *  A bit stream.
 */

#ifndef COMMON_BITSTREAM_H
#define COMMON_BITSTREAM_H

#include "common/types.h"

namespace Common {

class SeekableReadStream;

/** A bit stream. */
class BitStream {
public:
	BitStream();
	virtual ~BitStream();

	/** Read a number of bits. */
	uint64 getBits(int n);

	/** Get the current position. */
	virtual uint32 pos()  const = 0;
	/** Return the number of bits in the stream. */
	virtual uint32 size() const = 0;

protected:
	/** Read a bit. */
	virtual int getBit() = 0;
};

/** A big endian bit stream. */
class BitStreamBE : public BitStream {
public:
	BitStreamBE(SeekableReadStream &stream, uint32 bitCount);
	BitStreamBE(const byte *data, uint32 bitCount);
	~BitStreamBE();

	uint32 pos()  const;
	uint32 size() const;

protected:
	int getBit();

private:
	SeekableReadStream *_stream;

	byte  _value;   ///< Current byte.
	uint8 _inValue; ///< Position within the current byte.
};

/** A 32bit little endian bit stream. */
class BitStream32LE : public BitStream {
public:
	BitStream32LE(SeekableReadStream &stream, uint32 bitCount);
	BitStream32LE(const byte *data, uint32 bitCount);
	~BitStream32LE();

	uint32 pos()  const;
	uint32 size() const;

protected:
	int getBit();

private:
	SeekableReadStream *_stream;

	uint32 _value;   ///< Current 32bit value.
	uint8  _inValue; ///< Position within the current 32bit value.
};

} // End of namespace Common

#endif // COMMON_BITSTREAM_H

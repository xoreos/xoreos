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
 *  A bit stream writer.
 */

#ifndef COMMON_BITSTREAMWRITER_H
#define COMMON_BITSTREAMWRITER_H

#include <cassert>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/disposableptr.h"
#include "src/common/error.h"
#include "src/common/writestream.h"

namespace Common {

/** A bit stream writer. */
class BitStreamWriter {
public:
	virtual ~BitStreamWriter() {
	}

	/** Write a bit to the bit stream. */
	virtual void putBit(bool bit) = 0;

	/** Write a multi-bit value to the bit stream. */
	virtual void putBits(uint32_t bits, size_t n) = 0;

	/** Flush the stream, forcing all cached bits to the written. */
	virtual void flush() = 0;

protected:
	BitStreamWriter() {
	}
};

/**
 * A template implementing a bit stream writer for different data memory layouts.
 *
 * Such a bit stream writer accepts bits one at a time, and writes them in
 * chunks of valueBits bits to the data stream.
 *
 * For example, a bit stream writer with the layout parameters 32, true, false
 * for valueBits, isLE and isMSB2LSB, writes 32bit little-endian values to
 * the data stream, ordering the bits LSB to MSB.
 */
template<int valueBits, bool isLE, bool isMSB2LSB>
class BitStreamWriterImpl : boost::noncopyable, public BitStreamWriter {
private:
	DisposablePtr<WriteStream> _stream; ///< The output stream.

	uint64_t _value;   ///< Current value.
	uint8_t  _inValue; ///< Position within the current value.

	/** Write a data value. */
	inline void writeData(uint64_t data) {
		if (isLE) {
			if (valueBits ==  8)
				_stream->writeByte(data);
			if (valueBits == 16)
				_stream->writeUint16LE(data);
			if (valueBits == 32)
				_stream->writeUint32LE(data);
			if (valueBits == 64)
				_stream->writeUint64LE(data);
		} else {
			if (valueBits ==  8)
				_stream->writeByte(data);
			if (valueBits == 16)
				_stream->writeUint16BE(data);
			if (valueBits == 32)
				_stream->writeUint32BE(data);
			if (valueBits == 64)
				_stream->writeUint64BE(data);
		}
	}

	/** Write the full data value. */
	inline void writeValue() {
		/* If we're writing the bits LSB first, they accumulate at the upper parts of the value data.
		 * We need to shift them down before writing it to the byte stream. */
		if (!isMSB2LSB)
			_value >>= 64 - valueBits;

		writeData(_value);
		_value = 0;
	}

public:
	/** Create a bit stream using this input data stream and optionally delete it on destruction. */
	BitStreamWriterImpl(WriteStream *stream, bool disposeAfterUse = false) :
		_stream(stream, disposeAfterUse), _value(0), _inValue(0) {

		assert(_stream);

		if ((valueBits != 8) && (valueBits != 16) && (valueBits != 32) && (valueBits != 64))
			throw Exception("BitStreamWriter: Invalid memory layout %d, %d, %d", valueBits, isLE, isMSB2LSB);
	}

	/** Create a bit stream using this input data stream. */
	BitStreamWriterImpl(WriteStream &stream) :
		_stream(&stream, false), _value(0), _inValue(0) {

		if ((valueBits != 8) && (valueBits != 16) && (valueBits != 32) && (valueBits != 64))
			throw Exception("BitStreamWriter: Invalid memory layout %d, %d, %d", valueBits, isLE, isMSB2LSB);
	}

	~BitStreamWriterImpl() {
	}

	/** Write a bit to the bit stream. */
	void putBit(bool bit) {
		// Put the bit, according to the writing direction
		if (isMSB2LSB)
			_value = (_value << 1) | (bit ? 1 : 0);
		else
			_value = (_value >> 1) | (static_cast<uint64_t>(bit ? 1 : 0) << 63);

		// Increase the position within the current value
		_inValue = (_inValue + 1) % valueBits;

		// Check if we have a full value to write
		if (_inValue == 0)
			writeValue();
	}

	/** Write a multi-bit value to the bit stream. */
	void putBits(uint32_t bits, size_t n) {
		if (n == 0)
			return;

		if (n > 32)
			throw Exception("Too many bits requested to be written");

		if (isMSB2LSB)
			bits <<= 32 - n;

		while (n-- > 0) {
			if (isMSB2LSB) {
				putBit(bits & 0x80000000);
				bits <<= 1;
			} else {
				putBit(bits & 1);
				bits >>= 1;
			}
		}
	}

	/** Flush the stream, forcing all cached bits to the written.
	 *
	 *  This will resync the stream back to valueBits boundaries, padding with 0 bits
	 */
	void flush() {
		if (_inValue > 0) {
			const size_t padding = valueBits - _inValue;
			if (isMSB2LSB)
				_value <<= padding;
			else
				_value >>= padding;

			writeValue();
			_inValue = 0;
		}

		_stream->flush();
	}
};

// typedefs for various memory layouts.

/** 8-bit data, MSB to LSB. */
typedef BitStreamWriterImpl<8, false, true > BitStreamWriter8MSB;
/** 8-bit data, LSB to MSB. */
typedef BitStreamWriterImpl<8, false, false> BitStreamWriter8LSB;

/** 16-bit little-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<16, true , true > BitStreamWriter16LEMSB;
/** 16-bit little-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<16, true , false> BitStreamWriter16LELSB;
/** 16-bit big-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<16, false, true > BitStreamWriter16BEMSB;
/** 16-bit big-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<16, false, false> BitStreamWriter16BELSB;

/** 32-bit little-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<32, true , true > BitStreamWriter32LEMSB;
/** 32-bit little-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<32, true , false> BitStreamWriter32LELSB;
/** 32-bit big-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<32, false, true > BitStreamWriter32BEMSB;
/** 32-bit big-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<32, false, false> BitStreamWriter32BELSB;

/** 64-bit little-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<64, true , true > BitStreamWriter64LEMSB;
/** 64-bit little-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<64, true , false> BitStreamWriter64LELSB;
/** 64-bit big-endian data, MSB to LSB. */
typedef BitStreamWriterImpl<64, false, true > BitStreamWriter64BEMSB;
/** 64-bit big-endian data, LSB to MSB. */
typedef BitStreamWriterImpl<64, false, false> BitStreamWriter64BELSB;

} // End of namespace Common

#endif // COMMON_BITSTREAMWRITER_H

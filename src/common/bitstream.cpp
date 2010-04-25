/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/bitstream.cpp
 *  A bit stream.
 */

#include <cstring>

#include "common/bitstream.h"
#include "common/stream.h"
#include "common/error.h"
#include "common/util.h"

namespace Common {

BitStream::BitStream() {
}

BitStream::~BitStream() {
}

void BitStream::skip(uint32 n) {
	while (n-- > 0)
		getBit();
}


BitStreamBE::BitStreamBE(SeekableReadStream &stream, uint32 bitCount) : _value(0), _inValue(0) {
	if ((bitCount % 8) != 0)
		throw Common::Exception("Big-endian bit stream size has to be dividable by 8");

	// Read the number of bytes of the stream

	uint32 byteSize = bitCount / 8;
	byte *data = new byte[byteSize];

	if (stream.read(data, byteSize) != byteSize) {
		delete[] data;
		throw Exception(kReadError);
	}

	_stream = new MemoryReadStream(data, byteSize, DisposeAfterUse::YES);
}

BitStreamBE::BitStreamBE(const byte *data, uint32 bitCount) : _value(0), _inValue(0) {
	if ((bitCount % 8) != 0)
		throw Common::Exception("Big-endian bit stream size has to be dividable by 8");

	// Copy the number of bytes from the data array

	uint32 byteSize = bitCount / 8;
	byte *dataN = new byte[byteSize];

	std::memcpy(dataN, data, byteSize);

	_stream = new MemoryReadStream(dataN, byteSize, DisposeAfterUse::YES);
}

BitStreamBE::~BitStreamBE() {
	delete _stream;
}

uint32 BitStreamBE::getBit() {
	if (_inValue == 0) {
		// Need to get new byte

		if (_stream->eos())
			throw Common::Exception("End of bit stream reached");

		_value = _stream->readByte();
	}

	// Get the current bit
	int b = ((_value & 0x80) == 0) ? 0 : 1;

	// Shift to the next bit
	_value <<= 1;

	// Increase the position within the current byte
	_inValue = (_inValue + 1) % 8;

	return b;
}

uint32 BitStreamBE::getBits(uint32 n) {
	if (n > 32)
		throw Common::Exception("Too many bits requested to be read");

	// Read the number of bits
	uint32 v = 0;
	while (n-- > 0)
		v = (v << 1) | getBit();

	return v;
}

void BitStreamBE::addBit(uint32 &x, uint32 n) {
	x = (x << 1) | getBit();
}

uint32 BitStreamBE::pos() const {
	if (_stream->pos() == 0)
		return 0;

	uint32 pos = (_inValue == 0) ? _stream->pos() : (_stream->pos() - 1);
	return pos * 8 + _inValue;
}

uint32 BitStreamBE::size() const {
	return _stream->size() * 8;
}


BitStream32LE::BitStream32LE(SeekableReadStream &stream, uint32 bitCount) : _value(0), _inValue(0) {
	if ((bitCount % 32) != 0)
		throw Common::Exception("32bit little-endian bit stream size has to be dividable by 32");

	// Read the number of bytes of the stream

	uint32 byteSize = bitCount / 8;
	byte  *data     = new byte[byteSize];

	if (stream.read(data, byteSize) != byteSize) {
		delete[] data;
		throw Exception(kReadError);
	}

	_stream = new MemoryReadStream(data, byteSize, DisposeAfterUse::YES);
}

BitStream32LE::BitStream32LE(const byte *data, uint32 bitCount) : _value(0), _inValue(0) {
	if ((bitCount % 32) != 0)
		throw Common::Exception("32bit little-endian bit stream size has to be dividable by 32");

	// Copy the number of bytes from the data array

	uint32 byteSize = bitCount / 8;
	byte  *dataN    = new byte[byteSize];

	std::memcpy(dataN, data, byteSize);

	_stream = new MemoryReadStream(dataN, byteSize, DisposeAfterUse::YES);
}

BitStream32LE::~BitStream32LE() {
	delete _stream;
}

uint32 BitStream32LE::getBit() {
	if (_inValue == 0) {
		// Need to get new 32bit value

		if (_stream->eos())
			throw Common::Exception("End of bit stream reached");

		_value = _stream->readUint32LE();
	}

	// Get the current bit
	int b = ((_value & 1) == 0) ? 0 : 1;

	// Shift to the next bit
	_value >>= 1;

	// Increase the position within the current byte
	_inValue = (_inValue + 1) % 32;

	return b;
}

uint32 BitStream32LE::getBits(uint32 n) {
	if (n > 32)
		throw Common::Exception("Too many bits requested to be read");

	// Read the number of bits
	uint32 v = 0;
	for (uint32 i = 0; i < n; i++)
		v = (v >> 1) | (((uint32) getBit()) << 31);

	v >>= (32 - n);
	return v;
}

void BitStream32LE::addBit(uint32 &x, uint32 n) {
	x = (x & ~(1 << n)) | (getBit() << n);
}

uint32 BitStream32LE::pos() const {
	if (_stream->pos() == 0)
		return 0;

	uint32 pos = ((_inValue == 0) ? _stream->pos() : (_stream->pos() - 1)) & 0xFFFFFFFC;
	return pos * 8 + _inValue;
}

uint32 BitStream32LE::size() const {
	return _stream->size() * 8;
}

} // End of namespace Common

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
 *  Classes for serializing and deserializing raw data.
 */

#include <boost/type_traits.hpp>

#include "src/common/serializationstream.h"
#include "src/common/encoding.h"
#include "src/common/string.h"

namespace Common {

SerializationStream::~SerializationStream() {
}

SerializationReadStream::SerializationReadStream(SeekableReadStream &stream) : _stream(stream) {
}

void SerializationReadStream::readOrWriteUint64LE(uint64_t &value) {
	value = _stream.readUint64LE();
}

void SerializationReadStream::readOrWriteUint32LE(uint32_t &value) {
	value = _stream.readUint32LE();
}

void SerializationReadStream::readOrWriteUint16LE(uint16_t &value) {
	value = _stream.readUint16LE();
}

void SerializationReadStream::readOrWriteUint64BE(uint64_t &value) {
	value = _stream.readUint64BE();
}

void SerializationReadStream::readOrWriteUint32BE(uint32_t &value) {
	value = _stream.readUint32BE();
}

void SerializationReadStream::readOrWriteUint16BE(uint16_t &value) {
	value = _stream.readUint16BE();
}

void SerializationReadStream::readOrWriteSint64LE(int64_t &value) {
	value = _stream.readSint64LE();
}

void SerializationReadStream::readOrWriteSint32LE(int32_t &value) {
	value = _stream.readSint32LE();
}

void SerializationReadStream::readOrWriteSint16LE(int16_t &value) {
	value = _stream.readSint16LE();
}

void SerializationReadStream::readOrWriteSint64BE(int64_t &value) {
	value = _stream.readSint64BE();
}

void SerializationReadStream::readOrWriteSint32BE(int32_t &value) {
	value = _stream.readSint32BE();
}

void SerializationReadStream::readOrWriteSint16BE(int16_t &value) {
	value = _stream.readSint16BE();
}

template<typename T>
static UString toHexString(T value) {
	using unsignedT = typename boost::make_unsigned<T>::type;
	unsignedT v = static_cast<unsignedT>(value);

	const size_t bits = sizeof(unsignedT) * 8;

	UString str = "0x";
	for (size_t i = 0; i < bits; i += 4) {
		const uint nV = (v >> (bits - 4)) & 0xF;
		v <<= 4;

		str += Common::String::format("%X", nV);
	}

	return str;
}

void SerializationReadStream::readOrWriteUint64LE(const uint64_t &value) {
	uint64_t value2 = _stream.readUint64LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint64LE(): Invalid uint64_t value %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteUint32LE(const uint32_t &value) {
	uint32_t value2 = _stream.readUint32LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint32LE(): Invalid uint32_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteUint16LE(const uint16_t &value) {
	uint16_t value2 = _stream.readUint16LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint16LE(): Invalid uint16_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteUint64BE(const uint64_t &value) {
	uint64_t value2 = _stream.readUint64BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint64BE(): Invalid uint64_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteUint32BE(const uint32_t &value) {
	uint32_t value2 = _stream.readUint32BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint32BE(): Invalid uint32_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteUint16BE(const uint16_t &value) {
	uint16_t value2 = _stream.readUint16BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteUint16BE(): Invalid uint16_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint64LE(const int64_t &value) {
	int64_t value2 = _stream.readSint64LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint64LE(): Invalid int64_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint32LE(const int32_t &value) {
	int32_t value2 = _stream.readSint32LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint32LE(): Invalid int32_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint16LE(const int16_t &value) {
	int16_t value2 = _stream.readSint16LE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint16LE(): Invalid int16_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint64BE(const int64_t &value) {
	int64_t value2 = _stream.readSint64BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint64BE(): Invalid int64_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint32BE(const int32_t &value) {
	int32_t value2 = _stream.readSint32BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint32BE(): Invalid int32_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteSint16BE(const int16_t &value) {
	int16_t value2 = _stream.readSint16BE();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteSint16BE(): Invalid int16_t toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteByte(byte &value) {
	value = _stream.readByte();
}

void SerializationReadStream::readOrWriteByte(const byte &value) {
	byte value2 = _stream.readByte();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteByte(): Invalid byte toHexString(value).c_str() %s, should have been %s",
		                        toHexString(value2).c_str(), toHexString(value).c_str());
}

void SerializationReadStream::readOrWriteChar(char &value) {
	value = static_cast<char>(_stream.readChar());
}

void SerializationReadStream::readOrWriteChar(const char &value) {
	char value2 = _stream.readChar();
	if (value != value2)
		throw Common::Exception("SerializationReadStream::readOrWriteByte(): Invalid char value \'%c\', should have been \'%c\'", value2, value);
}

void SerializationReadStream::readOrWriteBytePrefixedASCIIString(Common::UString &value) {
	byte length = _stream.readByte();
	value = Common::readStringFixed(_stream, Common::kEncodingASCII, length);
}

void SerializationReadStream::readOrWriteUint32LEPrefixedASCIIString(Common::UString &value) {
	uint32_t length = _stream.readUint32LE();
	value = Common::readStringFixed(_stream, Common::kEncodingASCII, length);
}

void SerializationReadStream::readOrWriteBytePrefixedASCIIString(const Common::UString &value) {
	byte length = _stream.readByte();
	Common::UString value2 = Common::readStringFixed(_stream, Common::kEncodingASCII, length);
	if (value != value2)
		throw Common::Exception(
				"SerializationReadStream::readOrWriteByte(): Invalid string value \"%s\", should have been \"%s\"",
				value2.c_str(),
				value.c_str()
		);
}

void SerializationReadStream::readOrWriteUint32LEPrefixedASCIIString(const Common::UString &value) {
	uint32_t length = _stream.readUint32LE();
	Common::UString value2 = Common::readStringFixed(_stream, Common::kEncodingASCII, length);
	if (value != value2)
		throw Common::Exception(
				"SerializationReadStream::readOrWriteByte(): Invalid string value \"%s\", should have been \"%s\"",
				value2.c_str(),
				value.c_str()
		);
}

SerializationWriteStream::SerializationWriteStream(WriteStream &stream) : _stream(stream) {
}

void SerializationWriteStream::readOrWriteUint64LE(uint64_t &value) {
	_stream.writeUint64LE(value);
}

void SerializationWriteStream::readOrWriteUint32LE(uint32_t &value) {
	_stream.writeUint32LE(value);
}

void SerializationWriteStream::readOrWriteUint16LE(uint16_t &value) {
	_stream.writeUint16LE(value);
}

void SerializationWriteStream::readOrWriteUint64BE(uint64_t &value) {
	_stream.writeUint64BE(value);
}

void SerializationWriteStream::readOrWriteUint32BE(uint32_t &value) {
	_stream.writeUint32BE(value);
}

void SerializationWriteStream::readOrWriteUint16BE(uint16_t &value) {
	_stream.writeUint16BE(value);
}

void SerializationWriteStream::readOrWriteSint64LE(int64_t &value) {
	_stream.writeSint64LE(value);
}

void SerializationWriteStream::readOrWriteSint32LE(int32_t &value) {
	_stream.writeSint32LE(value);
}

void SerializationWriteStream::readOrWriteSint16LE(int16_t &value) {
	_stream.writeSint16LE(value);
}

void SerializationWriteStream::readOrWriteSint64BE(int64_t &value) {
	_stream.writeSint64BE(value);
}

void SerializationWriteStream::readOrWriteSint32BE(int32_t &value) {
	_stream.writeSint32BE(value);
}

void SerializationWriteStream::readOrWriteSint16BE(int16_t &value) {
	_stream.writeSint16BE(value);
}

void SerializationWriteStream::readOrWriteUint64LE(const uint64_t &value) {
	_stream.writeUint64LE(value);
}

void SerializationWriteStream::readOrWriteUint32LE(const uint32_t &value) {
	_stream.writeUint32LE(value);
}

void SerializationWriteStream::readOrWriteUint16LE(const uint16_t &value) {
	_stream.writeUint16LE(value);
}

void SerializationWriteStream::readOrWriteUint64BE(const uint64_t &value) {
	_stream.writeUint64BE(value);
}

void SerializationWriteStream::readOrWriteUint32BE(const uint32_t &value) {
	_stream.writeUint32BE(value);
}

void SerializationWriteStream::readOrWriteUint16BE(const uint16_t &value) {
	_stream.writeUint16BE(value);
}

void SerializationWriteStream::readOrWriteSint64LE(const int64_t &value) {
	_stream.writeSint64LE(value);
}

void SerializationWriteStream::readOrWriteSint32LE(const int32_t &value) {
	_stream.writeSint32LE(value);
}

void SerializationWriteStream::readOrWriteSint16LE(const int16_t &value) {
	_stream.writeSint16LE(value);
}

void SerializationWriteStream::readOrWriteSint64BE(const int64_t &value) {
	_stream.writeSint64BE(value);
}

void SerializationWriteStream::readOrWriteSint32BE(const int32_t &value) {
	_stream.writeSint32BE(value);
}

void SerializationWriteStream::readOrWriteSint16BE(const int16_t &value) {
	_stream.writeSint16BE(value);
}

void SerializationWriteStream::readOrWriteByte(byte &value) {
	_stream.writeByte(value);
}

void SerializationWriteStream::readOrWriteByte(const byte &value) {
	_stream.writeByte(value);
}

void SerializationWriteStream::readOrWriteChar(char &value) {
	_stream.writeByte(static_cast<byte>(value));
}

void SerializationWriteStream::readOrWriteChar(const char &value) {
	_stream.writeByte(static_cast<byte>(value));
}

void SerializationWriteStream::readOrWriteBytePrefixedASCIIString(Common::UString &value) {
	readOrWriteBytePrefixedASCIIString(static_cast<const Common::UString &>(value));
}

void SerializationWriteStream::readOrWriteUint32LEPrefixedASCIIString(Common::UString &value) {
	readOrWriteUint32LEPrefixedASCIIString(static_cast<const Common::UString &>(value));
}

void SerializationWriteStream::readOrWriteBytePrefixedASCIIString(const Common::UString &value) {
	std::unique_ptr<SeekableReadStream> encoded = convertString(value, Common::kEncodingASCII, false);
	_stream.writeByte(encoded->size());
	_stream.writeStream(*encoded);
}

void SerializationWriteStream::readOrWriteUint32LEPrefixedASCIIString(const Common::UString &value) {
	std::unique_ptr<SeekableReadStream> encoded = convertString(value, Common::kEncodingASCII, false);
	_stream.writeUint32LE(encoded->size());
	_stream.writeStream(*encoded);
}

} // End of namespace Common

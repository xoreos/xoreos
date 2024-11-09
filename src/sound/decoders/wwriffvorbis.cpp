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

/* Based heavily on the ww2ogg tool by Adam Gashlin (hcs)
 * (<https://github.com/hcs64/ww2ogg)>, which is licensed under the
 * terms of the BSD 3-clause license.
 *
 * The original copyright note in ww2ogg reads as follows:
 * Copyright (c) 2002, Xiph.org Foundation
 * Copyright (c) 2009-2016, Adam Gashlin
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>

#include "src/common/types.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"
#include "src/common/disposableptr.h"
#include "src/common/bitstream.h"
#include "src/common/bitstreamwriter.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/sound/audiostream.h"

#include "src/sound/decoders/wwriffvorbis.h"
#include "src/sound/decoders/wwriffvorbisdata.h"
#include "src/sound/decoders/vorbis.h"

namespace Sound {

class CodebookLibrary {
public:
	CodebookLibrary(Common::SeekableReadStream &stream);
	CodebookLibrary();

	~CodebookLibrary() = default;

	CodebookLibrary(const CodebookLibrary &) = delete;
	CodebookLibrary &operator=(const CodebookLibrary &) = delete;

	void rebuild(size_t i, Common::BitStreamWriter &bos);
	void rebuild(Common::BitStream &bis, size_t size, Common::BitStreamWriter &bos);

	void copy(Common::BitStream &bis, Common::BitStreamWriter &bos);

private:
	Common::SeekableReadStream *_stream;

	size_t _count;
	size_t _offsetOffset;

	Common::SeekableReadStream *getCodebook(size_t i);
};

static size_t bookMapType1QuantVals(size_t entries, size_t dimensions) {
	const size_t bits = Common::intLog2(entries) + 1;
	size_t vals = entries >> ((bits - 1) * (dimensions - 1) / dimensions);

	while (1) {
		size_t acc  = 1;
		size_t acc1 = 1;

		for (size_t i = 0; i < dimensions; i++) {
			acc  *= vals;
			acc1 *= vals + 1;
		}

		if ((acc <= entries) && (acc1 > entries))
			return vals;

		if(acc > entries) {
			vals--;
		} else {
			vals++;
		}
	}
}

CodebookLibrary::CodebookLibrary() : _stream(0), _count(0), _offsetOffset(0) {
}

CodebookLibrary::CodebookLibrary(Common::SeekableReadStream &stream) : _stream(&stream) {
	_stream->seek(_stream->size() - 4);

	_offsetOffset = _stream->readUint32LE();
	_count = (_stream->size() - _offsetOffset) / 4;
}

Common::SeekableReadStream *CodebookLibrary::getCodebook(size_t i) {
	if (!_stream)
		throw Common::Exception("CodebookLibrary::getCodebook(): No codebook library");
	if (i >= (_count - 1))
		throw Common::Exception("CodebookLibrary::getCodebook(): Codebook out of range");

	_stream->seek(_offsetOffset + i * 4);

	const size_t offset = _stream->readUint32LE();
	const size_t next   = _stream->readUint32LE();

	return new Common::SeekableSubReadStream(_stream, offset, next);
}

void CodebookLibrary::rebuild(size_t i, Common::BitStreamWriter &bos) {
	Common::BitStream8LSB bitStream(getCodebook(i), true);

	rebuild(bitStream, bitStream.size() / 8, bos);
}

void CodebookLibrary::rebuild(Common::BitStream &bis, size_t size, Common::BitStreamWriter &bos) {
	const uint32_t id = 0x564342;
	const uint16_t dimensions = bis.getBits(4);
	const uint32_t entries = bis.getBits(14);

	bos.putBits(id, 24);
	bos.putBits(dimensions, 16);
	bos.putBits(entries, 24);

	const bool ordered = bis.getBit();
	bos.putBit(ordered);

	if (ordered) {
		const uint8_t initialLength = bis.getBits(5);
		bos.putBits(initialLength, 5);

		uint32_t currentEntry = 0;
		while (currentEntry < entries) {
			const size_t bitCount = Common::intLog2(entries - currentEntry) + 1;
			const uint32_t number = bis.getBits(bitCount);
			bos.putBits(number, bitCount);

			currentEntry += number;
		}

		if (currentEntry > entries)
			throw Common::Exception("CodebookLibrary::rebuild(): Current entry out of range");

	} else {

		const uint8_t codewordLengthLength = bis.getBits(3);
		const bool sparse = bis.getBit();

		if ((codewordLengthLength == 0) || (codewordLengthLength > 5))
			throw Common::Exception("CodebookLibrary::rebuild(): Nonsense codeword length");

		bos.putBit(sparse);

		for (size_t i = 0; i < entries; i++) {
			bool present = true;

			if (sparse) {
				present = bis.getBit();
				bos.putBit(present);
			}

			if (present)
				bos.putBits(bis.getBits(codewordLengthLength), 5);
		}

	}

	const uint8_t lookupType = bis.getBits(1);
	bos.putBits(lookupType, 4);

	if        (lookupType == 0) {
	} else if (lookupType == 1) {

		const uint32_t min = bis.getBits(32);
		const uint32_t max = bis.getBits(32);

		const uint8_t valueLength = bis.getBits(4) + 1;
		const bool   sequenceFlag = bis.getBit();

		bos.putBits(min, 32);
		bos.putBits(max, 32);

		bos.putBits(valueLength - 1, 4);
		bos.putBit(sequenceFlag);

		const size_t quantVals = bookMapType1QuantVals(entries, dimensions);
		for (size_t i = 0; i < quantVals; i++)
			bos.putBits(bis.getBits(valueLength), valueLength);
	}

	if ((size != 0) && (((bis.pos() / 8) + 1) != size ))
		throw Common::Exception("CodebookLibrary::rebuild(): Size mismatch: %s != %s",
		                        Common::composeString((bis.pos() / 8) + 1).c_str(),
		                        Common::composeString(size).c_str());
}

void CodebookLibrary::copy(Common::BitStream &bis, Common::BitStreamWriter &bos) {
	const uint32_t id = bis.getBits(24);
	const uint16_t dimensions = bis.getBits(16);
	const uint32_t entries = bis.getBits(24);

	if (id != 0x564342)
		throw Common::Exception("CodebookLibrary::copy(): Invalid codebook identifier");

	bos.putBits(id, 24);
	bos.putBits(dimensions, 16);
	bos.putBits(entries, 24);

	const bool ordered = bis.getBit();
	bos.putBit(ordered);

	if (ordered) {

		const uint8_t initialLength = bis.getBits(5);
		bos.putBits(initialLength, 5);

		uint32_t currentEntry = 0;
		while (currentEntry < entries) {
			const size_t bitCount = Common::intLog2(entries - currentEntry) + 1;
			const uint32_t number = bis.getBits(bitCount);
			bos.putBits(number, bitCount);

			currentEntry += number;
		}

		if (currentEntry > entries)
			throw Common::Exception("CodebookLibrary::copy(): Current entry out of range");

	} else {

		const bool sparse = bis.getBit();
		bos.putBit(sparse);

		for (size_t i = 0; i < entries; i++) {
			bool present = true;

			if (sparse) {
				present = bis.getBit();
				bos.putBit(present);
			}

			if (present)
				bos.putBits(bis.getBits(5), 5);
		}

	}

	const uint8_t lookupType = bis.getBits(4);
	bos.putBits(lookupType, 4);

	if        (lookupType == 0) {
	} else if (lookupType == 1) {

		const uint32_t min = bis.getBits(32);
		const uint32_t max = bis.getBits(32);

		const uint8_t valueLength = bis.getBits(4) + 1;
		const bool   sequenceFlag = bis.getBit();

		bos.putBits(min, 32);
		bos.putBits(max, 32);

		bos.putBits(valueLength - 1, 4);
		bos.putBit(sequenceFlag);

		const size_t quantVals = bookMapType1QuantVals(entries, dimensions);
		for (size_t i = 0; i < quantVals; i++)
			bos.putBits(bis.getBits(valueLength), valueLength);

	} else
		throw Common::Exception("CodebookLibrary::copy(): Invalid lookup type %u", lookupType);
}

class WwRIFFVorbisStream : public Sound::RewindableAudioStream {
public:
	WwRIFFVorbisStream(Common::SeekableReadStream *inStream, bool disposeStream,
	                   Common::SeekableReadStream *codebooks, bool disposeCodebooks,
	                   bool fullSetup);

	size_t readBuffer(int16_t *buffer, const size_t numSamples);

	int getChannels() const;
	int getRate() const;

	bool endOfData() const;

	bool rewind();
	uint64_t getLength() const;

private:
	std::unique_ptr<Sound::PacketizedAudioStream> _vorbis;

	Common::DisposablePtr<Common::SeekableReadStream> _inStream;
	Common::DisposablePtr<Common::SeekableReadStream> _codebooks;

	bool _fullSetup;
	bool _headerTriadPresent;
	bool _oldPacketHeaders;
	bool _noGranule;
	bool _modPackets;

	size_t _dataOffset;
	size_t _dataSize;
	size_t _currentOffset;

	size_t _setupPacketOffset;
	size_t _firstAudioPacketOffset;

	uint16_t _channels;
	uint32_t _sampleRate;
	uint32_t _averageBytesPerSecond;

	uint64_t _sampleCount;

	uint32_t _uid;
	uint8_t _blocksize0Pow;
	uint8_t _blocksize1Pow;

	uint32_t _loopCount;
	uint32_t _loopStart;
	uint32_t _loopEnd;

	std::unique_ptr<bool[]> _modeBlockFlags;
	size_t _modeBits;

	bool _end;


	void init();

	Common::SeekableReadStream *generateHeaderIdentification();
	Common::SeekableReadStream *generateHeaderComment();
	Common::SeekableReadStream *generateHeaderSetup();

	Common::SeekableReadStream *createPacket();
};

/* Modern 2 or 6 byte header */
class Packet {
public:
	Packet(Common::SeekableReadStream &i, long o, bool noGranule = false) :
			_offset(o), _size(SIZE_MAX), _absoluteGranule(0), _noGranule(noGranule) {

		i.seek(_offset);

		_size = i.readUint16LE();

		if (!_noGranule)
			_absoluteGranule = i.readUint32LE();
	}

	size_t headerSize() { return _noGranule ? 2 : 6; }
	size_t size() { return _size; }

	size_t offset() { return _offset + headerSize(); }
	size_t nextOffset() { return _offset + headerSize() + _size; }

	uint32_t granule() { return _absoluteGranule; }

private:
	size_t _offset;
	size_t _size;
	uint32_t _absoluteGranule;
	bool _noGranule;
};


WwRIFFVorbisStream::WwRIFFVorbisStream(Common::SeekableReadStream *inStream, bool disposeStream,
                                       Common::SeekableReadStream *codebooks, bool disposeCodebooks,
                                       bool fullSetup) :
		_inStream(inStream, disposeStream), _codebooks(codebooks, disposeCodebooks),
		_fullSetup(fullSetup), _headerTriadPresent(false), _oldPacketHeaders(false),
		_noGranule(false), _modPackets(false),
		_dataOffset(SIZE_MAX), _dataSize(SIZE_MAX), _currentOffset(SIZE_MAX),
		_setupPacketOffset(SIZE_MAX), _firstAudioPacketOffset(SIZE_MAX),
		_channels(0), _sampleRate(0), _averageBytesPerSecond(0), _sampleCount(kInvalidLength),
		_uid(0), _blocksize0Pow(0), _blocksize1Pow(0),
		_loopCount(0), _loopStart(0), _loopEnd(0),
		_end(false) {

	init();

	if (_headerTriadPresent)
		throw Common::Exception("WwRIFFVorbisStream: Header with triad");

	if (_oldPacketHeaders)
		throw Common::Exception("WwRIFFVorbisStream: Old packet header");

	if (_modPackets)
		throw Common::Exception("WwRIFFVorbisStream: Modified packets");

	rewind();
}

int WwRIFFVorbisStream::getChannels() const {
	return _vorbis->getChannels();
}

int WwRIFFVorbisStream::getRate() const {
	return _vorbis->getRate();
}

bool WwRIFFVorbisStream::endOfData() const {
	return _end;
}

bool WwRIFFVorbisStream::rewind() {
	_end = false;

	std::unique_ptr<Common::SeekableReadStream> headerIdentification(generateHeaderIdentification());
	std::unique_ptr<Common::SeekableReadStream> headerComment(generateHeaderComment());
	std::unique_ptr<Common::SeekableReadStream> headerSetup(generateHeaderSetup());

#ifdef ENABLE_VORBIS
	_vorbis.reset(Sound::makePacketizedVorbisStream(*headerIdentification, *headerComment, *headerSetup));
#else
	throw Common::Exception("Vorbis decoding disabled when building without libvorbis");
#endif

	_currentOffset = _dataOffset + _firstAudioPacketOffset;

	return true;
}

uint64_t WwRIFFVorbisStream::getLength() const {
	return _sampleCount;
}

size_t WwRIFFVorbisStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	size_t samples = 0;
	while (samples < numSamples) {
		const size_t needSamples = numSamples - samples;
		const size_t gotSamples  = _vorbis->readBuffer(buffer, needSamples);

		buffer  += gotSamples;
		samples += gotSamples;

		if (gotSamples < needSamples) {
			std::unique_ptr<Common::SeekableReadStream> packet(createPacket());
			if (!packet) {
				_end = true;
				break;
			}

			_vorbis->queuePacket(packet.release());
		}
	}

	return samples;
}

void WwRIFFVorbisStream::init() {
	if (_inStream->readUint32BE() != MKTAG('R', 'I', 'F', 'F'))
		throw Common::Exception("WwRIFFVorbisStream::init(): Missing RIFF");

	const size_t riffSize = _inStream->readUint32LE() + 8;

	if (_inStream->readUint32BE() != MKTAG('W', 'A', 'V', 'E'))
		throw Common::Exception("WwRIFFVorbisStream::init(): Missing WAVE");

	size_t offsetFMT = SIZE_MAX, offsetSMPL = SIZE_MAX, offsetVORB = SIZE_MAX;
	size_t sizeFMT   = SIZE_MAX,                        sizeVORB   = SIZE_MAX;

	size_t chunkOffset = 12;
	while (chunkOffset < riffSize) {
		_inStream->seek(chunkOffset);

		if ((chunkOffset + 8) > riffSize)
			throw Common::Exception("WwRIFFVorbisStream::init(): Chunk header truncated");

		const Common::UString chunkType = Common::readStringFixed(*_inStream, Common::kEncodingASCII, 4);

		const size_t chunkSize = _inStream->readUint32LE();

		if        (chunkType == "fmt ") {
			offsetFMT = chunkOffset + 8;
			sizeFMT   = chunkSize;
		} else if (chunkType == "smpl") {
			offsetSMPL = chunkOffset + 8;
		} else if (chunkType == "vorb") {
			offsetVORB = chunkOffset + 8;
			sizeVORB   = chunkSize;
		} else if (chunkType == "data") {
			_dataOffset = chunkOffset + 8;
			_dataSize   = chunkSize;
		}

			chunkOffset = chunkOffset + 8 + chunkSize;
	}

	if (chunkOffset > riffSize)
		throw Common::Exception("WwRIFFVorbisStream::init(): Chunk truncated");

	if ((offsetFMT == SIZE_MAX) && (_dataOffset == SIZE_MAX))
		throw Common::Exception("WwRIFFVorbisStream::init(): Expected fmt, data chunks");

	if ((offsetVORB == SIZE_MAX) && (sizeFMT != 0x42))
		throw Common::Exception("WwRIFFVorbisStream::init(): Expected 0x42 fmt if vorb missing");

	if ((offsetVORB != SIZE_MAX) && (sizeFMT != 0x28) && (sizeFMT != 0x18) && (sizeFMT != 0x12))
		throw Common::Exception("WwRIFFVorbisStream::init(): Bad fmt size");

	if ((offsetVORB == SIZE_MAX) && (sizeFMT == 0x42))
		offsetVORB = offsetFMT + 0x18;

	_inStream->seek(offsetFMT);
	if (_inStream->readUint16LE() != 0xFFFF)
		throw Common::Exception("WwRIFFVorbisStream::init(): Bad codec id");

	_channels    = _inStream->readUint16LE();
	_sampleRate = _inStream->readUint32LE();

	_averageBytesPerSecond = _inStream->readUint32LE();

	if (_inStream->readUint16LE() != 0)
		throw Common::Exception("WwRIFFVorbisStream::init(): Bad block align");
	if (_inStream->readUint16LE() != 0)
		throw Common::Exception("WwRIFFVorbisStream::init(): Expected 0 bps");

	if (_inStream->readUint16LE() != (sizeFMT - 0x12))
		throw Common::Exception("WwRIFFVorbisStream::init(): Bad extra fmt length");

	if (offsetSMPL != SIZE_MAX) {
		_inStream->seek(offsetSMPL + 0x1C);
		_loopCount = _inStream->readUint32LE();

		if (_loopCount != 1)
			throw Common::Exception("WwRIFFVorbisStream::init(): Expected one loop");

		_inStream->seek(offsetSMPL + 0x2C);

		_loopStart = _inStream->readUint32LE();
		_loopEnd   = _inStream->readUint32LE();
	}

	switch (sizeVORB) {
		case SIZE_MAX:
		case 0x28:
		case 0x2A:
		case 0x2C:
		case 0x32:
		case 0x34:
			_inStream->seek(offsetVORB);
			break;

		default:
			throw Common::Exception("WwRIFFVorbisStream::init(): Bad vorb size");
	}

	_sampleCount = _inStream->readUint32LE();

	switch (sizeVORB) {
		case SIZE_MAX:
		case 0x2A: {
			_noGranule = true;

			_inStream->seek(offsetVORB + 0x4);
			const uint32_t modSignal = _inStream->readUint32LE();

			/* set
			 * D9     11011001
			 * CB     11001011
			 * BC     10111100
			 * B2     10110010
			 * unset
			 * 4A     01001010
			 * 4B     01001011
			 * 69     01101001
			 * 70     01110000
			 * A7     10100111 !!! */

			/* Seems to be 0xD9 when _modPackets should be set.
			 * Also seen 0xCB, 0xBC, 0xB2. */

			if ((modSignal != 0x4A) && (modSignal != 0x4B) && (modSignal != 0x69) && (modSignal != 0x70)) {
				_modPackets = true;
			}

			_inStream->seek(offsetVORB + 0x10);
			break;
		}

		default:
			_inStream->seek(offsetVORB + 0x18);
			break;
	}

	_setupPacketOffset = _inStream->readUint32LE();
	_firstAudioPacketOffset = _inStream->readUint32LE();

	switch (sizeVORB) {
		case SIZE_MAX:
		case 0x2A:
			_inStream->seek(offsetVORB + 0x24);
			break;

		case 0x32:
		case 0x34:
			_inStream->seek(offsetVORB + 0x2C);
			break;
	}

	switch(sizeVORB) {
		case 0x28:
		case 0x2C:
			// Ok to leave _uid, _blocksize0Pow and _blocksize1Pow unset
			_headerTriadPresent = true;
			_oldPacketHeaders = true;
			break;

		case SIZE_MAX:
		case 0x2A:
		case 0x32:
		case 0x34:
			_uid = _inStream->readUint32LE();
			_blocksize0Pow = _inStream->readByte();
			_blocksize1Pow = _inStream->readByte();
			break;
	}

	if (_loopCount != 0) {
		_loopEnd = (_loopEnd == 0) ? _sampleCount : (_loopEnd + 1);

		if ((_loopStart >= _sampleCount) || (_loopEnd > _sampleCount) || (_loopStart > _loopEnd))
			throw Common::Exception("WwRIFFVorbisStream::init(): Loops out of range");
	}
}

static void putVorbisString(Common::BitStreamWriter &stream) {
	stream.putBits('v', 8);
	stream.putBits('o', 8);
	stream.putBits('r', 8);
	stream.putBits('b', 8);
	stream.putBits('i', 8);
	stream.putBits('s', 8);
}

static void putVorbisHeader(Common::BitStreamWriter &stream, uint8_t type) {
	stream.putBits(type, 8);
	putVorbisString(stream);
}

Common::SeekableReadStream *WwRIFFVorbisStream::generateHeaderIdentification() {
	Common::MemoryWriteStreamDynamic header(true, 32);
	Common::BitStreamWriter8LSB bits(header);

	putVorbisHeader(bits, 1);

	bits.putBits(0, 32); // Version

	bits.putBits(_channels, 8);
	bits.putBits(_sampleRate, 32);

	// Bitrate: max, nominal, min
	bits.putBits(0, 32);
	bits.putBits(_averageBytesPerSecond * 8, 32);
	bits.putBits(0, 32);

	bits.putBits(_blocksize0Pow, 4);
	bits.putBits(_blocksize1Pow, 4);

	bits.putBits(1, 1); // Framing

	bits.flush();

	header.setDisposable(false);
	return new Common::MemoryReadStream(header.getData(), header.size(), true);
}

Common::SeekableReadStream *WwRIFFVorbisStream::generateHeaderComment() {
	Common::MemoryWriteStreamDynamic header(true, 16);
	Common::BitStreamWriter8LSB bits(header);

	putVorbisHeader(bits, 3);

	bits.putBits(0, 32); // Length of vendor string
	bits.putBits(0, 32); // Number of user comments

	bits.putBits(1, 1); // Framing

	bits.flush();

	header.setDisposable(false);
	return new Common::MemoryReadStream(header.getData(), header.size(), true);
}

Common::SeekableReadStream *WwRIFFVorbisStream::generateHeaderSetup() {
	Common::MemoryWriteStreamDynamic header(true);
	Common::BitStreamWriter8LSB bits(header);

	putVorbisHeader(bits, 5);

	Packet setupPacket(*_inStream, _dataOffset + _setupPacketOffset, _noGranule);

	_inStream->seek(setupPacket.offset());

	if (setupPacket.granule() != 0)
		throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
		                        "Setup packet granule != 0");

	const size_t startPos = _inStream->pos() * 8;
	Common::BitStream8LSB in(*_inStream);

	// Codebook count
	const size_t codebookCount = in.getBits(8) + 1;
	bits.putBits(codebookCount - 1, 8);

	if (!_codebooks) {
		CodebookLibrary cbl;

		for (size_t i = 0; i < codebookCount; i++) {
			if (_fullSetup)
				cbl.copy(in, bits);
			else
				cbl.rebuild(in, 0, bits);
		}

	} else {
		CodebookLibrary cbl(*_codebooks);

		for (size_t i = 0; i < codebookCount; i++) {
			const uint16_t codebookID = in.getBits(10);

			cbl.rebuild(codebookID, bits);
		}
	}

	// Time-domain transforms (placeholder)
	bits.putBits(0, 6);
	bits.putBits(0, 16);

	if (_fullSetup) {
		while (in.pos() < (setupPacket.size() * 8))
			bits.putBit(in.getBit());

	} else {
		const size_t floorCount = in.getBits(6) + 1;
		bits.putBits(floorCount - 1, 6);

		for (size_t i = 0; i < floorCount; i++) {
			bits.putBits(1, 16); // Floor type

			const size_t floor1Partitions = in.getBits(5);
			bits.putBits(floor1Partitions, 5);

			std::unique_ptr<uint8_t[]> floor1PartitionClassList = std::make_unique<uint8_t[]>(floor1Partitions);
			uint8_t maxClass = 0;

			for (size_t j = 0; j < floor1Partitions; j++) {
				floor1PartitionClassList[j] = in.getBits(4);
				bits.putBits(floor1PartitionClassList[j], 4);

				maxClass = MAX(maxClass, floor1PartitionClassList[j]);
			}

			std::unique_ptr<uint8_t[]> floor1ClassDimensionsList = std::make_unique<uint8_t[]>(maxClass + 1);
			for (size_t j = 0; j <= maxClass; j++) {
				floor1ClassDimensionsList[j] = in.getBits(3) + 1;
				bits.putBits(floor1ClassDimensionsList[j] - 1, 3);

				const uint8_t subClasses = in.getBits(2);
				bits.putBits(subClasses, 2);

				if (subClasses != 0) {
					const uint8_t masterbook = in.getBits(8);
					bits.putBits(masterbook, 8);

					if (masterbook >= codebookCount)
						throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
						                        "Invalid floor1 masterbook");
				}

				for (size_t k = 0; k < (size_t(1) << subClasses); k++) {
					const int16_t subClassBook = static_cast<int16_t>(in.getBits(8)) - 1;
					bits.putBits(static_cast<uint16_t>(subClassBook + 1), 8);

					if (subClassBook >= 0 && (static_cast<size_t>(subClassBook) >= codebookCount))
						throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
						                        "Invalid floor1 subclass book");
				}
			}

			const uint8_t floor1Multiplier = in.getBits(2) + 1;
			bits.putBits(floor1Multiplier - 1, 2);

			const uint8_t rangeBits = in.getBits(4);
			bits.putBits(rangeBits, 4);

			for (size_t j = 0; j < floor1Partitions; j++) {
				const uint8_t currentClassNumber = floor1PartitionClassList[j];

				for (size_t k = 0; k < floor1ClassDimensionsList[currentClassNumber]; k++)
					bits.putBits(in.getBits(rangeBits), rangeBits);

			}
		}

		const size_t residueCount = in.getBits(6) + 1;
		bits.putBits(residueCount - 1, 6);

		for (size_t i = 0; i < residueCount; i++) {
			const uint8_t residueType = in.getBits(2);
			bits.putBits(residueType, 16);

			if (residueType > 2)
				throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
				                        "Invalid residue type");

			// Residues: begin, end, partition size
			bits.putBits(in.getBits(24), 24);
			bits.putBits(in.getBits(24), 24);
			bits.putBits(in.getBits(24), 24);

			const size_t residueClassifications = in.getBits(6) + 1;
			bits.putBits(residueClassifications - 1, 6);

			const uint8_t residueClassbook = in.getBits(8);
			bits.putBits(residueClassbook, 8);

			if (residueClassbook >= codebookCount)
				throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
				                        "Invalid residue classbook");

			std::unique_ptr<uint16_t[]> residueCascade = std::make_unique<uint16_t[]>(residueClassifications);
			for (size_t j = 0; j < residueClassifications; j++) {
				const uint8_t lowBits = in.getBits(3);
				bits.putBits(lowBits, 3);

				const uint8_t bitFlag = in.getBit();
				bits.putBit(bitFlag);

				uint8_t highBits = 0;
				if (bitFlag) {
					highBits = in.getBits(5);
					bits.putBits(highBits, 5);
				}

				residueCascade[j] = highBits * 8 + lowBits;
			}

			for (size_t j = 0; j < residueClassifications; j++) {
				for (uint8_t k = 0; k < 8; k++) {
					if (residueCascade[j] & (1 << k)) {
						const size_t residueBook = in.getBits(8);
						bits.putBits(residueBook, 8);

						if (residueBook >= codebookCount)
							throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
							                        "Invalid residue book");
					}
				}
			}
		}

		const size_t mappingCount = in.getBits(6) + 1;
		bits.putBits(mappingCount - 1, 6);

		for (size_t i = 0; i < mappingCount; i++) {
			bits.putBits(0, 16); // Mapping type

			const uint8_t subMapsFlag = in.getBit();
			bits.putBit(subMapsFlag);

			uint8_t subMaps = 1;
			if (subMapsFlag) {
				subMaps = in.getBits(4) + 1;
				bits.putBits(subMaps - 1, 4);
			}

			const uint8_t squarePolarFlag = in.getBit();
			bits.putBit(squarePolarFlag);

			if (squarePolarFlag) {
				const size_t couplingSteps = in.getBits(8) + 1;
				bits.putBits(couplingSteps - 1, 8);

				for (size_t j = 0; j < couplingSteps; j++) {
					const size_t bitCount = Common::intLog2(_channels - 1) + 1;

					const uint32_t magnitude = in.getBits(bitCount);
					const uint32_t angle = in.getBits(bitCount);

					bits.putBits(magnitude, bitCount);
					bits.putBits(angle, bitCount);

					if ((angle == magnitude) || (magnitude >= _channels) || (angle >= _channels))
						throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
						                        "Invalid coupling");
				}
			}

			const uint8_t mappingReserved = in.getBits(2);
			bits.putBits(mappingReserved, 2);

			if (mappingReserved != 0)
				throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
				                        "Mapping reserved field nonzero");

			if (subMaps > 1) {
				for (size_t j = 0; j < _channels; j++) {
					const uint8_t MappingMux = in.getBits(4);
					bits.putBits(MappingMux, 4);

					if (MappingMux >= subMaps)
						throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
						                        "MappingMux >= subMaps");
				}
			}

			for (size_t j = 0; j < subMaps; j++) {
				const uint8_t timeConfig    = in.getBits(8);
				const uint8_t floorNumber   = in.getBits(8);
				const uint8_t residueNumber = in.getBits(8);

				bits.putBits(timeConfig   , 8);
				bits.putBits(floorNumber  , 8);
				bits.putBits(residueNumber, 8);

				if (floorNumber >= floorCount)
					throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
					                        "Invalid floor mapping");
				if (residueNumber >= residueCount)
					throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
					                        "Invalid residue mapping");
			}
		}

		const size_t modeCount = in.getBits(6) + 1;
		bits.putBits(modeCount - 1, 6);

		_modeBlockFlags = std::make_unique<bool[]>(modeCount);
		_modeBits = Common::intLog2(modeCount - 1) + 1;

		for (size_t i = 0; i < modeCount; i++) {
			_modeBlockFlags[i] = in.getBit();
			bits.putBit(_modeBlockFlags[i]);

			bits.putBits(0, 16); // Window type
			bits.putBits(0, 16); // Transform type

			const uint8_t mapping = in.getBits(8);
			bits.putBits(mapping, 8);

			if (mapping >= mappingCount)
				throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
				                        "Invalid mode mapping");
		}

	}

	bits.putBits(1, 1); // Framing

	bits.flush();

	if (((in.pos() - startPos + 7) / 8) != setupPacket.size())
		throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
		                        "Didn't read the exact size of the setup packet");

	if (setupPacket.nextOffset() != (_dataOffset + static_cast<size_t>(_firstAudioPacketOffset)))
		throw Common::Exception("WwRIFFVorbisStream::generateHeaderSetup(): "
		                        "First audio packet doesn't follow setup packet");

	header.setDisposable(false);
	return new Common::MemoryReadStream(header.getData(), header.size(), true);
}

Common::SeekableReadStream *WwRIFFVorbisStream::createPacket() {
	if (_currentOffset >= (_dataOffset + _dataSize))
		return 0;

	Common::MemoryWriteStreamDynamic header(true);
	Common::BitStreamWriter8LSB bits(header);

	Packet audioPacket(*_inStream, _currentOffset, _noGranule);

	const size_t packerHeaderSize = audioPacket.headerSize();
	const size_t size = audioPacket.size();
	const size_t packetPayloadOffset = audioPacket.offset();
	const size_t nextOffset = audioPacket.nextOffset();

	if (_currentOffset + packerHeaderSize > _dataOffset + _dataSize)
		throw Common::Exception("WwRIFFVorbisStream::createPacket(): "
		                        "Page header truncated");

	_currentOffset = packetPayloadOffset;
	_inStream->seek(_currentOffset);

	std::unique_ptr<Common::SeekableReadStream> packetStream(_inStream->readStream(size));

	_currentOffset = nextOffset;

	if (_currentOffset > (_dataOffset + _dataSize))
		throw Common::Exception("WwRIFFVorbisStream::createPacket(): "
		                        "Page truncated");

	return packetStream.release();
}


Sound::RewindableAudioStream *makeWwRIFFVorbisStream(Common::SeekableReadStream *wwRIFFVorbis,
                                                     bool disposeAfterUse) {

	Common::MemoryReadStream *codebook = new Common::MemoryReadStream(kCodeBook, sizeof(kCodeBook), false);

	return new WwRIFFVorbisStream(wwRIFFVorbis, disposeAfterUse, codebook, true, false);
}

} // End of namespace Sound

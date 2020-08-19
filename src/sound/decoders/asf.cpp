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
 *  Decoding Microsoft's Advanced Streaming Format.
 */

/* Based on the ASF implementation in FFmpeg (<https://ffmpeg.org/)>,
 * which is released under the terms of version 2 or later of the GNU
 * Lesser General Public License.
 *
 * The original copyright note in libavformat/asf.c reads as follows:
 *
 * Copyright (c) 2000, 2001 Fabrice Bellard
 *
 * This file is part of FFmpeg.
 *
 * FFmpeg is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * FFmpeg is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with FFmpeg; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <memory>

#include "src/common/disposableptr.h"
#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/sound/audiostream.h"

#include "src/sound/decoders/asf.h"
#include "src/sound/decoders/wma.h"
#include "src/sound/decoders/wave_types.h"

namespace Sound {

class ASFGUID {
public:
	ASFGUID(Common::SeekableReadStream &stream) {
		stream.read(id, 16);
	}

	ASFGUID(byte a0, byte a1, byte a2, byte a3, byte a4, byte a5, byte a6, byte a7, byte a8, byte a9, byte a10, byte a11, byte a12, byte a13, byte a14, byte a15) {
		id[0]  = a0;   id[1]  = a1;  id[2]  = a2;  id[3]  = a3;
		id[4]  = a4;   id[5]  = a5;  id[6]  = a6;  id[7]  = a7;
		id[8]  = a8;   id[9]  = a9;  id[10] = a10; id[11] = a11;
		id[12] = a12;  id[13] = a13; id[14] = a14; id[15] = a15;
	}

	bool operator==(const ASFGUID &g) const {
		return !std::memcmp(g.id, id, 16);
	}

	bool operator!=(const ASFGUID &g) const {
		return std::memcmp(g.id, id, 16) != 0;
	}

	Common::UString toString() const {
		return Common::UString::format("%02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x, %02x",
				id[0], id[1], id[2], id[3], id[4], id[5], id[6], id[7], id[8], id[9], id[10], id[11], id[12], id[13], id[14], id[15]);
	}

private:
	byte id[16];
};

// GUID's that we need
static const ASFGUID s_asfHeader         = ASFGUID(0x30, 0x26, 0xB2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xA6, 0xD9, 0x00, 0xAA, 0x00, 0x62, 0xCE, 0x6C);
static const ASFGUID s_asfFileHeader     = ASFGUID(0xA1, 0xDC, 0xAB, 0x8C, 0x47, 0xA9, 0xCF, 0x11, 0x8E, 0xE4, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65);
static const ASFGUID s_asfHead1          = ASFGUID(0xb5, 0x03, 0xbf, 0x5f, 0x2E, 0xA9, 0xCF, 0x11, 0x8e, 0xe3, 0x00, 0xc0, 0x0c, 0x20, 0x53, 0x65);
static const ASFGUID s_asfComment        = ASFGUID(0x33, 0x26, 0xb2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);
static const ASFGUID s_asfStreamHeader   = ASFGUID(0x91, 0x07, 0xDC, 0xB7, 0xB7, 0xA9, 0xCF, 0x11, 0x8E, 0xE6, 0x00, 0xC0, 0x0C, 0x20, 0x53, 0x65);
static const ASFGUID s_asfCodecComment   = ASFGUID(0x40, 0x52, 0xD1, 0x86, 0x1D, 0x31, 0xD0, 0x11, 0xA3, 0xA4, 0x00, 0xA0, 0xC9, 0x03, 0x48, 0xF6);
static const ASFGUID s_asfDataHeader     = ASFGUID(0x36, 0x26, 0xb2, 0x75, 0x8E, 0x66, 0xCF, 0x11, 0xa6, 0xd9, 0x00, 0xaa, 0x00, 0x62, 0xce, 0x6c);
static const ASFGUID s_asfAudioStream    = ASFGUID(0x40, 0x9E, 0x69, 0xF8, 0x4D, 0x5B, 0xCF, 0x11, 0xA8, 0xFD, 0x00, 0x80, 0x5F, 0x5C, 0x44, 0x2B);
static const ASFGUID s_asfExtendedHeader = ASFGUID(0x40, 0xA4, 0xD0, 0xD2, 0x07, 0xE3, 0xD2, 0x11, 0x97, 0xF0, 0x00, 0xA0, 0xC9, 0x5E, 0xA8, 0x50);
static const ASFGUID s_asfStreamBitRate  = ASFGUID(0xce, 0x75, 0xf8, 0x7b, 0x8d, 0x46, 0xd1, 0x11, 0x8d, 0x82, 0x00, 0x60, 0x97, 0xc9, 0xa2, 0xb2);

class ASFStream : public RewindableAudioStream {
public:
	ASFStream(Common::SeekableReadStream *stream, bool dispose);
	~ASFStream();

	size_t readBuffer(int16_t *buffer, const size_t numSamples);

	bool endOfData() const;
	int getChannels() const { return _channels; }
	int getRate() const { return _sampleRate; }
	uint64_t getLength() const;
	uint64_t getDuration() const;

	bool rewind();

private:
	// Packet data
	struct Packet {
		Packet();
		~Packet();

		byte flags;
		byte segmentType;
		uint16_t packetSize;
		uint32_t sendTime;
		uint16_t duration;

		struct Segment {
			byte streamID;
			byte sequenceNumber;
			bool isKeyframe;
			std::vector<Common::SeekableReadStream *> data;
		};

		std::vector<Segment> segments;
	};

	Common::DisposablePtr<Common::SeekableReadStream> _stream;

	void load();

	void parseStreamHeader();
	void parseFileHeader();
	Packet *readPacket();
	PacketizedAudioStream *createAudioStream();
	void feedAudioData();
	bool allDataLoaded() const;

	size_t _rewindPos;
	uint64_t _curPacket;
	std::unique_ptr<PacketizedAudioStream> _curAudioStream;
	byte _curSequenceNumber;

	// Header object variables
	uint64_t _packetCount;
	uint64_t _duration;
	uint32_t _minPacketSize, _maxPacketSize;

	// Stream object variables
	uint16_t _streamID;
	uint16_t _compression;
	uint16_t _channels;
	int _sampleRate;
	uint32_t _bitRate;
	uint16_t _blockAlign;
	uint16_t _bitsPerCodedSample;
	std::unique_ptr<Common::SeekableReadStream> _extraData;
};

ASFStream::Packet::Packet() {
}

ASFStream::Packet::~Packet() {
	for (size_t i = 0; i < segments.size(); i++)
		for (size_t j = 0; j < segments[i].data.size(); j++)
				delete segments[i].data[j];
}

ASFStream::ASFStream(Common::SeekableReadStream *stream, bool dispose) : _stream(stream, dispose) {
	_curPacket = 0;
	_curSequenceNumber = 1; // They always start at one

	load();
}

ASFStream::~ASFStream() {
}

void ASFStream::load() {
	ASFGUID guid = ASFGUID(*_stream);
	if (guid != s_asfHeader)
		throw Common::Exception("ASFStream: Missing asf header");

	_stream->readUint64LE();
	_stream->readUint32LE();
	_stream->readByte();
	_stream->readByte();

	for (;;) {
		size_t startPos = _stream->pos();
		guid = ASFGUID(*_stream);
		uint64_t size = _stream->readUint64LE();

		if (_stream->eos())
			throw Common::Exception("ASFStream: Unexpected eos");

		// TODO: Parse each chunk
		if (guid == s_asfFileHeader) {
			parseFileHeader();
		} else if (guid == s_asfHead1) {
			// Should be safe to ignore
		} else if (guid == s_asfComment) {
			// Ignored
		} else if (guid == s_asfStreamHeader) {
			parseStreamHeader();
		} else if (guid == s_asfCodecComment) {
			// TODO
		} else if (guid == s_asfDataHeader) {
			// Done parsing the header
			break;
		} else if (guid == s_asfExtendedHeader) {
			// TODO
		} else if (guid == s_asfStreamBitRate) {
			// Ignored
		} else
			warning("Found unknown ASF GUID: %s", guid.toString().c_str());

		_stream->seek(startPos + size);
	}

	// Skip to the beginning of the packets
	_stream->skip(26);
	_rewindPos = _stream->pos();
}

void ASFStream::parseFileHeader() {
	_stream->skip(16); // skip client GUID
	/* uint64_t fileSize = */ _stream->readUint64LE();
	/* uint64_t creationTime = */ _stream->readUint64LE();
	_packetCount = _stream->readUint64LE();
	/* uint64_t endTimestamp = */ _stream->readUint64LE();
	_duration = _stream->readUint64LE();
	/* uint32_t startTimestamp = */ _stream->readUint32LE();
	/* uint32_t unknown = */ _stream->readUint32LE();
	/* uint32_t flags = */ _stream->readUint32LE();
	_minPacketSize = _stream->readUint32LE();
	_maxPacketSize = _stream->readUint32LE();
	/* uint32_t uncFrameSize = */ _stream->readUint32LE();

	// We only know how to support packets of one length
	if (_minPacketSize != _maxPacketSize)
		throw Common::Exception("ASFStream::parseFileHeader(): Mismatched packet sizes: Min = %d, Max = %d", _minPacketSize, _maxPacketSize);

	// If we have no packets, bail out
	if (_packetCount == 0)
		throw Common::Exception("ASFStream::parseFileHeader(): No packets");
}

void ASFStream::parseStreamHeader() {
	if (_curAudioStream)
		throw Common::Exception("ASFStream::parseStreamHeader(): Multiple stream headers found");

	ASFGUID guid = ASFGUID(*_stream);

	if (guid != s_asfAudioStream)
		throw Common::Exception("ASFStream::parseStreamHeader(): Found non-audio stream");

	_stream->skip(16); // skip a guid
	_stream->readUint64LE(); // total size
	uint32_t typeSpecificSize = _stream->readUint32LE();
	_stream->readUint32LE();
	_streamID = _stream->readUint16LE();
	_stream->readUint32LE();

	// Parse the wave header
	_compression = _stream->readUint16LE();
	_channels = _stream->readUint16LE();
	_sampleRate = _stream->readUint32LE();
	_bitRate = _stream->readUint32LE() * 8;
	_blockAlign = _stream->readUint16LE();
	_bitsPerCodedSample = (typeSpecificSize == 14) ? 8 : _stream->readUint16LE();

	if (typeSpecificSize >= 18) {
		uint32_t cbSize = _stream->readUint16LE();
		cbSize = MIN<int>(cbSize, typeSpecificSize - 18);
		_extraData.reset(_stream->readStream(cbSize));
	}

	_curAudioStream.reset(createAudioStream());
}

uint64_t ASFStream::getLength() const {
	return (_duration * getRate()) / 10000000;
}

uint64_t ASFStream::getDuration() const {
	return _duration / 10000;
}

bool ASFStream::rewind() {
	// Seek back to the start of the packets
	_stream->seek(_rewindPos);

	// Reset our packet counter
	_curPacket = 0;

	// Reset our underlying stream
	_curAudioStream.reset(createAudioStream());

	// Reset this too
	_curSequenceNumber = 1;

	return true;
}

ASFStream::Packet *ASFStream::readPacket() {
	if (_curPacket == _packetCount)
		throw Common::Exception("ASFStream::readPacket(): Reading too many packets");

	size_t packetStartPos = _stream->pos();

	// Read a single ASF packet
	if (_stream->readByte() != 0x82)
		throw Common::Exception("ASFStream::readPacket(): Missing packet header");

	if (_stream->readUint16LE() != 0)
		throw Common::Exception("ASFStream::readPacket(): Unknown is not zero");

	Packet *packet = new Packet();
	packet->flags = _stream->readByte();
	packet->segmentType = _stream->readByte();
	packet->packetSize = (packet->flags & 0x40) ? _stream->readUint16LE() : 0;

	uint16_t paddingSize = 0;
	if (packet->flags & 0x10)
		paddingSize = _stream->readUint16LE();
	else if (packet->flags & 0x08)
		paddingSize = _stream->readByte();

	packet->sendTime = _stream->readUint32LE();
	packet->duration = _stream->readUint16LE();

	byte segmentCount = (packet->flags & 0x01) ? _stream->readByte() : 1;
	packet->segments.resize(segmentCount & 0x3F);

	for (uint32_t i = 0; i < packet->segments.size(); i++) {
		Packet::Segment &segment = packet->segments[i];

		segment.streamID = _stream->readByte();
		segment.sequenceNumber = _stream->readByte();
		segment.isKeyframe = (segment.streamID & 0x80) != 0;
		segment.streamID &= 0x7F;

		uint32_t fragmentOffset = 0;
		if (packet->segmentType == 0x55)
			fragmentOffset = _stream->readByte();
		else if (packet->segmentType == 0x59)
			fragmentOffset = _stream->readUint16LE();
		else if (packet->segmentType == 0x5D)
			fragmentOffset = _stream->readUint32LE();
		else
			throw Common::Exception("ASFStream::readPacket(): Unknown packet segment type 0x%02x", packet->segmentType);

		byte flags = _stream->readByte();
		if (flags == 1) {
			//uint32_t objectStartTime = fragmentOffset; // reused purpose
			_stream->readByte(); // unknown

			size_t dataLength = (packet->segments.size() == 1) ? (_maxPacketSize - (_stream->pos() - packetStartPos) - paddingSize) : _stream->readUint16LE();
			size_t startObjectPos = _stream->pos();

			while (_stream->pos() < dataLength + startObjectPos)
				segment.data.push_back(_stream->readStream(_stream->readByte()));
		} else if (flags == 8) {
			/* uint32_t objectLength = */ _stream->readUint32LE();
			/* uint32_t objectStartTime = */ _stream->readUint32LE();

			size_t dataLength = 0;
			if (packet->segments.size() == 1)
				dataLength = _maxPacketSize - (_stream->pos() - packetStartPos) - fragmentOffset - paddingSize;
			else if (segmentCount & 0x40)
				dataLength = _stream->readByte();
			else
				dataLength = _stream->readUint16LE();

			_stream->skip(fragmentOffset);
			segment.data.push_back(_stream->readStream(dataLength));
		} else
			throw Common::Exception("ASFStream::readPacket(): Unknown packet flags 0x%02x", flags);
	}

	// Skip any padding
	_stream->skip(paddingSize);

	// We just read a packet
	_curPacket++;

	if (_stream->pos() != packetStartPos + _maxPacketSize)
		throw Common::Exception("ASFStream::readPacket(): Mismatching packet pos: %u (should be %u)", (uint)_stream->pos(), (uint)(_maxPacketSize + packetStartPos));

	return packet;
}

PacketizedAudioStream *ASFStream::createAudioStream() {
	switch (_compression) {
	case kWaveWMAv2:
		return makeWMAStream(2, _sampleRate, _channels, _bitRate, _blockAlign, *_extraData);
	default:
		throw Common::Exception("ASFStream::createAudioStream(): Unknown compression 0x%04x", _compression);
	}

	return 0;
}

void ASFStream::feedAudioData() {
	std::unique_ptr<Packet> packet(readPacket());

	// TODO
	if (packet->segments.size() != 1)
		throw Common::Exception("ASFStream::feedAudioData(): Only single segment packets supported");

	Packet::Segment &segment = packet->segments[0];

	// We should only have one stream in a ASF audio file
	if (segment.streamID != _streamID)
		throw Common::Exception("ASFStream::feedAudioData(): Packet stream ID mismatch");

	// TODO
	if (segment.sequenceNumber != _curSequenceNumber)
		throw Common::Exception("ASFStream::feedAudioData(): Only one sequence number per packet supported");

	// This can overflow and needs to overflow!
	_curSequenceNumber++;

	// TODO
	if (segment.data.size() != 1)
		throw Common::Exception("ASFStream::feedAudioData(): Packet grouping not supported");

	_curAudioStream->queuePacket(segment.data[0]);

	// The PacketizedAudioStream has taken ownership of the pointer; reset it here
	segment.data[0] = 0;
}

size_t ASFStream::readBuffer(int16_t *buffer, const size_t numSamples) {
	size_t samplesDecoded = 0;

	while (true) {
		// Read as much out of the stream as possible
		const size_t n = _curAudioStream->readBuffer(buffer + samplesDecoded, numSamples - samplesDecoded);
		if (n == kSizeInvalid)
			return kSizeInvalid;

		samplesDecoded += n;

		// If we have decoded up to what is requested or have no more data, bail
		if (samplesDecoded == numSamples || allDataLoaded())
			break;

		// If we have no data, feed the beast
		if (_curAudioStream->endOfData())
			feedAudioData();
	}

	return samplesDecoded;
}

bool ASFStream::allDataLoaded() const {
	return _curPacket == _packetCount;
}

bool ASFStream::endOfData() const {
	return allDataLoaded() && _curAudioStream->endOfData();
}

RewindableAudioStream *makeASFStream(Common::SeekableReadStream *stream, bool disposeAfterUse) {
	return new ASFStream(stream, disposeAfterUse);
}

} // End of namespace Sound

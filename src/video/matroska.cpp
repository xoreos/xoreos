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
 *  Decoding Matroska videos.
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/timestamp.h"

#include "src/sound/audiostream.h"
#include "src/sound/decoders/vorbis.h"

#include "src/video/matroska.h"
#include "src/video/codecs/codec.h"
#include "src/video/codecs/vpx.h"

namespace Video {

namespace {

enum EBMLID {
	kEBMLIDHeader = 0x1A45DFA3,

	kEBMLIDVoid = 0xEC,
	kEBMLIDCRC32 = 0xBF,

	kEBMLIDVersion = 0x4286,
	kEBMLIDReadVersion = 0x42F7,
	kEBMLIDMaxIDLength = 0x42F2,
	kEBMLIDMaxSizeLength = 0x42F3,
	kEBMLIDDocType = 0x4282,
	kEBMLIDDocTypeVersion = 0x4287,
	kEBMLIDDocTypeReadVersion = 0x4285,

	kMatroskaIDSegment = 0x18538067,

	kMatroskaIDSeekHead = 0x114D9B74,
	kMatroskaIDSeek = 0x4DBB,
	kMatroskaIDSeekID = 0x53AB,
	kMatroskaIDSeekPosition = 0x53AC,

	kMatroskaIDInfo = 0x1549A966,
	kMatroskaIDTimeCodeScale = 0x2AD7B1,
	kMatroskaIDDuration = 0x4489,
	kMatroskaIDMuxingApp = 0x4D80,
	kMatroskaIDWritingApp = 0x5741,

	kMatroskaIDTracks = 0x1654AE6B,
	kMatroskaIDTrackEntry = 0xAE,
	kMatroskaIDTrackNumber = 0xD7,
	kMatroskaIDTrackUID = 0x73C5,
	kMatroskaIDTrackType = 0x83,
	kMatroskaIDFlagLacing = 0x9C,
	kMatroskaIDDefaultDuration = 0x23E383,
	kMatroskaIDLanguage = 0x22B59C,
	kMatroskaIDCodecID = 0x86,
	kMatroskaIDCodecPrivate = 0x63A2,

	kMatroskaIDVideo = 0xE0,
	kMatroskaIDPixelWidth = 0xB0,
	kMatroskaIDPixelHeight = 0xBA,

	kMatroskaIDAudio = 0xE1,
	kMatroskaIDChannels = 0x9F,
	kMatroskaIDSamplingFrequency = 0xB5,

	kMatroskaIDCluster = 0x1F43B675,
	kMatroskaIDTimeCode = 0xE7,
	kMatroskaIDBlockGroup = 0xA0,
	kMatroskaIDBlock = 0xA1,
	kMatroskaIDSimpleBlock = 0xA3
};

enum MatroskaTrackType {
	kMatroskaTrackVideo = 1,
	kMatroskaTrackAudio = 2,
	kMatroskaTrackComplex = 3,
	kMatroskaTrackLogo = 16,
	kMatroskaTrackSubtitle = 17,
	kMatroskaTrackButtons = 18,
	kMatroskaTrackControl = 32
};

struct MatroskaElementOffset {
	uint64_t offset;
	uint64_t size;
};

uint32_t readElementID(Common::SeekableReadStream &stream) {
	uint32_t x = stream.readByte();
	if ((x & 0x80) != 0)
		return x;
	if ((x & 0x40) != 0)
		return (x << 8) | stream.readByte();
	if ((x & 0x20) != 0)
		return (x << 16) | stream.readUint16BE();
	if ((x & 0x10) != 0) {
		x <<= 24;
		x |= stream.readUint16BE() << 8;
		return x | stream.readByte();
	}

	throw Common::Exception("Invalid EBML element ID: %d", x);
}

uint64_t readElementSize(Common::SeekableReadStream &stream) {
	uint64_t size = stream.readByte();
	if ((size & 0x80) != 0)
		return size & 0x7F;
	if ((size & 0x40) != 0)
		return ((size & 0x3F) << 8) | stream.readByte();
	if ((size & 0x20) != 0)
		return ((size & 0x1F) << 16) | stream.readUint16BE();
	if ((size & 0x10) != 0) {
		size &= 0x0F;
		size <<= 24;
		size |= stream.readUint16BE() << 8;
		return size | stream.readByte();
	}
	if ((size & 0x08) != 0)
		return ((size & 0x07) << 32) | stream.readUint32BE();
	if ((size & 0x04) != 0) {
		size &= 0x03;
		size <<= 40;
		size |= static_cast<uint64_t>(stream.readByte()) << 32;
		return size | stream.readUint32BE();
	}
	if ((size & 0x02) != 0) {
		size &= 0x01;
		size <<= 48;
		size |= static_cast<uint64_t>(stream.readUint16BE()) << 32;
		return size | stream.readUint32BE();
	}
	if ((size & 0x01) != 0) {
		size = static_cast<uint64_t>(stream.readByte()) << 48;
		size |= static_cast<uint64_t>(stream.readUint16BE()) << 32;
		return size | stream.readUint32BE();
	}

	throw Common::Exception("Invalid EBML element size");
}

enum EBMLParseMode {
	kEBMLParseOptional,
	kEBMLParseRequired
};

void readEBMLEntry(uint64_t &value, Common::SeekableReadStream &stream, uint64_t UNUSED(filePos)) {
	if (stream.size() > sizeof(uint64_t))
		throw Common::Exception("Invalid EBML integer size %u", (uint)stream.size());

	value = 0;

	while (stream.pos() < stream.size()) {
		value <<= 8;
		value |= stream.readByte();
	}
}

template<typename T>
void readEBMLString(T &value, Common::SeekableReadStream &stream) {
	// Max 16MB for strings
	if (stream.size() > 0x1000000)
		throw Common::Exception("Invalid EBML string size %u", (uint)stream.size());

	// Read in the entire stream
	std::unique_ptr<char[]> array = std::make_unique<char[]>(stream.size());
	stream.read(array.get(), stream.size());

	// Treat the whole thing as a string
	value = T(array.get(), stream.size());

	// TODO: May need to check for NUL character
}

void readEBMLEntry(std::string &value, Common::SeekableReadStream &stream, uint64_t UNUSED(filePos)) {
	readEBMLString(value, stream);
}

void readEBMLEntry(Common::UString &value, Common::SeekableReadStream &stream, uint64_t UNUSED(filePos)) {
	readEBMLString(value, stream);
}

void readEBMLEntry(double &value, Common::SeekableReadStream &stream, uint64_t UNUSED(filePos)) {
	// Read based on the size of the stream
	switch (stream.size()) {
	case 0:
		value = 0.0;
		break;
	case 4:
		value = stream.readIEEEFloatBE();
		break;
	case 8:
		value = stream.readIEEEDoubleBE();
		break;
	default:
		throw Common::Exception("Invalid EBML double size %u", (int)stream.size());
	}
}

void readEBMLEntry(std::shared_ptr<Common::SeekableReadStream> &value, Common::SeekableReadStream &stream, uint64_t UNUSED(filePos)) {
	if (stream.size() > 0x10000000)
		throw Common::Exception("Invalid EBML binary size %u", (int)stream.size());

	value.reset(stream.readStream(stream.size()));
}

void readEBMLEntry(MatroskaElementOffset &value, Common::SeekableReadStream &stream, uint64_t filePos) {
	value.offset = filePos;
	value.size = stream.size();
}

template<typename T>
void readEBMLEntry(std::vector<T> &value, Common::SeekableReadStream &stream, uint64_t filePos) {
	T object;
	readEBMLEntry(object, stream, filePos);
	value.push_back(object);
}

class EBMLBaseEntryReader {
public:
	virtual ~EBMLBaseEntryReader() {}
	virtual void read(Common::SeekableReadStream &stream, uint64_t filePos) = 0;
	virtual void setDefaultValue() = 0;
	virtual EBMLBaseEntryReader *clone() const = 0;
};

template<typename T>
void copyEBMLValue(T &object, T *defaultValue = nullptr) {
	if (defaultValue)
		object = *defaultValue;
}

template<typename T>
class EBMLEntryReader : public EBMLBaseEntryReader {
public:
	EBMLEntryReader(T &object) : _object(object) {}

	void read(Common::SeekableReadStream &stream, uint64_t filePos) {
		readEBMLEntry(_object, stream, filePos);
	}

	void setDefaultValue() {
		// No default
	}

	EBMLBaseEntryReader *clone() const {
		return new EBMLEntryReader(_object);
	}

private:
	T &_object;
};

template<typename T>
class EBMLEntryReaderDefault : public EBMLBaseEntryReader {
public:
	EBMLEntryReaderDefault(T &object, const T &defaultValue) : _object(object), _defaultValue(defaultValue) {}

	void read(Common::SeekableReadStream &stream, uint64_t filePos) {
		readEBMLEntry(_object, stream, filePos);
	}

	void setDefaultValue() {
		_object = _defaultValue;
	}

	EBMLBaseEntryReader *clone() const {
		return new EBMLEntryReaderDefault(_object, _defaultValue);
	}

private:
	T &_object;
	const T &_defaultValue;
};

class EBMLSyntaxReader {
public:
	template<typename T>
	EBMLSyntaxReader(EBMLID id, EBMLParseMode mode, T &object) :
		_id(id),
		_mode(mode),
		_reader(new EBMLEntryReader<T>(object)) {}

	template<typename T>
	EBMLSyntaxReader(EBMLID id, EBMLParseMode mode, T &object, const T &defaultValue) :
		_id(id),
		_mode(mode),
		_reader(new EBMLEntryReaderDefault<T>(object, defaultValue)) {}

	EBMLSyntaxReader(const EBMLSyntaxReader &reader) :
		_id(reader._id),
		_mode(reader._mode),
		_reader(_reader->clone()) {}

	EBMLID getID() const { return _id; }
	EBMLParseMode getMode() const { return _mode; }
	void read(Common::SeekableReadStream &stream, uint64_t filePos) { _reader->read(stream, filePos); }
	void setDefaultValue() { _reader->setDefaultValue(); }

private:
	EBMLID _id;
	EBMLParseMode _mode;
	std::unique_ptr<EBMLBaseEntryReader> _reader;
};

template<size_t N>
void readEBMLNode(EBMLSyntaxReader (&readers)[N], Common::SeekableReadStream &stream, uint64_t filePos) {
	bool foundEntry[N];
	for (size_t i = 0; i < N; i++)
		foundEntry[i] = false;

	while (stream.pos() < stream.size()) {
		uint32_t id = readElementID(stream);
		uint64_t size = readElementSize(stream);

		// Check for void/crc and ignore it
		if (id == kEBMLIDVoid || id == kEBMLIDCRC32) {
			stream.seek(size, Common::SeekableReadStream::kOriginCurrent);
			continue;
		}

		// Look up the ID in the table
		EBMLSyntaxReader *reader = nullptr;
		for (size_t i = 0; i < N; i++) {
			if (readers[i].getID() == id) {
				reader = &readers[i];
				foundEntry[i] = true;
				break;
			}
		}

		// If we don't know what the ID is, ignore it
		if (!reader) {
			stream.seek(size, Common::SeekableReadStream::kOriginCurrent);
			continue;
		}

		size_t startPos = stream.pos();
		Common::SeekableSubReadStream element(&stream, startPos, startPos + size);
		reader->read(element, filePos + startPos);
		stream.seek(startPos + size);
	}

	// Check for required values and fill in defaults if necessary
	for (size_t i = 0; i < N; i++) {
		if (foundEntry[i])
			continue;

		// If it's not required, fill in the default and move on
		if (readers[i].getMode() == kEBMLParseOptional) {
			readers[i].setDefaultValue();
			continue;
		}

		throw Common::Exception("Missing required EBML ID 0x%X", readers[i].getID());
	}
}

struct EBMLHeader {
	uint64_t version;
	uint64_t readVersion;
	uint64_t maxIDLength;
	uint64_t maxSizeLength;
	std::string docType;
	uint64_t docTypeVersion;
	uint64_t docTypeReadVersion;
};

void readEBMLEntry(EBMLHeader &header, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kEBMLIDVersion, kEBMLParseRequired, header.version),
		EBMLSyntaxReader(kEBMLIDReadVersion, kEBMLParseRequired, header.readVersion),
		EBMLSyntaxReader(kEBMLIDMaxIDLength, kEBMLParseRequired, header.maxIDLength),
		EBMLSyntaxReader(kEBMLIDMaxSizeLength, kEBMLParseRequired, header.maxSizeLength),
		EBMLSyntaxReader(kEBMLIDDocType, kEBMLParseRequired, header.docType),
		EBMLSyntaxReader(kEBMLIDDocTypeVersion, kEBMLParseRequired, header.docTypeVersion),
		EBMLSyntaxReader(kEBMLIDDocTypeReadVersion, kEBMLParseRequired, header.docTypeReadVersion)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaSeek {
	std::shared_ptr<Common::SeekableReadStream> seekID;
	uint64_t seekPosition;
};

void readEBMLEntry(MatroskaSeek &seek, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDSeekID, kEBMLParseRequired, seek.seekID),
		EBMLSyntaxReader(kMatroskaIDSeekPosition, kEBMLParseRequired, seek.seekPosition)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaSeekHead {
	std::vector<MatroskaSeek> seeks;
};

void readEBMLEntry(MatroskaSeekHead &seekHead, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDSeek, kEBMLParseRequired, seekHead.seeks)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaInfo {
	uint64_t timeCodeScale;
	double duration;
	Common::UString muxingApp;
	Common::UString writingApp;
};

void readEBMLEntry(MatroskaInfo &info, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDTimeCodeScale, kEBMLParseRequired, info.timeCodeScale),
		EBMLSyntaxReader(kMatroskaIDDuration, kEBMLParseOptional, info.duration, 0.0),
		EBMLSyntaxReader(kMatroskaIDMuxingApp, kEBMLParseRequired, info.muxingApp),
		EBMLSyntaxReader(kMatroskaIDWritingApp, kEBMLParseRequired, info.writingApp)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaVideo {
	MatroskaVideo() : pixelWidth(0), pixelHeight(0) {}
	uint64_t pixelWidth;
	uint64_t pixelHeight;
};

void readEBMLEntry(MatroskaVideo &video, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDPixelWidth, kEBMLParseRequired, video.pixelWidth),
		EBMLSyntaxReader(kMatroskaIDPixelHeight, kEBMLParseRequired, video.pixelHeight)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaAudio {
	MatroskaAudio() : channels(0), samplingFrequency(0.0) {}
	uint64_t channels;
	double samplingFrequency;
};

void readEBMLEntry(MatroskaAudio &audio, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDChannels, kEBMLParseRequired, audio.channels),
		EBMLSyntaxReader(kMatroskaIDSamplingFrequency, kEBMLParseRequired, audio.samplingFrequency)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaTrackEntry {
	uint64_t trackNumber;
	uint64_t trackUID;
	uint64_t trackType;
	uint64_t flagLacing;
	std::string language;
	std::string codecID;
	uint64_t defaultDuration;
	std::shared_ptr<Common::SeekableReadStream> codecPrivate;
	MatroskaVideo video;
	MatroskaAudio audio;
};

void readEBMLEntry(MatroskaTrackEntry &trackEntry, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDTrackNumber, kEBMLParseRequired, trackEntry.trackNumber),
		EBMLSyntaxReader(kMatroskaIDTrackUID, kEBMLParseRequired, trackEntry.trackUID),
		EBMLSyntaxReader(kMatroskaIDTrackType, kEBMLParseRequired, trackEntry.trackType),
		EBMLSyntaxReader(kMatroskaIDFlagLacing, kEBMLParseRequired, trackEntry.flagLacing),
		EBMLSyntaxReader(kMatroskaIDLanguage, kEBMLParseOptional, trackEntry.language),
		EBMLSyntaxReader(kMatroskaIDCodecID, kEBMLParseRequired, trackEntry.codecID),
		EBMLSyntaxReader(kMatroskaIDDefaultDuration, kEBMLParseOptional, trackEntry.defaultDuration),
		EBMLSyntaxReader(kMatroskaIDCodecPrivate, kEBMLParseOptional, trackEntry.codecPrivate),
		EBMLSyntaxReader(kMatroskaIDVideo, kEBMLParseOptional, trackEntry.video),
		EBMLSyntaxReader(kMatroskaIDAudio, kEBMLParseOptional, trackEntry.audio)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaTracks {
	std::vector<MatroskaTrackEntry> tracks;
};

void readEBMLEntry(MatroskaTracks &tracks, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDTrackEntry, kEBMLParseRequired, tracks.tracks)
	};

	readEBMLNode(syntax, stream, filePos);
}

struct MatroskaSegment {
	MatroskaSeekHead seekHead;
	MatroskaInfo info;
	MatroskaTracks tracks;
	std::vector<MatroskaElementOffset> clusters;
};


void readEBMLEntry(MatroskaSegment &segment, Common::SeekableReadStream &stream, uint64_t filePos) {
	EBMLSyntaxReader syntax[] = {
		EBMLSyntaxReader(kMatroskaIDSeekHead, kEBMLParseOptional, segment.seekHead),
		EBMLSyntaxReader(kMatroskaIDInfo, kEBMLParseRequired, segment.info),
		EBMLSyntaxReader(kMatroskaIDTracks, kEBMLParseOptional, segment.tracks),
		EBMLSyntaxReader(kMatroskaIDCluster, kEBMLParseOptional, segment.clusters)
	};

	readEBMLNode(syntax, stream, filePos);
}

template<typename T>
void readEBMLEntrySingle(T &object, EBMLID id, Common::SeekableReadStream &stream) {
	uint32_t checkedID = readElementID(stream);
	if (id != checkedID)
		throw Common::Exception("Failed to find EBML ID %d", id);

	uint64_t elementSize = readElementSize(stream);

	size_t startPos = stream.pos();
	Common::SeekableSubReadStream element(&stream, startPos, startPos + elementSize);
	readEBMLEntry(object, element, startPos);
	stream.seek(startPos + elementSize);
}

bool isValidDocType(const std::string &docType) {
	static const char *const matroskaDocTypes[] = {
		"matroska",
		"webm"
	};

	for (size_t i = 0; i < ARRAYSIZE(matroskaDocTypes); i++)
		if (docType == matroskaDocTypes[i])
			return true;

	return false;
}

enum {
	kEBMLVersion = 1
};

Common::Timestamp makeTimestamp(uint64_t nanoseconds) {
	return Common::Timestamp(0, nanoseconds, UINT64_C(1000000000));
}

} // End of anonymous namespace

Matroska::Matroska(Common::SeekableReadStream *fd) : _fd(fd) {
	assert(_fd);

	load();
}

void Matroska::load() {
	// Read the header in
	EBMLHeader header;
	readEBMLEntrySingle(header, kEBMLIDHeader, *_fd);

	// Validate the header
	if (header.version > kEBMLVersion)
		throw Common::Exception("Unhandled EBML version: %u", (uint)header.version);
	if (header.maxSizeLength > sizeof(uint64_t))
		throw Common::Exception("Unhandled EBML max size: %u", (uint)header.maxSizeLength);
	if (header.maxIDLength > sizeof(uint32_t))
		throw Common::Exception("Unhandled EBML ID length: %u", (uint)header.maxIDLength);
	if (!isValidDocType(header.docType))
		throw Common::Exception("Unhandled EBML doc type: %s", header.docType.c_str());

	// Read the segment
	MatroskaSegment segment;
	readEBMLEntrySingle(segment, kMatroskaIDSegment, *_fd);

	// Ensure we have at least one cluster, or there won't be any data to read from
	if (segment.clusters.empty())
		throw Common::Exception("No Matroska clusters found");

	// Store the time code scale
	_timeCodeScale = segment.info.timeCodeScale;

	// Look for a video track and make sure there is only one
	MatroskaVideoTrack *videoTrack = nullptr;
	for (size_t i = 0; i < segment.tracks.tracks.size(); i++) {
		MatroskaTrackEntry &entry = segment.tracks.tracks[i];
		if (entry.trackType != kMatroskaTrackVideo)
			continue;

		if (videoTrack)
			throw Common::Exception("Multiple Matroska video tracks found");

		// Validate the width/height
		if (entry.video.pixelWidth == 0 || entry.video.pixelHeight == 0)
			throw Common::Exception("Invalid Matroska video dimensions: %ux%u", (uint)entry.video.pixelWidth, (uint)entry.video.pixelHeight);

		// Create the video track and initialize the codec
		videoTrack = new MatroskaVideoTrack(entry.trackNumber, entry.video.pixelWidth, entry.video.pixelHeight, entry.defaultDuration);
		addTrack(videoTrack);
		videoTrack->initCodec(entry.codecID, entry.codecPrivate.get());
	}

	// Make sure we have one video track
	if (!videoTrack)
		throw Common::Exception("Missing Matroska video track");

	// Look for an audio track and take only the first one
	for (size_t i = 0; i < segment.tracks.tracks.size(); i++) {
		MatroskaTrackEntry &entry = segment.tracks.tracks[i];
		if (entry.trackType != kMatroskaTrackAudio)
			continue;

		// Validate the audio data
		if (entry.audio.channels == 0)
			throw Common::Exception("Missing Matroska audio channels");
		if (entry.audio.samplingFrequency == 0)
			throw Common::Exception("Missing Matroska audio sampling frequency");

		// Create the audio track
		try {
			addTrack(new MatroskaAudioTrack(entry.trackNumber, entry.audio.channels, entry.audio.samplingFrequency, entry.codecID, entry.codecPrivate.get()));
		} catch (const Common::Exception &ex) {
			warning("Failed to create audio track %u: %s", (uint)entry.trackNumber, ex.what());
		}
		break;
	}

	// Store away cluster info
	for (size_t i = 0; i < segment.clusters.size(); i++) {
		Cluster cluster;
		cluster.offset = segment.clusters[i].offset;
		cluster.size = segment.clusters[i].size;
		_clusters.push_back(cluster);
	}

	// Initialize video
	initVideo();
}


void Matroska::decodeNextTrackFrame(VideoTrack &track) {
	MatroskaVideoTrack &videoTrack = static_cast<MatroskaVideoTrack &>(track);

	// Get the next packet for the track
	uint64_t nextTimestamp;
	std::unique_ptr<Common::SeekableReadStream> packet(getNextPacket(videoTrack.getTrackNumber(), nextTimestamp));

	// If we didn't get a packet, we're done.
	if (!packet) {
		videoTrack.finish();
		return;
	}

	// Decode the frame
	videoTrack.decodeFrame(*_surface, *packet, nextTimestamp);
	_needCopy = true;
}

void Matroska::checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime) {
	MatroskaAudioTrack &audioTrack = static_cast<MatroskaAudioTrack &>(track);

	// Loop until we have enough audio queued
	while (audioTrack.canBufferData() && makeTimestamp(audioTrack.getLastTimestamp()) < endTime) {
		// Fetch the packet
		uint64_t nextTimestamp;
		std::unique_ptr<Common::SeekableReadStream> packet(getNextPacket(audioTrack.getTrackNumber(), nextTimestamp));

		// If we fetched no packet or we have detected no further packets, we're at the end
		if (!packet || nextTimestamp == std::numeric_limits<uint64_t>::max()) {
			audioTrack.finish();
			break;
		}

		// Decode the packet
		audioTrack.queueAudio(packet.release());

		// Update the timestamp
		audioTrack.setLastTimestamp(nextTimestamp);
	}
}

Common::SeekableReadStream *Matroska::getNextPacket(uint64_t trackNumber, uint64_t &nextTimestamp) {
	// Fetch the status for the current track
	TrackStatus &status = _status[trackNumber];

	// For the first frame, fetch the first packet
	if (!status.curCluster)
		findNextPacket(trackNumber, status);

	// If there is no offset, there are no packets left
	if (status.nextPacket.offset == 0)
		return nullptr;

	// Read in the actual packet
	_fd->seek(status.nextPacket.offset);
	std::unique_ptr<Common::SeekableReadStream> packet(_fd->readStream(status.nextPacket.size));

	// Find the next packet
	findNextPacket(trackNumber, status);

	// Update the timestamp
	nextTimestamp = status.nextPacket.timestamp;

	return packet.release();
}

void Matroska::findNextPacket(uint64_t trackNumber, TrackStatus& status) {
	Packet &packet = status.nextPacket;

	// If we don't have a cluster, start us off at the first one
	if (!status.curCluster) {
		status.curCluster = &_clusters[0];
		_fd->seek(status.curCluster->offset);
		readEBMLEntrySingle(status.curCluster->timestamp, kMatroskaIDTimeCode, *_fd);
		status.curPos = _fd->pos();
	}

	// Continue until we find a packet
	for (;;) {
		// Seek to the last cluster part
		_fd->seek(status.curPos);

		// Look until we get to the block
		while (_fd->pos() < status.curCluster->offset + status.curCluster->size) {
			uint32_t id = readElementID(*_fd);
			uint64_t size = readElementSize(*_fd);
			uint64_t endPos = _fd->pos() + size;

			// Block group, should contain one block, so nest into it
			if (id == kMatroskaIDBlockGroup) {
				id = readElementID(*_fd);
				readElementSize(*_fd);
			}

			// Skip anything that's not a block or simple block
			if ((id != kMatroskaIDBlock) && (id != kMatroskaIDSimpleBlock)) {
				_fd->seek(endPos);
				status.curPos = endPos;
				continue;
			}

			// Read the simple block header
			uint64_t blockTrackNumber = readElementSize(*_fd);
			int16_t blockTimeCode = _fd->readSint16BE();
			byte blockFlags = _fd->readByte();

			// If the block isn't ours, continue on
			if (blockTrackNumber != trackNumber) {
				_fd->seek(endPos);
				status.curPos = endPos;
				continue;
			}

			// TODO: Lacing
			if ((blockFlags & 0x60) != 0)
				throw Common::Exception("Unhandled Matroska lacing");

			// Fill in the packet info
			packet.offset = _fd->pos();
			packet.size = endPos - packet.offset;
			packet.timestamp = (status.curCluster->timestamp + blockTimeCode) * _timeCodeScale;

			// Update the track position for next time
			status.curPos = endPos;

			// We've got this packet now
			return;
		}

		// If we're at the last cluster, we're done
		if (status.curCluster == &_clusters[_clusters.size() - 1])
			break;

		// Go to the next cluster
		status.curCluster++;
		_fd->seek(status.curCluster->offset);
		readEBMLEntrySingle(status.curCluster->timestamp, kMatroskaIDTimeCode, *_fd);
		status.curPos = _fd->pos();
	}

	// No more packets left; mark it as such
	packet = Packet();
	packet.timestamp = std::numeric_limits<uint64_t>::max();
}

Matroska::MatroskaVideoTrack::MatroskaVideoTrack(uint64_t trackNumber, uint32_t width, uint32_t height, uint64_t defaultDuration) : _trackNumber(trackNumber), _width(width), _height(height), _defaultDuration(defaultDuration), _curFrame(-1), _finished(false), _timestamp(0) {
}

void Matroska::MatroskaVideoTrack::decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &frameData, uint64_t timestamp) {
	_videoCodec->decodeFrame(surface, frameData);
	_curFrame++;

	if (timestamp == std::numeric_limits<uint64_t>::max())
		_timestamp += _defaultDuration;
	else
		_timestamp = timestamp;
}

void Matroska::MatroskaVideoTrack::initCodec(const std::string &codec, Common::SeekableReadStream *UNUSED(extraData)) {
	if (codec == "V_VP8")
#ifdef ENABLE_VPX
		_videoCodec.reset(makeVP8Decoder());
#else
		throw Common::Exception("Missing libvpx support");
#endif

	if (!_videoCodec)
		throw Common::Exception("Unknown Matroska codec %s", codec.c_str());
}

Common::Timestamp Matroska::MatroskaVideoTrack::getNextFrameStartTime() const {
	if (_curFrame < 0)
		return 0;

	return makeTimestamp(_timestamp);
}

Matroska::MatroskaAudioTrack::MatroskaAudioTrack(uint64_t trackNumber, uint32_t channelCount, uint32_t sampleRate, const std::string &codec, Common::SeekableReadStream *extraData) : _trackNumber(trackNumber), _lastTimestamp(0) {
	_audioStream.reset(createStream(channelCount, sampleRate, codec, extraData));
}

bool Matroska::MatroskaAudioTrack::canBufferData() const {
	return !_audioStream->endOfStream();
}

void Matroska::MatroskaAudioTrack::queueAudio(Common::SeekableReadStream *stream) {
	_audioStream->queuePacket(stream);
}

void Matroska::MatroskaAudioTrack::finish() {
	_audioStream->finish();
}

Sound::AudioStream *Matroska::MatroskaAudioTrack::getAudioStream() const {
	return _audioStream.get();
}

Sound::PacketizedAudioStream *Matroska::MatroskaAudioTrack::createStream(uint32_t UNUSED(channelCount), uint32_t UNUSED(sampleRate), const std::string &codec, Common::SeekableReadStream *extraData) const {
	if (codec == "A_VORBIS") {
		if (!extraData)
			throw Common::Exception("Missing Vorbis extra data");

#if ENABLE_VORBIS
		return Sound::makePacketizedVorbisStream(*extraData);
#else
		throw Common::Exception("Missing libvorbis support");
#endif
	}

	throw Common::Exception("Unknown Matroska audio codec: %s", codec.c_str());
}

} // End of namespace Video

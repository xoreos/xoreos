/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file graphics/video/quicktime.cpp
 *  Decoding Apple QuickTime videos.
 */

//
// Heavily based on ScummVM code which is heavily based on ffmpeg code.
//
// Copyright (c) 2001 Fabrice Bellard.
// First version by Francois Revol revol@free.fr
// Seek function by Gael Chardon gael.dev@4now.net
//

#include "common/error.h"
#include "common/stream.h"
#include "events/events.h"
#include "graphics/video/quicktime.h"
#include "sound/audiostream.h"

// Audio codecs
#include "sound/decoders/adpcm.h"
#include "sound/decoders/pcm.h"

// Video codecs
// (None yet :P)

namespace Graphics {

////////////////////////////////////////////
// QuickTimeDecoder
////////////////////////////////////////////

static const char *tag2str(uint32 tag) {
	static char string[5];
	string[0] = (tag >> 24) & 0xff;
	string[1] = (tag >> 16) & 0xff;
	string[2] = (tag >> 8) & 0xff;
	string[3] = tag & 0xff;
	string[4] = 0;
	return string;
}

QuickTimeDecoder::QuickTimeDecoder(Common::SeekableReadStream *stream) : VideoDecoder() {
	initParseTable();
	_fd = stream;
	_foundMOOV = false;
	_videoTrackIndex = _audioTrackIndex = -1;
	_startTime = _nextFrameStartTime = 0;
	_curFrame = -1;

	Atom atom = { 0, 0, 0xffffffff };

	if (readDefault(atom) < 0 || !_foundMOOV)
		throw Common::Exception("Not a valid QuickTime video");

	// Remove unknown/unhandled tracks
	for (uint32 i = 0; i < _tracks.size(); i++) {
		if (_tracks[i]->codecType == CODEC_TYPE_MOV_OTHER) {
			delete _tracks[i];
			_tracks.erase(_tracks.begin() + i);
			i--;
		}
	}

	// Adjust time scale
	for (uint32 i = 0; i < _tracks.size(); i++) {
		if (!_tracks[i]->timeScale)
			_tracks[i]->timeScale = _timeScale;

		if (_tracks[i]->codecType == CODEC_TYPE_VIDEO && _videoTrackIndex < 0)
			_videoTrackIndex = i;
		else if (_tracks[i]->codecType == CODEC_TYPE_AUDIO && _audioTrackIndex < 0)
			_audioTrackIndex = i;

	}

	// Initialize audio, if present
	if (_audioTrackIndex >= 0) {
		AudioSampleDesc *entry = (AudioSampleDesc *)_tracks[_audioTrackIndex]->sampleDescs[0];

		if (entry->isAudioCodecSupported()) {
			_curAudioChunk = 0;

			// Make sure the bits per sample transfers to the sample size
			if (entry->getCodecTag() == MKID_BE('raw ') || entry->getCodecTag() == MKID_BE('twos'))
				_tracks[_audioTrackIndex]->sampleSize = (entry->_bitsPerSample / 8) * entry->_channels;

			initSound(entry->_sampleRate, entry->_channels == 2, true);
			updateAudioBuffer();
		} else
			_audioTrackIndex = -1; // Invalidate the stream
	}

	// Yeah, this wouldn't be a video if there's no video stream :P
	assert(_videoTrackIndex >= 0);

	// Initialize video, if present
	for (uint32 i = 0; i < _tracks[_videoTrackIndex]->sampleDescs.size(); i++)
		((VideoSampleDesc *)_tracks[_videoTrackIndex]->sampleDescs[i])->initCodec();

	_width = _tracks[_videoTrackIndex]->width;
	_height = _tracks[_videoTrackIndex]->height;
	// TODO: Pitch
}

QuickTimeDecoder::~QuickTimeDecoder() {
	removeFromQueue(kQueueGLContainer);
	removeFromQueue(kQueueVideo);

	for (uint32 i = 0; i < _tracks.size(); i++)
		delete _tracks[i];

	delete _fd;
}

QuickTimeDecoder::SampleDesc *QuickTimeDecoder::readSampleDesc(Track *track, uint32 format) {
	if (track->codecType == CODEC_TYPE_VIDEO) {
		VideoSampleDesc *entry = new VideoSampleDesc(track, format);

		_fd->readUint16BE(); // version
		_fd->readUint16BE(); // revision level
		_fd->readUint32BE(); // vendor
		_fd->readUint32BE(); // temporal quality
		_fd->readUint32BE(); // spacial quality

		uint16 width = _fd->readUint16BE(); // width
		uint16 height = _fd->readUint16BE(); // height

		// The width is most likely invalid for entries after the first one
		// so only set the overall width if it is not zero here.
		if (width)
			track->width = width;

		if (height)
			track->height = height;

		_fd->readUint32BE(); // horiz resolution
		_fd->readUint32BE(); // vert resolution
		_fd->readUint32BE(); // data size, always 0
		_fd->readUint16BE(); // frames per samples

		byte codecName[32];
		_fd->read(codecName, 32); // codec name, pascal string (FIXME: true for mp4?)
		if (codecName[0] <= 31) {
			memcpy(entry->_codecName, &codecName[1], codecName[0]);
			entry->_codecName[codecName[0]] = 0;
		}

		entry->_bitsPerSample = _fd->readUint16BE(); // depth
		entry->_colorTableId = _fd->readUint16BE(); // colortable id

		// figure out the palette situation
		byte colorDepth = entry->_bitsPerSample & 0x1F;

		// if the depth is 2, 4, or 8 bpp, file is palettized
		if (colorDepth == 2 || colorDepth == 4 || colorDepth == 8)
			throw Common::Exception("No paletted video support");

		return entry;
	} else if (track->codecType == CODEC_TYPE_AUDIO) {
		AudioSampleDesc *entry = new AudioSampleDesc(track, format);

		uint16 stsdVersion = _fd->readUint16BE();
		_fd->readUint16BE(); // revision level
		_fd->readUint32BE(); // vendor

		entry->_channels = _fd->readUint16BE();			 // channel count
		entry->_bitsPerSample = _fd->readUint16BE();	  // sample size

		_fd->readUint16BE(); // compression id = 0
		_fd->readUint16BE(); // packet size = 0

		entry->_sampleRate = (_fd->readUint32BE() >> 16);

		if (stsdVersion == 0) {
			// Not used, except in special cases. See below.
			entry->_samplesPerFrame = entry->_bytesPerFrame = 0;
		} else if (stsdVersion == 1) {
			// Read QT version 1 fields. In version 0 these dont exist.
			entry->_samplesPerFrame = _fd->readUint32BE();
			_fd->readUint32BE(); // bytes per packet
			entry->_bytesPerFrame = _fd->readUint32BE();
			_fd->readUint32BE(); // bytes per sample
		} else {
			warning("Unsupported QuickTime STSD audio version %d", stsdVersion);
			delete entry;
			return 0;
		}

		// Version 0 videos (such as the Riven ones) don't have this set,
		// but we need it later on. Add it in here.
		if (format == MKID_BE('ima4')) {
			entry->_samplesPerFrame = 64;
			entry->_bytesPerFrame = 34 * entry->_channels;
		}

		if (entry->_sampleRate == 0 && track->timeScale > 1)
			entry->_sampleRate = track->timeScale;

		return entry;
	}

	return 0;
}

bool QuickTimeDecoder::isOldDemuxing() const {
	assert(_audioTrackIndex >= 0);
	return _tracks[_audioTrackIndex]->timeToSampleCount == 1 && _tracks[_audioTrackIndex]->timeToSample[0].duration == 1;
}

uint32 QuickTimeDecoder::getFrameDuration() {
	if (_videoTrackIndex < 0)
		return 0;

	uint32 curFrameIndex = 0;
	for (int32 i = 0; i < _tracks[_videoTrackIndex]->timeToSampleCount; i++) {
		curFrameIndex += _tracks[_videoTrackIndex]->timeToSample[i].count;
		if ((uint32)_curFrame < curFrameIndex) {
			// Ok, now we have what duration this frame has.
			return _tracks[_videoTrackIndex]->timeToSample[i].duration;
		}
	}

	// This should never occur
	error("Cannot find duration for frame %d", _curFrame);
	return 0;
}

Codec *QuickTimeDecoder::findDefaultVideoCodec() const {
	if (_videoTrackIndex < 0 || _tracks[_videoTrackIndex]->sampleDescs.empty())
		return 0;

	return ((VideoSampleDesc *)_tracks[_videoTrackIndex]->sampleDescs[0])->_videoCodec;
}

void QuickTimeDecoder::processData() {
	if (_curFrame >= (int32)_tracks[_videoTrackIndex]->frameCount - 1) {
		_finished = true;
		return;
	}

	_started = true;

	if (_startTime == 0)
		_startTime = EventMan.getTimestamp();

	if (getTimeToNextFrame() > 0)
		return;

	_curFrame++;
	_nextFrameStartTime += getFrameDuration();

	// Update the audio while we're at it
	updateAudioBuffer();

	// Get the next packet
	uint32 descId;
	Common::SeekableReadStream *frameData = getNextFramePacket(descId);

	if (!frameData || !descId || descId > _tracks[_videoTrackIndex]->sampleDescs.size())
		return;

	// Find which video description entry we want
	VideoSampleDesc *entry = (VideoSampleDesc *)_tracks[_videoTrackIndex]->sampleDescs[descId - 1];

	if (entry->_videoCodec) {
		// TODO: Lots
		// Like with _needCopy
		/*
		const Graphics::Surface *frame = entry->videoCodec->decodeImage(frameData);
		delete frameData;

		return frame;
		*/
	}
}

uint32 QuickTimeDecoder::getElapsedTime() const {
	// TODO: Extend decoder to get the elapsed time of the sound
	//	return g_system->getMixer()->getSoundElapsedTime(_audHandle);

	return EventMan.getTimestamp() - _startTime;
}

uint32 QuickTimeDecoder::getTimeToNextFrame() const {
	if (!_started || _curFrame < 0)
		return 0;

	// Convert from the QuickTime rate base to 1000
	uint32 nextFrameStartTime = _nextFrameStartTime * 1000 / _tracks[_videoTrackIndex]->timeScale;
	uint32 elapsedTime = getElapsedTime();

	if (nextFrameStartTime <= elapsedTime)
		return 0;

	return nextFrameStartTime - elapsedTime;
}

bool QuickTimeDecoder::hasTime() const {
	return getTimeToNextFrame() >= 10;
}

void QuickTimeDecoder::initParseTable() {
	static const ParseTable p[] = {
		{ &QuickTimeDecoder::readDefault, MKID_BE('dinf') },
		{ &QuickTimeDecoder::readLeaf,    MKID_BE('dref') },
		{ &QuickTimeDecoder::readDefault, MKID_BE('edts') },
		{ &QuickTimeDecoder::readELST,    MKID_BE('elst') },
		{ &QuickTimeDecoder::readHDLR,    MKID_BE('hdlr') },
		{ &QuickTimeDecoder::readLeaf,    MKID_BE('mdat') },
		{ &QuickTimeDecoder::readMDHD,    MKID_BE('mdhd') },
		{ &QuickTimeDecoder::readDefault, MKID_BE('mdia') },
		{ &QuickTimeDecoder::readDefault, MKID_BE('minf') },
		{ &QuickTimeDecoder::readMOOV,    MKID_BE('moov') },
		{ &QuickTimeDecoder::readMVHD,    MKID_BE('mvhd') },
		{ &QuickTimeDecoder::readLeaf,    MKID_BE('smhd') },
		{ &QuickTimeDecoder::readDefault, MKID_BE('stbl') },
		{ &QuickTimeDecoder::readSTCO,    MKID_BE('stco') },
		{ &QuickTimeDecoder::readSTSC,    MKID_BE('stsc') },
		{ &QuickTimeDecoder::readSTSD,    MKID_BE('stsd') },
		{ &QuickTimeDecoder::readSTSS,    MKID_BE('stss') },
		{ &QuickTimeDecoder::readSTSZ,    MKID_BE('stsz') },
		{ &QuickTimeDecoder::readSTTS,    MKID_BE('stts') },
		{ &QuickTimeDecoder::readTRAK,    MKID_BE('trak') },
		{ &QuickTimeDecoder::readLeaf,    MKID_BE('udta') },
		{ &QuickTimeDecoder::readLeaf,    MKID_BE('vmhd') },
		{ &QuickTimeDecoder::readDefault, MKID_BE('wave') },
		{ &QuickTimeDecoder::readESDS,    MKID_BE('esds') },
		{ 0, 0 }
	};

	_parseTable = p;
}

int QuickTimeDecoder::readDefault(Atom atom) {
	uint32 total_size = 0;
	Atom a;
	int err = 0;

	a.offset = atom.offset;

	while (((total_size + 8) < atom.size) && !_fd->eos() && _fd->pos() < _fd->size() && !err) {
		a.size = atom.size;
		a.type = 0;

		if (atom.size >= 8) {
			a.size = _fd->readUint32BE();
			a.type = _fd->readUint32BE();

			// Some QuickTime videos with resource forks have mdat chunks
			// that are of size 0. Adjust it so it's the correct size.
			if (a.type == MKID_BE('mdat') && a.size == 0)
				a.size = _fd->size();
		}

		total_size += 8;
		a.offset += 8;

		if (a.size == 1) { // 64 bit extended size
			warning("64 bit extended size is not supported in QuickTime");
			return -1;
		}

		if (a.size == 0) {
			a.size = atom.size - total_size;
			if (a.size <= 8)
				break;
		}

		uint32 i = 0;

		for (; _parseTable[i].type != 0 && _parseTable[i].type != a.type; i++)
			; // Empty

		if (a.size < 8)
			break;

		a.size -= 8;

		if (_parseTable[i].type == 0) {
			// skip leaf atoms data
			_fd->seek(a.size, SEEK_CUR);
		} else {
			uint32 start_pos = _fd->pos();
			err = (this->*_parseTable[i].func)(a);

			uint32 left = a.size - _fd->pos() + start_pos;

			if (left > 0) // skip garbage at atom end
				_fd->seek(left, SEEK_CUR);
		}

		a.offset += a.size;
		total_size += a.size;
	}

	if (!err && total_size < atom.size)
		_fd->seek(atom.size - total_size, SEEK_SET);

	return err;
}

int QuickTimeDecoder::readLeaf(Atom atom) {
	if (atom.size > 1)
		_fd->seek(atom.size, SEEK_SET);

	return 0;
}

int QuickTimeDecoder::readMOOV(Atom atom) {
	if (readDefault(atom) < 0)
		return -1;

	// We parsed the 'moov' atom, so we don't need anything else
	_foundMOOV = true;
	return 1;
}

int QuickTimeDecoder::readMVHD(Atom atom) {
	byte version = _fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	if (version == 1) {
		warning("QuickTime version 1");
		_fd->readUint32BE(); _fd->readUint32BE();
		_fd->readUint32BE(); _fd->readUint32BE();
	} else {
		_fd->readUint32BE(); // creation time
		_fd->readUint32BE(); // modification time
	}

	_timeScale = _fd->readUint32BE(); // time scale

	// We're just skipping *everything* else in this atom :P

	return 0;
}

int QuickTimeDecoder::readTRAK(Atom atom) {
	Track *track = new Track();

	if (!track)
		return -1;

	track->codecType = CODEC_TYPE_MOV_OTHER;
	track->startTime = 0; // XXX: check
	_tracks.push_back(track);

	return readDefault(atom);
}

// edit list atom
int QuickTimeDecoder::readELST(Atom atom) {
	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags
	uint32 editCount = _fd->readUint32BE();	 // entries

	for (uint32 i = 0; i < editCount; i++){
		_fd->readUint32BE(); // Track duration
		_fd->readUint32BE(); // Media time
		_fd->readUint32BE(); // Media rate
	}

	if (editCount != 1)
		warning("Multiple edit list entries. Things may go awry");

	return 0;
}

int QuickTimeDecoder::readHDLR(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	// component type
	/* uint32 ctype = */ _fd->readUint32BE();
	uint32 type = _fd->readUint32BE(); // component subtype

	if (type == MKID_BE('vide'))
		track->codecType = CODEC_TYPE_VIDEO;
	else if (type == MKID_BE('soun'))
		track->codecType = CODEC_TYPE_AUDIO;

	_fd->readUint32BE(); // component  manufacture
	_fd->readUint32BE(); // component flags
	_fd->readUint32BE(); // component flags mask

	if (atom.size <= 24)
		return 0; // nothing left to read

	// .mov: PASCAL string
	byte len = _fd->readByte();
	_fd->seek(len, SEEK_CUR);

	_fd->seek(atom.size - (_fd->pos() - atom.offset), SEEK_CUR);

	return 0;
}

int QuickTimeDecoder::readMDHD(Atom atom) {
	Track *track = _tracks.back();
	byte version = _fd->readByte();

	if (version > 1)
		return 1; // unsupported

	_fd->readByte(); _fd->readByte();
	_fd->readByte(); // flags

	if (version == 1) {
		_fd->readUint32BE(); _fd->readUint32BE();
		_fd->readUint32BE(); _fd->readUint32BE();
	} else {
		_fd->readUint32BE(); // creation time
		_fd->readUint32BE(); // modification time
	}

	track->timeScale = _fd->readUint32BE();
	track->duration = (version == 1) ? (_fd->readUint32BE(), _fd->readUint32BE()) : _fd->readUint32BE(); // duration

	_fd->readUint16BE(); // language
	_fd->readUint16BE(); // quality

	return 0;
}

int QuickTimeDecoder::readSTSD(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	uint32 entryCount = _fd->readUint32BE();
	track->sampleDescs.resize(entryCount);

	for (uint32 i = 0; i < entryCount; i++) { // Parsing Sample description table
		Atom a = { 0, 0, 0 };
		uint32 start_pos = _fd->pos();
		int size = _fd->readUint32BE(); // size
		uint32 format = _fd->readUint32BE(); // data format

		_fd->readUint32BE(); // reserved
		_fd->readUint16BE(); // reserved
		_fd->readUint16BE(); // index

		track->sampleDescs[i] = readSampleDesc(track, format);

		if (!track->sampleDescs[i]) {
			// other codec type, just skip (rtp, mp4s, tmcd ...)
			_fd->seek(size - (_fd->pos() - start_pos), SEEK_CUR);
		}

		// this will read extra atoms at the end (wave, alac, damr, avcC, SMI ...)
		a.size = size - (_fd->pos() - start_pos);
		if (a.size > 8)
			readDefault(a);
		else if (a.size > 0)
			_fd->seek(a.size, SEEK_CUR);
	}

	return 0;
}

int QuickTimeDecoder::readSTSC(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	track->sampleToChunkCount = _fd->readUint32BE();

	track->sampleToChunk = new SampleToChunkEntry[track->sampleToChunkCount];

	if (!track->sampleToChunk)
		return -1;

	for (uint32 i = 0; i < track->sampleToChunkCount; i++) {
		track->sampleToChunk[i].first = _fd->readUint32BE() - 1;
		track->sampleToChunk[i].count = _fd->readUint32BE();
		track->sampleToChunk[i].id = _fd->readUint32BE();
		//warning("Sample to Chunk[%d]: First = %d, Count = %d", i, track->sampleToChunk[i].first, track->sampleToChunk[i].count);
	}

	return 0;
}

int QuickTimeDecoder::readSTSS(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	track->keyframeCount = _fd->readUint32BE();
	track->keyframes = new uint32[track->keyframeCount];

	if (!track->keyframes)
		return -1;

	for (uint32 i = 0; i < track->keyframeCount; i++)
		track->keyframes[i] = _fd->readUint32BE() - 1; // Adjust here, the frames are based on 1

	return 0;
}

int QuickTimeDecoder::readSTSZ(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	track->sampleSize = _fd->readUint32BE();
	track->sampleCount = _fd->readUint32BE();

	if (track->sampleSize)
		return 0; // there isn't any table following

	track->sampleSizes = new uint32[track->sampleCount];

	if (!track->sampleSizes)
		return -1;

	for (uint32 i = 0; i < track->sampleCount; i++)
		track->sampleSizes[i] = _fd->readUint32BE();

	return 0;
}

int QuickTimeDecoder::readSTTS(Atom atom) {
	Track *track = _tracks.back();
	uint32 totalSampleCount = 0;

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	track->timeToSampleCount = _fd->readUint32BE();
	track->timeToSample = new TimeToSampleEntry[track->timeToSampleCount];

	for (int32 i = 0; i < track->timeToSampleCount; i++) {
		track->timeToSample[i].count = _fd->readUint32BE();
		track->timeToSample[i].duration = _fd->readUint32BE();

		totalSampleCount += track->timeToSample[i].count;
	}

	track->frameCount = totalSampleCount;

	return 0;
}

int QuickTimeDecoder::readSTCO(Atom atom) {
	Track *track = _tracks.back();

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	track->chunkCount = _fd->readUint32BE();
	track->chunkOffsets = new uint32[track->chunkCount];

	if (!track->chunkOffsets)
		return -1;

	for (uint32 i = 0; i < track->chunkCount; i++)
		track->chunkOffsets[i] = _fd->readUint32BE();

	return 0;
}

enum {
	kMP4IODescTag          = 2,
	kMP4ESDescTag          = 3,
	kMP4DecConfigDescTag   = 4,
	kMP4DecSpecificDescTag = 5
};

static int readMP4DescLength(Common::SeekableReadStream *stream) {
	int length = 0;
	int count = 4;

	while (count--) {
		byte c = stream->readByte();
		length = (length << 7) | (c & 0x7f);

		if (!(c & 0x80))
			break;
	}

	return length;
}

static void readMP4Desc(Common::SeekableReadStream *stream, byte &tag, int &length) {
	tag = stream->readByte();
	length = readMP4DescLength(stream);
}

int QuickTimeDecoder::readESDS(Atom atom) {
	if (_tracks.empty())
		return 0;

	Track *track = _tracks.back();

	_fd->readUint32BE(); // version + flags

	byte tag;
	int length;

	readMP4Desc(_fd, tag, length);
	_fd->readUint16BE(); // id
	if (tag == kMP4ESDescTag)
		_fd->readByte(); // priority

	// Check if we've got the Config MPEG-4 header
	readMP4Desc(_fd, tag, length);
	if (tag != kMP4DecConfigDescTag)
		return 0;

	track->objectTypeMP4 = _fd->readByte();
	_fd->readByte();                      // stream type
	_fd->readUint16BE(); _fd->readByte(); // buffer size
	_fd->readUint32BE();                  // max bitrate
	_fd->readUint32BE();                  // avg bitrate

	// Check if we've got the Specific MPEG-4 header
	readMP4Desc(_fd, tag, length);
	if (tag != kMP4DecSpecificDescTag)
		return 0;

	track->extraData = _fd->readStream(length);

	return 0;
}

Common::SeekableReadStream *QuickTimeDecoder::getNextFramePacket(uint32 &descId) {
	if (_videoTrackIndex < 0)
		return NULL;

	// First, we have to track down which chunk holds the sample and which sample in the chunk contains the frame we are looking for.
	int32 totalSampleCount = 0;
	int32 sampleInChunk = 0;
	int32 actualChunk = -1;

	for (uint32 i = 0; i < _tracks[_videoTrackIndex]->chunkCount; i++) {
		int32 sampleToChunkIndex = -1;

		for (uint32 j = 0; j < _tracks[_videoTrackIndex]->sampleToChunkCount; j++)
			if (i >= _tracks[_videoTrackIndex]->sampleToChunk[j].first)
				sampleToChunkIndex = j;

		if (sampleToChunkIndex < 0)
			error("This chunk (%d) is imaginary", sampleToChunkIndex);

		totalSampleCount += _tracks[_videoTrackIndex]->sampleToChunk[sampleToChunkIndex].count;

		if (totalSampleCount > _curFrame) {
			actualChunk = i;
			descId = _tracks[_videoTrackIndex]->sampleToChunk[sampleToChunkIndex].id;
			sampleInChunk = _tracks[_videoTrackIndex]->sampleToChunk[sampleToChunkIndex].count - totalSampleCount + _curFrame;
			break;
		}
	}

	if (actualChunk < 0) {
		warning("Could not find data for frame %d", _curFrame);
		return NULL;
	}

	// Next seek to that frame
	_fd->seek(_tracks[_videoTrackIndex]->chunkOffsets[actualChunk]);

	// Then, if the chunk holds more than one frame, seek to where the frame we want is located
	for (int32 i = _curFrame - sampleInChunk; i < _curFrame; i++) {
		if (_tracks[_videoTrackIndex]->sampleSize != 0)
			_fd->skip(_tracks[_videoTrackIndex]->sampleSize);
		else
			_fd->skip(_tracks[_videoTrackIndex]->sampleSizes[i]);
	}

	// Finally, read in the raw data for the frame
	//printf ("Frame Data[%d]: Offset = %d, Size = %d\n", getCurFrame(), _fd->pos(), _tracks[_videoTrackIndex]->sampleSizes[getCurFrame()]);

	if (_tracks[_videoTrackIndex]->sampleSize != 0)
		return _fd->readStream(_tracks[_videoTrackIndex]->sampleSize);

	return _fd->readStream(_tracks[_videoTrackIndex]->sampleSizes[_curFrame]);
}

void QuickTimeDecoder::queueNextAudioChunk() {
	AudioSampleDesc *entry = (AudioSampleDesc *)_tracks[_audioTrackIndex]->sampleDescs[0];
	Common::MemoryWriteStreamDynamic *wStream = new Common::MemoryWriteStreamDynamic();

	_fd->seek(_tracks[_audioTrackIndex]->chunkOffsets[_curAudioChunk]);

	// First, we have to get the sample count
	uint32 sampleCount = entry->getAudioChunkSampleCount(_curAudioChunk);
	assert(sampleCount);

	if (isOldDemuxing()) {
		// Old-style audio demuxing

		// Then calculate the right sizes
		while (sampleCount > 0) {
			uint32 samples = 0, size = 0;

			if (entry->_samplesPerFrame >= 160) {
				samples = entry->_samplesPerFrame;
				size = entry->_bytesPerFrame;
			} else if (entry->_samplesPerFrame > 1) {
				samples = MIN<uint32>((1024 / entry->_samplesPerFrame) * entry->_samplesPerFrame, sampleCount);
				size = (samples / entry->_samplesPerFrame) * entry->_bytesPerFrame;
			} else {
				samples = MIN<uint32>(1024, sampleCount);
				size = samples * _tracks[_audioTrackIndex]->sampleSize;
			}

			// Now, we read in the data for this data and output it
			byte *data = (byte *)malloc(size);
			_fd->read(data, size);
			wStream->write(data, size);
			free(data);
			sampleCount -= samples;
		}
	} else {
		// New-style audio demuxing

		// Find our starting sample
		uint32 startSample = 0;
		for (uint32 i = 0; i < _curAudioChunk; i++)
			startSample += entry->getAudioChunkSampleCount(i);

		for (uint32 i = 0; i < sampleCount; i++) {
			uint32 size = (_tracks[_audioTrackIndex]->sampleSize != 0) ? _tracks[_audioTrackIndex]->sampleSize : _tracks[_audioTrackIndex]->sampleSizes[i + startSample];

			// Now, we read in the data for this data and output it
			byte *data = (byte *)malloc(size);
			_fd->read(data, size);
			wStream->write(data, size);
			free(data);
		}
	}

	// Now queue the buffer
	queueSound(entry->createAudioStream(new Common::MemoryReadStream(wStream->getData(), wStream->size(), true)));
	delete wStream;

	_curAudioChunk++;
}

void QuickTimeDecoder::updateAudioBuffer() {
	if (_audioTrackIndex < 0)
		return;

	uint32 numberOfChunksNeeded = 0;

	if (_curFrame == (int32)_tracks[_videoTrackIndex]->frameCount - 1) {
		// If we're on the last frame, make sure all audio remaining is buffered
		numberOfChunksNeeded = _tracks[_audioTrackIndex]->chunkCount;
	} else {
		AudioSampleDesc *entry = (AudioSampleDesc *)_tracks[_audioTrackIndex]->sampleDescs[0];

		// Calculate the amount of chunks we need in memory until the next frame
		uint32 timeToNextFrame = getTimeToNextFrame();
		uint32 timeFilled = 0;
		uint32 curAudioChunk = _curAudioChunk - getNumQueuedStreams();

		for (; timeFilled < timeToNextFrame && curAudioChunk < _tracks[_audioTrackIndex]->chunkCount; numberOfChunksNeeded++, curAudioChunk++) {
			uint32 sampleCount = entry->getAudioChunkSampleCount(curAudioChunk);
			assert(sampleCount);

			timeFilled += sampleCount * 1000 / entry->_sampleRate;
		}

		// Add a couple extra to ensure we don't underrun
		numberOfChunksNeeded += 3;
	}

	// Keep three streams in buffer so that if/when the first two end, it goes right into the next
	while (getNumQueuedStreams() < numberOfChunksNeeded && _curAudioChunk < _tracks[_audioTrackIndex]->chunkCount)
		queueNextAudioChunk();
}

QuickTimeDecoder::SampleDesc::SampleDesc(QuickTimeDecoder::Track *parentTrack, uint32 codecTag) {
	_parentTrack = parentTrack;
	_codecTag = codecTag;
}

QuickTimeDecoder::Track::Track() {
	chunkCount = 0;
	chunkOffsets = 0;
	timeToSampleCount = 0;
	timeToSample = 0;
	sampleToChunkCount = 0;
	sampleToChunk = 0;
	sampleSize = 0;
	sampleCount = 0;
	sampleSizes = 0;
	keyframeCount = 0;
	keyframes = 0;
	timeScale = 0;
	width = 0;
	height = 0;
	codecType = CODEC_TYPE_MOV_OTHER;
	extraData = 0;
	frameCount = 0;
	duration = 0;
	startTime = 0;
	objectTypeMP4 = 0;
}

QuickTimeDecoder::Track::~Track() {
	delete[] chunkOffsets;
	delete[] timeToSample;
	delete[] sampleToChunk;
	delete[] sampleSizes;
	delete[] keyframes;
	delete extraData;

	for (uint32 i = 0; i < sampleDescs.size(); i++)
		delete sampleDescs[i];
}

QuickTimeDecoder::AudioSampleDesc::AudioSampleDesc(QuickTimeDecoder::Track *parentTrack, uint32 codecTag) : QuickTimeDecoder::SampleDesc(parentTrack, codecTag) {
	_channels = 0;
	_sampleRate = 0;
	_samplesPerFrame = 0;
	_bytesPerFrame = 0;
	_bitsPerSample = 0;
}

bool QuickTimeDecoder::AudioSampleDesc::isAudioCodecSupported() const {
	// Check if the codec is a supported codec
	if (_codecTag == MKID_BE('twos') || _codecTag == MKID_BE('raw ') || _codecTag == MKID_BE('ima4'))
		return true;

	if (_codecTag == MKID_BE('mp4a')) {
		Common::UString audioType;

		switch (_parentTrack->objectTypeMP4) {
		case 0x40:
			audioType = "AAC";
			break;
		default:
			audioType = "Unknown";
			break;
		}

		warning("No MPEG-4 audio (%s) support", audioType.c_str());
	} else
		warning("Audio Codec Not Supported: \'%s\'", tag2str(_codecTag));

	return false;
}

uint32 QuickTimeDecoder::AudioSampleDesc::getAudioChunkSampleCount(uint chunk) const {
	uint32 sampleCount = 0;

	for (uint32 j = 0; j < _parentTrack->sampleToChunkCount; j++)
		if (chunk >= _parentTrack->sampleToChunk[j].first)
			sampleCount = _parentTrack->sampleToChunk[j].count;

	return sampleCount;
}

Sound::AudioStream *QuickTimeDecoder::AudioSampleDesc::createAudioStream(Common::SeekableReadStream *stream) const {
	if (!stream)
		return 0;

	if (_codecTag == MKID_BE('twos') || _codecTag == MKID_BE('raw ')) {
		// Standard PCM
		uint16 flags = 0;
		if (_codecTag == MKID_BE('raw '))
			flags |= Sound::FLAG_UNSIGNED;
		if (_channels == 2)
			flags |= Sound::FLAG_STEREO;
		if (_bitsPerSample == 16)
			flags |= Sound::FLAG_16BITS;

		return Sound::makePCMStream(stream, _sampleRate, flags);
	} else if (_codecTag == MKID_BE('ima4')) {
		// QuickTime IMA ADPCM
		return Sound::makeADPCMStream(stream, true, stream->size(), Sound::kADPCMApple, _sampleRate, _channels, 34);
	} else if (_codecTag == MKID_BE('mp4a')) {
		// TODO: MPEG-4 Audio
		throw Common::Exception("Unhandled MPEG-4 audio");
	}

	return 0;
}

QuickTimeDecoder::VideoSampleDesc::VideoSampleDesc(QuickTimeDecoder::Track *parentTrack, uint32 codecTag) : QuickTimeDecoder::SampleDesc(parentTrack, codecTag) {
	memset(_codecName, 0, 32);
	_colorTableId = 0;
	_palette = 0;
	_videoCodec = 0;
	_bitsPerSample = 0;
}

QuickTimeDecoder::VideoSampleDesc::~VideoSampleDesc() {
	delete[] _palette;
	delete _videoCodec;
}

void QuickTimeDecoder::VideoSampleDesc::initCodec() {
	if (_codecTag == MKID_BE('mp4v')) {
		Common::UString videoType;
	
		// Parse the object type
		switch (_parentTrack->objectTypeMP4) {
		case 0x20:
			videoType = "h.263";
			break;
		default:
			videoType = "Unknown";
			break;
		}

		// TODO: MPEG-4 Video
		warning("MPEG-4 Video (%s) not yet supported", videoType.c_str());
	} else if (_codecTag == MKID_BE('SVQ3')) {
		// TODO: Sorenson Video 3
		warning("Sorenson Video 3 not yet supported");
	} else {
		warning("Unsupported codec \'%s\'", tag2str(_codecTag));
	}
}

} // End of namespace Video

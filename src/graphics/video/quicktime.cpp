/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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
	_numStreams = 0;
	_videoStreamIndex = _audioStreamIndex = -1;
	_startTime = _nextFrameStartTime = 0;
	_curFrame = -1;

	MOVatom atom = { 0, 0, 0xffffffff };

	if (readDefault(atom) < 0 || !_foundMOOV)
		throw Common::Exception("Not a valid QuickTime video");

	// Remove non-Video/Audio streams
	for (uint32 i = 0; i < _numStreams;) {
		if (_streams[i]->codec_type == CODEC_TYPE_MOV_OTHER) {
			delete _streams[i];
			for (uint32 j = i + 1; j < _numStreams; j++)
				_streams[j - 1] = _streams[j];
			_numStreams--;
		} else
			i++;
	}

	// Adjust time/duration
	for (uint32 i = 0; i < _numStreams; i++) {
		MOVStreamContext *sc = _streams[i];

		if (!sc->time_rate)
			sc->time_rate = 1;

		if (!sc->time_scale)
			sc->time_scale = _timeScale;

		sc->duration /= sc->time_rate;

		if (sc->codec_type == CODEC_TYPE_VIDEO && _videoStreamIndex < 0)
			_videoStreamIndex = i;
		else if (sc->codec_type == CODEC_TYPE_AUDIO && _audioStreamIndex < 0)
			_audioStreamIndex = i;
	}

	// Initialize audio, if present
	if (_audioStreamIndex >= 0) {
		STSDEntry *entry = &_streams[_audioStreamIndex]->stsdEntries[0];

		if (checkAudioCodecSupport(entry->codecTag)) {
			_curAudioChunk = 0;

			// Make sure the bits per sample transfers to the sample size
			if (entry->codecTag == MKID_BE('raw ') || entry->codecTag == MKID_BE('twos'))
				_streams[_audioStreamIndex]->sample_size = (entry->bitsPerSample / 8) * entry->channels;

			initSound(entry->sampleRate, entry->channels == 2, true);
			updateAudioBuffer();
		} else
			_audioStreamIndex = -1; // Invalidate the stream

	}

	// Yeah, this wouldn't be a video if there's no video stream :P
	assert(_videoStreamIndex >= 0);

	// Initialize video, if present
	for (uint32 i = 0; i < _streams[_videoStreamIndex]->stsdEntryCount; i++) {
		STSDEntry *entry = &_streams[_videoStreamIndex]->stsdEntries[i];
		entry->videoCodec = createCodec(entry->codecTag, entry->bitsPerSample & 0x1F);
	}

	_width = _streams[_videoStreamIndex]->width;
	_height = _streams[_videoStreamIndex]->height;
	// TODO: Pitch
}

QuickTimeDecoder::~QuickTimeDecoder() {
	for (uint32 i = 0; i < _numStreams; i++)
		delete _streams[i];

	delete _fd;
}

uint32 QuickTimeDecoder::getFrameDuration() {
	if (_videoStreamIndex < 0)
		return 0;

	uint32 curFrameIndex = 0;
	for (int32 i = 0; i < _streams[_videoStreamIndex]->stts_count; i++) {
		curFrameIndex += _streams[_videoStreamIndex]->stts_data[i].count;
		if ((uint32)_curFrame < curFrameIndex) {
			// Ok, now we have what duration this frame has.
			return _streams[_videoStreamIndex]->stts_data[i].duration;
		}
	}

	// This should never occur
	error ("Cannot find duration for frame %d", _curFrame);
	return 0;
}

Codec *QuickTimeDecoder::createCodec(uint32 codecTag, byte bitsPerPixel) {
	if (codecTag == MKID_BE('mp4v')) {
		// TODO: MPEG-4 Video
		warning("MPEG-4 Video not yet supported");
	} else if (codecTag == MKID_BE('SVQ3')) {
		// TODO: Sorenson Video 3
		warning("Sorenson Video 3 not yet supported");
	} else {
		warning("Unsupported codec \'%s\'", tag2str(codecTag));
	}

	return NULL;
}

Codec *QuickTimeDecoder::findDefaultVideoCodec() const {
	if (_videoStreamIndex < 0 || !_streams[_videoStreamIndex]->stsdEntryCount)
		return 0;

	return _streams[_videoStreamIndex]->stsdEntries[0].videoCodec;
}

void QuickTimeDecoder::processData() {
	if (_curFrame >= (int32)_streams[_videoStreamIndex]->nb_frames - 1) {
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

	if (!frameData || !descId || descId > _streams[_videoStreamIndex]->stsdEntryCount)
		return;

	// Find which video description entry we want
	STSDEntry *entry = &_streams[_videoStreamIndex]->stsdEntries[descId - 1];

	if (entry->videoCodec) {
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
	uint32 nextFrameStartTime = _nextFrameStartTime * 1000 / _streams[_videoStreamIndex]->time_scale;
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
		{ &QuickTimeDecoder::readDefault, MKID_BE('mdat') },
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
		{ 0, 0 }
	};

	_parseTable = p;
}

int QuickTimeDecoder::readDefault(MOVatom atom) {
	uint32 total_size = 0;
	MOVatom a;
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
			// empty;

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

int QuickTimeDecoder::readLeaf(MOVatom atom) {
	if (atom.size > 1)
		_fd->seek(atom.size, SEEK_SET);

	return 0;
}

int QuickTimeDecoder::readMOOV(MOVatom atom) {
	if (readDefault(atom) < 0)
		return -1;

	// We parsed the 'moov' atom, so we don't need anything else
	_foundMOOV = true;
	return 1;
}

int QuickTimeDecoder::readMVHD(MOVatom atom) {
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

int QuickTimeDecoder::readTRAK(MOVatom atom) {
	MOVStreamContext *sc = new MOVStreamContext();

	if (!sc)
		return -1;

	sc->codec_type = CODEC_TYPE_MOV_OTHER;
	sc->start_time = 0; // XXX: check
	_streams[_numStreams++] = sc;

	return readDefault(atom);
}

// edit list atom
int QuickTimeDecoder::readELST(MOVatom atom) {
	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags
	uint32 editCount = _streams[_numStreams - 1]->edit_count = _fd->readUint32BE();	 // entries

	for (uint32 i = 0; i < editCount; i++){
		_fd->readUint32BE(); // Track duration
		_fd->readUint32BE(); // Media time
		_fd->readUint32BE(); // Media rate
	}

	if (editCount != 1)
		warning("Multiple edit list entries. Things may go awry");

	return 0;
}

int QuickTimeDecoder::readHDLR(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	// component type
	uint32 ctype = _fd->readUint32LE();
	uint32 type = _fd->readUint32BE(); // component subtype

	if(ctype == 0) {
		warning("MP4 streams are not supported");
		return -1;
	}

	if (type == MKID_BE('vide'))
		st->codec_type = CODEC_TYPE_VIDEO;
	else if (type == MKID_BE('soun'))
		st->codec_type = CODEC_TYPE_AUDIO;

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

int QuickTimeDecoder::readMDHD(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];
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

	st->time_scale = _fd->readUint32BE();
	st->duration = (version == 1) ? (_fd->readUint32BE(), _fd->readUint32BE()) : _fd->readUint32BE(); // duration

	_fd->readUint16BE(); // language
	_fd->readUint16BE(); // quality

	return 0;
}

int QuickTimeDecoder::readSTSD(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->stsdEntryCount = _fd->readUint32BE();
	st->stsdEntries = new STSDEntry[st->stsdEntryCount];

	for (uint32 i = 0; i < st->stsdEntryCount; i++) { // Parsing Sample description table
		STSDEntry *entry = &st->stsdEntries[i];

		MOVatom a = { 0, 0, 0 };
		uint32 start_pos = _fd->pos();
		int size = _fd->readUint32BE(); // size
		uint32 format = _fd->readUint32BE(); // data format

		_fd->readUint32BE(); // reserved
		_fd->readUint16BE(); // reserved
		_fd->readUint16BE(); // index

		entry->codecTag = format;

		if (st->codec_type == CODEC_TYPE_VIDEO) {
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
				st->width = width;

			if (height)
				st->height = height;

			_fd->readUint32BE(); // horiz resolution
			_fd->readUint32BE(); // vert resolution
			_fd->readUint32BE(); // data size, always 0
			_fd->readUint16BE(); // frames per samples

			byte codec_name[32];
			_fd->read(codec_name, 32); // codec name, pascal string (FIXME: true for mp4?)
			if (codec_name[0] <= 31) {
				memcpy(entry->codecName, &codec_name[1], codec_name[0]);
				entry->codecName[codec_name[0]] = 0;
			}

			entry->bitsPerSample = _fd->readUint16BE(); // depth
			entry->colorTableId = _fd->readUint16BE(); // colortable id

			// figure out the palette situation
			byte colorDepth = entry->bitsPerSample & 0x1F;

			// if the depth is 2, 4, or 8 bpp, file is palettized
			// Also, we should *never* get that here
			assert(colorDepth > 8);
			
		} else if (st->codec_type == CODEC_TYPE_AUDIO) {
			uint16 stsdVersion = _fd->readUint16BE();
			_fd->readUint16BE(); // revision level
			_fd->readUint32BE(); // vendor

			entry->channels = _fd->readUint16BE();			 // channel count
			entry->bitsPerSample = _fd->readUint16BE();	  // sample size

			_fd->readUint16BE(); // compression id = 0
			_fd->readUint16BE(); // packet size = 0

			entry->sampleRate = (_fd->readUint32BE() >> 16);

			if (stsdVersion == 0) {
				// Not used, except in special cases. See below.
				entry->samplesPerFrame = entry->bytesPerFrame = 0;
			} else if (stsdVersion == 1) {
				// Read QT version 1 fields. In version 0 these dont exist.
				entry->samplesPerFrame = _fd->readUint32BE();
				_fd->readUint32BE(); // bytes per packet
				entry->bytesPerFrame = _fd->readUint32BE();
				_fd->readUint32BE(); // bytes per sample
			} else {
				warning("Unsupported QuickTime STSD audio version %d", stsdVersion);
				return 1;
			}

			// Version 0 videos (such as the Riven ones) don't have this set,
			// but we need it later on. Add it in here.
			if (format == MKID_BE('ima4')) {
				entry->samplesPerFrame = 64;
				entry->bytesPerFrame = 34 * entry->channels;
			}

			if (entry->sampleRate == 0 && st->time_scale > 1)
				entry->sampleRate = st->time_scale;
		} else {
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

int QuickTimeDecoder::readSTSC(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->sample_to_chunk_sz = _fd->readUint32BE();
	st->sample_to_chunk = new MOVstsc[st->sample_to_chunk_sz];

	if (!st->sample_to_chunk)
		return -1;

	for (uint32 i = 0; i < st->sample_to_chunk_sz; i++) {
		st->sample_to_chunk[i].first = _fd->readUint32BE() - 1;
		st->sample_to_chunk[i].count = _fd->readUint32BE();
		st->sample_to_chunk[i].id = _fd->readUint32BE();
		//warning("Sample to Chunk[%d]: First = %d, Count = %d", i, st->sample_to_chunk[i].first, st->sample_to_chunk[i].count);
	}

	return 0;
}

int QuickTimeDecoder::readSTSS(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->keyframe_count = _fd->readUint32BE();
	st->keyframes = new uint32[st->keyframe_count];

	if (!st->keyframes)
		return -1;

	for (uint32 i = 0; i < st->keyframe_count; i++)
		st->keyframes[i] = _fd->readUint32BE() - 1; // Adjust here, the frames are based on 1

	return 0;
}

int QuickTimeDecoder::readSTSZ(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->sample_size = _fd->readUint32BE();
	st->sample_count = _fd->readUint32BE();

	if (st->sample_size)
		return 0; // there isn't any table following

	st->sample_sizes = new uint32[st->sample_count];

	if (!st->sample_sizes)
		return -1;

	for (uint32 i = 0; i < st->sample_count; i++)
		st->sample_sizes[i] = _fd->readUint32BE();

	return 0;
}

static uint32 ff_gcd(uint32 a, uint32 b) {
	return b ? ff_gcd(b, a % b) : a;
}

int QuickTimeDecoder::readSTTS(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];
	uint32 duration = 0;
	uint32 total_sample_count = 0;

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->stts_count = _fd->readUint32BE();
	st->stts_data = new MOVstts[st->stts_count];
	st->time_rate = 0;

	for (int32 i = 0; i < st->stts_count; i++) {
		int sample_duration;
		int sample_count;

		sample_count = _fd->readUint32BE();
		sample_duration = _fd->readUint32BE();
		st->stts_data[i].count = sample_count;
		st->stts_data[i].duration = sample_duration;

		st->time_rate = ff_gcd(st->time_rate, sample_duration);

		duration += sample_duration * sample_count;
		total_sample_count += sample_count;
	}

	st->nb_frames = total_sample_count;

	if (duration)
		st->duration = duration;

	return 0;
}

int QuickTimeDecoder::readSTCO(MOVatom atom) {
	MOVStreamContext *st = _streams[_numStreams - 1];

	_fd->readByte(); // version
	_fd->readByte(); _fd->readByte(); _fd->readByte(); // flags

	st->chunk_count = _fd->readUint32BE();
	st->chunk_offsets = new uint32[st->chunk_count];

	if (!st->chunk_offsets)
		return -1;

	for (uint32 i = 0; i < st->chunk_count; i++)
		st->chunk_offsets[i] = _fd->readUint32BE();

	return 0;
}


Common::SeekableReadStream *QuickTimeDecoder::getNextFramePacket(uint32 &descId) {
	if (_videoStreamIndex < 0)
		return NULL;

	// First, we have to track down which chunk holds the sample and which sample in the chunk contains the frame we are looking for.
	int32 totalSampleCount = 0;
	int32 sampleInChunk = 0;
	int32 actualChunk = -1;

	for (uint32 i = 0; i < _streams[_videoStreamIndex]->chunk_count; i++) {
		int32 sampleToChunkIndex = -1;

		for (uint32 j = 0; j < _streams[_videoStreamIndex]->sample_to_chunk_sz; j++)
			if (i >= _streams[_videoStreamIndex]->sample_to_chunk[j].first)
				sampleToChunkIndex = j;

		if (sampleToChunkIndex < 0)
			error("This chunk (%d) is imaginary", sampleToChunkIndex);

		totalSampleCount += _streams[_videoStreamIndex]->sample_to_chunk[sampleToChunkIndex].count;

		if (totalSampleCount > _curFrame) {
			actualChunk = i;
			descId = _streams[_videoStreamIndex]->sample_to_chunk[sampleToChunkIndex].id;
			sampleInChunk = _streams[_videoStreamIndex]->sample_to_chunk[sampleToChunkIndex].count - totalSampleCount + _curFrame;
			break;
		}
	}

	if (actualChunk < 0) {
		warning("Could not find data for frame %d", _curFrame);
		return NULL;
	}

	// Next seek to that frame
	_fd->seek(_streams[_videoStreamIndex]->chunk_offsets[actualChunk]);

	// Then, if the chunk holds more than one frame, seek to where the frame we want is located
	for (int32 i = _curFrame - sampleInChunk; i < _curFrame; i++) {
		if (_streams[_videoStreamIndex]->sample_size != 0)
			_fd->skip(_streams[_videoStreamIndex]->sample_size);
		else
			_fd->skip(_streams[_videoStreamIndex]->sample_sizes[i]);
	}

	// Finally, read in the raw data for the frame
	//printf ("Frame Data[%d]: Offset = %d, Size = %d\n", getCurFrame(), _fd->pos(), _streams[_videoStreamIndex]->sample_sizes[getCurFrame()]);

	if (_streams[_videoStreamIndex]->sample_size != 0)
		return _fd->readStream(_streams[_videoStreamIndex]->sample_size);

	return _fd->readStream(_streams[_videoStreamIndex]->sample_sizes[_curFrame]);
}

bool QuickTimeDecoder::checkAudioCodecSupport(uint32 tag) {
	// Check if the codec is a supported codec
	if (tag == MKID_BE('twos') || tag == MKID_BE('raw ') || tag == MKID_BE('ima4'))
		return true;

	warning("Audio Codec Not Supported: \'%s\'", tag2str(tag));

	return false;
}

Sound::AudioStream *QuickTimeDecoder::createAudioStream(Common::SeekableReadStream *stream) {
	if (!stream || _audioStreamIndex < 0)
		return NULL;

	STSDEntry *entry = &_streams[_audioStreamIndex]->stsdEntries[0];

	if (entry->codecTag == MKID_BE('twos') || entry->codecTag == MKID_BE('raw ')) {
		// Standard PCM
		uint16 flags = 0;
		if (entry->codecTag == MKID_BE('raw '))
			flags |= Sound::FLAG_UNSIGNED;
		if (entry->channels == 2)
			flags |= Sound::FLAG_STEREO;
		if (entry->bitsPerSample == 16)
			flags |= Sound::FLAG_16BITS;

		return Sound::makePCMStream(stream, entry->sampleRate, flags);
	} else if (entry->codecTag == MKID_BE('ima4')) {
		// QuickTime IMA ADPCM
		return Sound::makeADPCMStream(stream, true, stream->size(), Sound::kADPCMApple, entry->sampleRate, entry->channels, 34);
	} else if (entry->codecTag == MKID_BE('mp4a')) {
		// TODO: MPEG-4 Audio
		throw Common::Exception("Unhandled MPEG-4 audio");
	}

	throw Common::Exception("Unsupported audio codec");

	return NULL;
}

uint32 QuickTimeDecoder::getAudioChunkSampleCount(uint chunk) {
	if (_audioStreamIndex < 0)
		return 0;

	uint32 sampleCount = 0;

	for (uint32 j = 0; j < _streams[_audioStreamIndex]->sample_to_chunk_sz; j++)
		if (chunk >= _streams[_audioStreamIndex]->sample_to_chunk[j].first)
			sampleCount = _streams[_audioStreamIndex]->sample_to_chunk[j].count;

	return sampleCount;
}

void QuickTimeDecoder::readNextAudioChunk() {
	STSDEntry *entry = &_streams[_audioStreamIndex]->stsdEntries[0];
	Common::MemoryWriteStreamDynamic *wStream = new Common::MemoryWriteStreamDynamic();

	_fd->seek(_streams[_audioStreamIndex]->chunk_offsets[_curAudioChunk]);

	// First, we have to get the sample count
	uint32 sampleCount = getAudioChunkSampleCount(_curAudioChunk);
	assert(sampleCount);

	// Then calculate the right sizes
	while (sampleCount > 0) {
		uint32 samples = 0, size = 0;

		if (entry->samplesPerFrame >= 160) {
			samples = entry->samplesPerFrame;
			size = entry->bytesPerFrame;
		} else if (entry->samplesPerFrame > 1) {
			samples = MIN<uint32>((1024 / entry->samplesPerFrame) * entry->samplesPerFrame, sampleCount);
			size = (samples / entry->samplesPerFrame) * entry->bytesPerFrame;
		} else {
			samples = MIN<uint32>(1024, sampleCount);
			size = samples * _streams[_audioStreamIndex]->sample_size;
		}

		// Now, we read in the data for this data and output it
		byte *data = (byte *)malloc(size);
		_fd->read(data, size);
		wStream->write(data, size);
		free(data);
		sampleCount -= samples;
	}

	// Now queue the buffer
	queueSound(createAudioStream(new Common::MemoryReadStream(wStream->getData(), wStream->size(), true)));
	delete wStream;

	_curAudioChunk++;
}

void QuickTimeDecoder::updateAudioBuffer() {
	if (_audioStreamIndex < 0)
		return;

	uint32 numberOfChunksNeeded = 0;

	if (_curFrame == (int32)_streams[_videoStreamIndex]->nb_frames - 1) {
		// If we're on the last frame, make sure all audio remaining is buffered
		numberOfChunksNeeded = _streams[_audioStreamIndex]->chunk_count;
	} else {
		STSDEntry *entry = &_streams[_audioStreamIndex]->stsdEntries[0];

		// Calculate the amount of chunks we need in memory until the next frame
		uint32 timeToNextFrame = getTimeToNextFrame();
		uint32 timeFilled = 0;
		uint32 curAudioChunk = _curAudioChunk - getNumQueuedStreams();

		for (; timeFilled < timeToNextFrame && curAudioChunk < _streams[_audioStreamIndex]->chunk_count; numberOfChunksNeeded++, curAudioChunk++) {
			uint32 sampleCount = getAudioChunkSampleCount(curAudioChunk);
			assert(sampleCount);

			timeFilled += sampleCount * 1000 / entry->sampleRate;
		}

		// Add a couple extra to ensure we don't underrun
		numberOfChunksNeeded += 3;
	}

	// Keep three streams in buffer so that if/when the first two end, it goes right into the next
	while (getNumQueuedStreams() < numberOfChunksNeeded && _curAudioChunk < _streams[_audioStreamIndex]->chunk_count)
		readNextAudioChunk();
}

QuickTimeDecoder::STSDEntry::STSDEntry() {
	codecTag = 0;
	bitsPerSample = 0;
	memset(codecName, 0, 32);
	colorTableId = 0;
	palette = 0;
	videoCodec = 0;
	channels = 0;
	sampleRate = 0;
	samplesPerFrame = 0;
	bytesPerFrame = 0;
}

QuickTimeDecoder::STSDEntry::~STSDEntry() {
	delete[] palette;
	delete videoCodec;
}

QuickTimeDecoder::MOVStreamContext::MOVStreamContext() {
	memset(this, 0, sizeof(MOVStreamContext));
}

QuickTimeDecoder::MOVStreamContext::~MOVStreamContext() {
	delete[] chunk_offsets;
	delete[] stts_data;
	delete[] sample_to_chunk;
	delete[] sample_sizes;
	delete[] keyframes;
	delete[] stsdEntries;
	delete extradata;
}

} // End of namespace Video

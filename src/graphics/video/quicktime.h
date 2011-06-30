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

/** @file graphics/video/quicktime.h
 *  Decoding Apple QuickTime videos.
 */

//
// Heavily based on ScummVM code which is heavily based on ffmpeg code.
//
// Copyright (c) 2001 Fabrice Bellard.
// First version by Francois Revol revol@free.fr
// Seek function by Gael Chardon gael.dev@4now.net
//

#ifndef GRAPHICS_VIDEO_QUICKTIME_H
#define GRAPHICS_VIDEO_QUICKTIME_H

#include "graphics/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

// This is the best class evar!!!!!
// (Dummied until we actually support video codecs)
class Codec {
public:
	Codec() {}
};

/**
 * Decoder for QuickTime videos.
 */
class QuickTimeDecoder : public VideoDecoder {
public:
	QuickTimeDecoder(Common::SeekableReadStream *stream);
	~QuickTimeDecoder();

	bool hasTime() const;

protected:
	void processData();

private:
	// This is the file handle from which data is read from. It can be the actual file handle or a decompressed stream.
	Common::SeekableReadStream *_fd;

	struct MOVatom {
		uint32 type;
		uint32 offset;
		uint32 size;
	};

	struct ParseTable {
		int (QuickTimeDecoder::*func)(MOVatom atom);
		uint32 type;
	};

	struct MOVstts {
		int count;
		int duration;
	};

	struct MOVstsc {
		uint32 first;
		uint32 count;
		uint32 id;
	};

	struct STSDEntry {
		STSDEntry();
		~STSDEntry();

		uint32 codecTag;
		uint16 bitsPerSample;

		// Video
		char codecName[32];
		uint16 colorTableId;
		byte *palette;
		Codec *videoCodec;

		// Audio
		uint16 channels;
		uint32 sampleRate;
		uint32 samplesPerFrame;
		uint32 bytesPerFrame;
	};

	enum CodecType {
		CODEC_TYPE_MOV_OTHER,
		CODEC_TYPE_VIDEO,
		CODEC_TYPE_AUDIO
	};

	struct MOVStreamContext {
		MOVStreamContext();
		~MOVStreamContext();

		uint32 chunk_count;
		uint32 *chunk_offsets;
		int stts_count;
		MOVstts *stts_data;
		int edit_count; /* number of 'edit' (elst atom) */
		uint32 sample_to_chunk_sz;
		MOVstsc *sample_to_chunk;
		uint32 sample_size;
		uint32 sample_count;
		uint32 *sample_sizes;
		uint32 keyframe_count;
		uint32 *keyframes;
		int32 time_scale;
		int time_rate;

		uint16 width;
		uint16 height;
		CodecType codec_type;

		uint32 stsdEntryCount;
		STSDEntry *stsdEntries;

		Common::SeekableReadStream *extradata;

		uint32 nb_frames;
		uint32 duration;
		uint32 start_time;

		byte objectTypeMP4;
	};

	const ParseTable *_parseTable;
	bool _foundMOOV;
	uint32 _timeScale;
	uint32 _numStreams;
	MOVStreamContext *_streams[20];

	uint32 _startTime;
	int32 _curFrame;

	void initParseTable();
	Sound::AudioStream *createAudioStream(Common::SeekableReadStream *stream);
	bool checkAudioCodecSupport(uint32 tag);
	Common::SeekableReadStream *getNextFramePacket(uint32 &descId);
	uint32 getFrameDuration();
	void init();

	void updateAudioBuffer();
	void readNextAudioChunk();
	uint32 getAudioChunkSampleCount(uint chunk);
	int8 _audioStreamIndex;
	uint _curAudioChunk;

	Codec *createCodec(uint32 codecTag, byte bitsPerPixel);
	Codec *findDefaultVideoCodec() const;
	uint32 _nextFrameStartTime;
	int8 _videoStreamIndex;

	uint32 getElapsedTime() const;
	uint32 getTimeToNextFrame() const;

	int readDefault(MOVatom atom);
	int readLeaf(MOVatom atom);
	int readELST(MOVatom atom);
	int readHDLR(MOVatom atom);
	int readMDHD(MOVatom atom);
	int readMOOV(MOVatom atom);
	int readMVHD(MOVatom atom);
	int readTRAK(MOVatom atom);
	int readSTCO(MOVatom atom);
	int readSTSC(MOVatom atom);
	int readSTSD(MOVatom atom);
	int readSTSS(MOVatom atom);
	int readSTSZ(MOVatom atom);
	int readSTTS(MOVatom atom);
	int readESDS(MOVatom atom);
};

} // End of namespace Video

#endif

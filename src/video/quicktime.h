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
 *  Decoding Apple QuickTime videos.
 */

//
// Heavily based on ScummVM code which is heavily based on ffmpeg code.
//
// Copyright (c) 2001 Fabrice Bellard.
// First version by Francois Revol revol@free.fr
// Seek function by Gael Chardon gael.dev@4now.net
//

#ifndef VIDEO_QUICKTIME_H
#define VIDEO_QUICKTIME_H

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {
	class Surface;
}

namespace Sound {
	class Codec;
}

namespace Video {

class Codec;

/**
 * Decoder for QuickTime videos.
 */
class QuickTimeDecoder : public VideoDecoder {
public:
	QuickTimeDecoder(Common::SeekableReadStream *stream);
	~QuickTimeDecoder();

	uint32 getTimeToNextFrame() const;

protected:
	void startVideo();
	void processData();

private:
	// This is the file handle from which data is read from. It can be the actual file handle or a decompressed stream.
	Common::SeekableReadStream *_fd;

	struct Atom {
		uint32 type;
		uint32 offset;
		uint32 size;
	};

	struct ParseTable {
		int (QuickTimeDecoder::*func)(Atom atom);
		uint32 type;
	};

	struct TimeToSampleEntry {
		int count;
		int duration;
	};

	struct SampleToChunkEntry {
		uint32 first;
		uint32 count;
		uint32 id;
	};

	struct Track;

	class SampleDesc {
	public:
		SampleDesc(Track *parentTrack, uint32 codecTag);
		virtual ~SampleDesc() {}

		uint32 getCodecTag() const { return _codecTag; }

	protected:
		Track *_parentTrack;
		uint32 _codecTag;
	};

	class AudioSampleDesc : public SampleDesc {
	public:
		AudioSampleDesc(Track *parentTrack, uint32 codecTag);
		~AudioSampleDesc();

		bool isAudioCodecSupported() const;
		uint32 getAudioChunkSampleCount(uint chunk) const;
		Sound::AudioStream *createAudioStream(Common::SeekableReadStream *stream) const;
		void initCodec();

		// TODO: Make private in the long run
		uint16 _bitsPerSample;
		uint16 _channels;
		uint32 _sampleRate;
		uint32 _samplesPerFrame;
		uint32 _bytesPerFrame;

		Sound::Codec *_codec;
	};

	class VideoSampleDesc : public SampleDesc {
	public:
		VideoSampleDesc(Track *parentTrack, uint32 codecTag);
		~VideoSampleDesc();

		void initCodec(Graphics::Surface &surface);

		// TODO: Make private in the long run
		uint16 _bitsPerSample;
		char _codecName[32];
		uint16 _colorTableId;
		byte *_palette;
		Codec *_videoCodec;
	};

	enum CodecType {
		CODEC_TYPE_MOV_OTHER,
		CODEC_TYPE_VIDEO,
		CODEC_TYPE_AUDIO
	};

	struct Track {
		Track();
		~Track();

		uint32 chunkCount;
		uint32 *chunkOffsets;
		int timeToSampleCount;
		TimeToSampleEntry *timeToSample;
		uint32 sampleToChunkCount;
		SampleToChunkEntry *sampleToChunk;
		uint32 sampleSize;
		uint32 sampleCount;
		uint32 *sampleSizes;
		uint32 keyframeCount;
		uint32 *keyframes;
		int32 timeScale;

		uint16 width;
		uint16 height;
		CodecType codecType;

		std::vector<SampleDesc *> sampleDescs;

		Common::SeekableReadStream *extraData;

		uint32 frameCount;
		uint32 duration;
		uint32 startTime;

		byte objectTypeMP4;
	};

	SampleDesc *readSampleDesc(Track *track, uint32 format);

	const ParseTable *_parseTable;
	bool _foundMOOV;
	uint32 _timeScale;
	std::vector<Track *> _tracks;

	int32 _curFrame;
	uint32 _startTime;

	void initParseTable();

	bool isOldDemuxing() const;
	void queueNextAudioChunk();
	void updateAudioBuffer();
	int _audioTrackIndex;
	uint _curAudioChunk;

	Codec *findDefaultVideoCodec() const;
	uint32 _nextFrameStartTime;
	int _videoTrackIndex;
	Common::SeekableReadStream *getNextFramePacket(uint32 &descId);
	uint32 getFrameDuration();

	uint32 getElapsedTime() const;

	int readDefault(Atom atom);
	int readLeaf(Atom atom);
	int readELST(Atom atom);
	int readHDLR(Atom atom);
	int readMDHD(Atom atom);
	int readMOOV(Atom atom);
	int readMVHD(Atom atom);
	int readTRAK(Atom atom);
	int readSTCO(Atom atom);
	int readSTSC(Atom atom);
	int readSTSD(Atom atom);
	int readSTSS(Atom atom);
	int readSTSZ(Atom atom);
	int readSTTS(Atom atom);
	int readESDS(Atom atom);

	void load();
	void clear();
};

} // End of namespace Video

#endif // VIDEO_QUICKTIME_H

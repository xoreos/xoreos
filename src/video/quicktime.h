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

/* Based on the Quicktime implementation in FFmpeg (<https://ffmpeg.org/)>,
 * which is released under the terms of version 2 or later of the GNU
 * Lesser General Public License.
 *
 * The original copyright note in libavformat/mov.c reads as follows:
 *
 * MOV demuxer
 * Copyright (c) 2001 Fabrice Bellard
 * Copyright (c) 2009 Baptiste Coudurier <baptiste dot coudurier at gmail dot com>
 *
 * first version by Francois Revol <revol@free.fr>
 * seek function by Gael Chardon <gael.dev@4now.net>
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

#ifndef VIDEO_QUICKTIME_H
#define VIDEO_QUICKTIME_H

#include <vector>

#include "src/common/types.h"
#include "src/common/ptrvector.h"
#include "src/common/scopedptr.h"
#include "src/common/timestamp.h"

#include "src/sound/audiostream.h"

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {
	class Surface;
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

	uint32 getNextFrameStartTime() const;

protected:
	void startVideo();
	void processData();

private:
	// This is the file handle from which data is read from. It can be the actual file handle or a decompressed stream.
	Common::ScopedPtr<Common::SeekableReadStream> _fd;

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

	struct QuickTimeTrack;

	class SampleDesc {
	public:
		SampleDesc(QuickTimeTrack *parentTrack, uint32 codecTag);
		virtual ~SampleDesc() {}

		uint32 getCodecTag() const { return _codecTag; }

		Common::ScopedPtr<Common::SeekableReadStream> _extraData;
		byte _objectTypeMP4;

	protected:
		QuickTimeTrack *_parentTrack;
		uint32 _codecTag;
	};

	class AudioSampleDesc : public SampleDesc {
	public:
		AudioSampleDesc(QuickTimeTrack *parentTrack, uint32 codecTag);

		bool isAudioCodecSupported() const;
		Sound::PacketizedAudioStream *createAudioStream() const;

		// TODO: Make private in the long run
		uint16 _bitsPerSample;
		uint16 _channels;
		uint32 _sampleRate;
		uint32 _samplesPerFrame;
		uint32 _bytesPerFrame;
	};

	class VideoSampleDesc : public SampleDesc {
	public:
		VideoSampleDesc(QuickTimeTrack *parentTrack, uint32 codecTag);
		~VideoSampleDesc();

		void initCodec();

		// TODO: Make private in the long run
		uint16 _bitsPerSample;
		char _codecName[32];
		uint16 _colorTableId;
		Common::ScopedArray<byte> _palette;
		Common::ScopedPtr<Codec> _videoCodec;
	};

	enum CodecType {
		CODEC_TYPE_MOV_OTHER,
		CODEC_TYPE_VIDEO,
		CODEC_TYPE_AUDIO
	};

	struct QuickTimeTrack {
		QuickTimeTrack();

		uint32 chunkCount;
		Common::ScopedArray<uint32> chunkOffsets;
		int timeToSampleCount;
		Common::ScopedArray<TimeToSampleEntry> timeToSample;
		uint32 sampleToChunkCount;
		Common::ScopedArray<SampleToChunkEntry> sampleToChunk;
		uint32 sampleSize;
		uint32 sampleCount;
		Common::ScopedArray<uint32> sampleSizes;
		uint32 keyframeCount;
		Common::ScopedArray<uint32> keyframes;
		int32 timeScale;

		uint16 width;
		uint16 height;
		CodecType codecType;

		Common::PtrVector<SampleDesc> sampleDescs;

		uint32 frameCount;
		uint32 duration;
		uint32 startTime;
	};

	class QuickTimeAudioTrack : public Sound::AudioStream {
	public:
		QuickTimeAudioTrack(QuickTimeDecoder *decoder, QuickTimeTrack *parentTrack);

		// AudioStream API
		size_t readBuffer(int16 *data, const size_t numSamples) { return _stream->readBuffer(data, numSamples); }
		int getChannels() const { return _stream->getChannels(); }
		int getRate() const { return _stream->getRate(); }
		bool endOfData() const { return _stream->endOfData(); }
		bool endOfStream() const { return _stream->endOfStream(); }

		// Queue *at least* "length" audio
		// If length is zero, it queues the next logical block of audio whether
		// that be a whole edit or just one chunk within an edit
		void queueAudio(const Common::Timestamp &length = Common::Timestamp());

	private:
		QuickTimeDecoder *_decoder;
		QuickTimeTrack *_parentTrack;
		uint _curChunk;
		Common::Timestamp _curMediaPos;
		uint32 _samplesQueued;
		bool _skipAACPrimer;
		Common::ScopedPtr<Sound::PacketizedAudioStream> _stream;

		Common::SeekableReadStream *readAudioChunk(uint chunk);
		bool isOldDemuxing() const;
		uint32 getAudioChunkSampleCount(uint chunk) const;
		Common::Timestamp getChunkLength(uint chunk, bool skipAACPrimer = false) const;
		uint32 getAACSampleTime(uint32 totalSampleCount, bool skipAACPrimer = false) const;
		bool allAudioQueued() const { return _curChunk >= _parentTrack->chunkCount; }
	};

	// The AudioTrackHandler is currently just a wrapper around some
	// QuickTimeDecoder functions.
	class AudioTrackHandler : public AudioTrack {
	public:
		AudioTrackHandler(QuickTimeDecoder *decoder, QuickTimeAudioTrack *audioTrack);

		bool canBufferData() const;

		void updateBuffer(const Common::Timestamp &endTime);

	protected:
		Sound::AudioStream *getAudioStream() const;

	private:
		QuickTimeDecoder *_decoder;
		QuickTimeAudioTrack *_audioTrack;
	};

	SampleDesc *readSampleDesc(QuickTimeTrack *track, uint32 format);

	const ParseTable *_parseTable;
	bool _foundMOOV;
	uint32 _timeScale;
	Common::PtrVector<QuickTimeTrack> _tracks;

	int32 _curFrame;
	uint32 _startTime;

	void initParseTable();

	void checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime);
	Common::PtrVector<QuickTimeAudioTrack> _audioTracks;

	Codec *findDefaultVideoCodec() const;
	uint32 _nextFrameStartTime;
	int _videoTrackIndex;
	Common::SeekableReadStream *getNextFramePacket(uint32 &descId);
	uint32 getFrameDuration();

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
};

} // End of namespace Video

#endif // VIDEO_QUICKTIME_H

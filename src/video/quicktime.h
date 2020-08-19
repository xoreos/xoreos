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
#include <memory>

#include "src/common/types.h"
#include "src/common/ptrvector.h"
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

protected:
	void decodeNextTrackFrame(VideoTrack &track);

private:
	// This is the file handle from which data is read from. It can be the actual file handle or a decompressed stream.
	std::unique_ptr<Common::SeekableReadStream> _fd;

	struct Atom {
		uint32_t type;
		uint32_t offset;
		uint32_t size;
	};

	struct ParseTable {
		int (QuickTimeDecoder::*func)(Atom atom);
		uint32_t type;
	};

	struct TimeToSampleEntry {
		int count;
		int duration;
	};

	struct SampleToChunkEntry {
		uint32_t first;
		uint32_t count;
		uint32_t id;
	};

	struct QuickTimeTrack;

	class SampleDesc {
	public:
		SampleDesc(QuickTimeTrack *parentTrack, uint32_t codecTag);
		virtual ~SampleDesc() {}

		uint32_t getCodecTag() const { return _codecTag; }

		std::unique_ptr<Common::SeekableReadStream> _extraData;
		byte _objectTypeMP4;

	protected:
		QuickTimeTrack *_parentTrack;
		uint32_t _codecTag;
	};

	class AudioSampleDesc : public SampleDesc {
	public:
		AudioSampleDesc(QuickTimeTrack *parentTrack, uint32_t codecTag);

		bool isAudioCodecSupported() const;
		Sound::PacketizedAudioStream *createAudioStream() const;

		// TODO: Make private in the long run
		uint16_t _bitsPerSample;
		uint16_t _channels;
		uint32_t _sampleRate;
		uint32_t _samplesPerFrame;
		uint32_t _bytesPerFrame;
	};

	class VideoSampleDesc : public SampleDesc {
	public:
		VideoSampleDesc(QuickTimeTrack *parentTrack, uint32_t codecTag);
		~VideoSampleDesc();

		void initCodec();

		// TODO: Make private in the long run
		uint16_t _bitsPerSample;
		char _codecName[32];
		uint16_t _colorTableId;
		std::unique_ptr<byte[]> _palette;
		std::unique_ptr<Codec> _videoCodec;
	};

	enum CodecType {
		CODEC_TYPE_MOV_OTHER,
		CODEC_TYPE_VIDEO,
		CODEC_TYPE_AUDIO
	};

	struct QuickTimeTrack {
		QuickTimeTrack();

		uint32_t chunkCount;
		std::unique_ptr<uint32_t[]> chunkOffsets;
		int timeToSampleCount;
		std::unique_ptr<TimeToSampleEntry[]> timeToSample;
		uint32_t sampleToChunkCount;
		std::unique_ptr<SampleToChunkEntry[]> sampleToChunk;
		uint32_t sampleSize;
		uint32_t sampleCount;
		std::unique_ptr<uint32_t[]> sampleSizes;
		uint32_t keyframeCount;
		std::unique_ptr<uint32_t[]> keyframes;
		int32_t timeScale;

		uint16_t width;
		uint16_t height;
		CodecType codecType;

		Common::PtrVector<SampleDesc> sampleDescs;

		uint32_t frameCount;
		uint32_t duration;
		uint32_t startTime;
	};

	class QuickTimeAudioTrack : public Sound::AudioStream {
	public:
		QuickTimeAudioTrack(QuickTimeDecoder *decoder, QuickTimeTrack *parentTrack);

		// AudioStream API
		size_t readBuffer(int16_t *data, const size_t numSamples) { return _stream->readBuffer(data, numSamples); }
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
		uint32_t _samplesQueued;
		bool _skipAACPrimer;
		std::unique_ptr<Sound::PacketizedAudioStream> _stream;

		Common::SeekableReadStream *readAudioChunk(uint chunk);
		bool isOldDemuxing() const;
		uint32_t getAudioChunkSampleCount(uint chunk) const;
		Common::Timestamp getChunkLength(uint chunk, bool skipAACPrimer = false) const;
		uint32_t getAACSampleTime(uint32_t totalSampleCount, bool skipAACPrimer = false) const;
		bool allAudioQueued() const { return _curChunk >= _parentTrack->chunkCount; }
	};

	class VideoTrackHandler : public VideoTrack {
	public:
		VideoTrackHandler(QuickTimeDecoder *decoder, QuickTimeTrack *parent);

		Common::Timestamp getDuration() const;
		uint32_t getWidth() const;
		uint32_t getHeight() const;
		int getCurFrame() const { return _curFrame; }
		int getFrameCount() const;
		Common::Timestamp getNextFrameStartTime() const;

		bool decodeNextFrame(Graphics::Surface &surface);

	private:
		QuickTimeDecoder *_decoder;
		QuickTimeTrack *_parent;
		int32_t _curFrame;
		uint32_t _nextFrameStartTime;

		Common::SeekableReadStream *getNextFramePacket(uint32_t &descId);
		uint32_t getFrameDuration();
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

	SampleDesc *readSampleDesc(QuickTimeTrack *track, uint32_t format);

	const ParseTable *_parseTable;
	bool _foundMOOV;
	uint32_t _timeScale;
	Common::PtrVector<QuickTimeTrack> _tracks;

	void initParseTable();

	void checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime);
	Common::PtrVector<QuickTimeAudioTrack> _audioTracks;

	Codec *findDefaultVideoCodec() const;
	int _videoTrackIndex;

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

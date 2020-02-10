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

#ifndef VIDEO_MATROSKA_H
#define VIDEO_MATROSKA_H

#include <map>

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {
	class AudioStream;
	class PacketizedAudioStream;
}

namespace Video {

class Codec;

/** A decoder for Matroska videos. */
class Matroska : public VideoDecoder {
public:
	Matroska(Common::SeekableReadStream *fd);

protected:
	void decodeNextTrackFrame(VideoTrack &track);
	void checkAudioBuffer(AudioTrack &track, const Common::Timestamp &endTime);

private:
	/** A description of where a matroska cluster is. */
	struct Cluster {
		Cluster() : offset(0), size(0), timestamp(0) {}
		uint64 offset;
		uint64 size;
		uint64 timestamp;
	};

	/** Information on a packet. */
	struct Packet {
		Packet() : offset(0), size(0), timestamp(0) {}
		uint64 offset;
		uint64 size;
		uint64 timestamp;
	};

	/** The status of the track. */
	struct TrackStatus {
		TrackStatus() : curCluster(nullptr), curPos(0) {}
		Cluster *curCluster;
		uint64 curPos;
		Packet nextPacket;
	};

	/** A Matroska video track. */
	class MatroskaVideoTrack : public VideoTrack {
	public:
		MatroskaVideoTrack(uint64 trackNumber, uint32 width, uint32 height, uint64 defaultDuration);

		bool endOfTrack() const { return _finished; }

		uint32 getWidth() const { return _width; }
		uint32 getHeight() const { return _height; }
		int getCurFrame() const { return _curFrame; }
		Common::Timestamp getNextFrameStartTime() const;

		void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &frameData, uint64 timestamp);
		void initCodec(const std::string &codec, Common::SeekableReadStream *extraData);
		void finish() { _finished = true; }

		/** Get the track number. */
		uint64 getTrackNumber() const { return _trackNumber; }

	private:
		uint64 _trackNumber;
		uint32 _width;
		uint32 _height;
		uint64 _defaultDuration;
		int _curFrame;
		bool _finished;
		uint64 _timestamp;

		/** The video codec. */
		std::unique_ptr<Codec> _videoCodec;
	};

	/** A Matroska audio track. */
	class MatroskaAudioTrack : public AudioTrack {
	public:
		MatroskaAudioTrack(uint64 trackNumber, uint32 channelCount, uint32 sampleRate, const std::string &codec, Common::SeekableReadStream *extraData);

		/** Queue audio stream data belonging to this track. */
		void queueAudio(Common::SeekableReadStream *stream);

		/** Mark the stream as finished. */
		void finish();

		/** Get the track number. */
		uint64 getTrackNumber() const { return _trackNumber; }

		/** Get the last timestamp of data. */
		uint64 getLastTimestamp() const { return _lastTimestamp; }

		/** Set the last timestamp of data. */
		void setLastTimestamp(uint64 lastTimestamp) { _lastTimestamp = lastTimestamp; }

		// AudioTrack API
		bool canBufferData() const;
		Sound::AudioStream *getAudioStream() const;

	private:
		uint64 _trackNumber;
		uint64 _lastTimestamp;
		std::unique_ptr<Sound::PacketizedAudioStream> _audioStream;
		Sound::PacketizedAudioStream *createStream(uint32 channelCount, uint32 sampleRate, const std::string &codec, Common::SeekableReadStream *extraData) const;
	};

	std::unique_ptr<Common::SeekableReadStream> _fd;

	/** The set of clusters in the video. */
	std::vector<Cluster> _clusters;

	/** The map from the track number to the current status. */
	std::map<uint64, TrackStatus> _status;

	/** The map from the track number to each video track. */
	std::map<uint64, MatroskaVideoTrack*> _trackMap;

	/** The time code scale. */
	uint64 _timeCodeScale;


	/** Load a Matroska video. */
	void load();

	/** Get the next packet for the given track. */
	Common::SeekableReadStream *getNextPacket(uint64 trackNumber, uint64 &nextTimestamp);

	/** Find the next packet information for a track. */
	void findNextPacket(uint64 trackNumber, TrackStatus& status);
};

} // End of namespace Video

#endif // VIDEO_MATROSKA_H

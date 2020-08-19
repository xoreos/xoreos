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
		uint64_t offset;
		uint64_t size;
		uint64_t timestamp;
	};

	/** Information on a packet. */
	struct Packet {
		Packet() : offset(0), size(0), timestamp(0) {}
		uint64_t offset;
		uint64_t size;
		uint64_t timestamp;
	};

	/** The status of the track. */
	struct TrackStatus {
		TrackStatus() : curCluster(nullptr), curPos(0) {}
		Cluster *curCluster;
		uint64_t curPos;
		Packet nextPacket;
	};

	/** A Matroska video track. */
	class MatroskaVideoTrack : public VideoTrack {
	public:
		MatroskaVideoTrack(uint64_t trackNumber, uint32_t width, uint32_t height, uint64_t defaultDuration);

		bool endOfTrack() const { return _finished; }

		uint32_t getWidth() const { return _width; }
		uint32_t getHeight() const { return _height; }
		int getCurFrame() const { return _curFrame; }
		Common::Timestamp getNextFrameStartTime() const;

		void decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &frameData, uint64_t timestamp);
		void initCodec(const std::string &codec, Common::SeekableReadStream *extraData);
		void finish() { _finished = true; }

		/** Get the track number. */
		uint64_t getTrackNumber() const { return _trackNumber; }

	private:
		uint64_t _trackNumber;
		uint32_t _width;
		uint32_t _height;
		uint64_t _defaultDuration;
		int _curFrame;
		bool _finished;
		uint64_t _timestamp;

		/** The video codec. */
		std::unique_ptr<Codec> _videoCodec;
	};

	/** A Matroska audio track. */
	class MatroskaAudioTrack : public AudioTrack {
	public:
		MatroskaAudioTrack(uint64_t trackNumber, uint32_t channelCount, uint32_t sampleRate, const std::string &codec, Common::SeekableReadStream *extraData);

		/** Queue audio stream data belonging to this track. */
		void queueAudio(Common::SeekableReadStream *stream);

		/** Mark the stream as finished. */
		void finish();

		/** Get the track number. */
		uint64_t getTrackNumber() const { return _trackNumber; }

		/** Get the last timestamp of data. */
		uint64_t getLastTimestamp() const { return _lastTimestamp; }

		/** Set the last timestamp of data. */
		void setLastTimestamp(uint64_t lastTimestamp) { _lastTimestamp = lastTimestamp; }

		// AudioTrack API
		bool canBufferData() const;
		Sound::AudioStream *getAudioStream() const;

	private:
		uint64_t _trackNumber;
		uint64_t _lastTimestamp;
		std::unique_ptr<Sound::PacketizedAudioStream> _audioStream;
		Sound::PacketizedAudioStream *createStream(uint32_t channelCount, uint32_t sampleRate, const std::string &codec, Common::SeekableReadStream *extraData) const;
	};

	std::unique_ptr<Common::SeekableReadStream> _fd;

	/** The set of clusters in the video. */
	std::vector<Cluster> _clusters;

	/** The map from the track number to the current status. */
	std::map<uint64_t, TrackStatus> _status;

	/** The map from the track number to each video track. */
	std::map<uint64_t, MatroskaVideoTrack*> _trackMap;

	/** The time code scale. */
	uint64_t _timeCodeScale;


	/** Load a Matroska video. */
	void load();

	/** Get the next packet for the given track. */
	Common::SeekableReadStream *getNextPacket(uint64_t trackNumber, uint64_t &nextTimestamp);

	/** Find the next packet information for a track. */
	void findNextPacket(uint64_t trackNumber, TrackStatus& status);
};

} // End of namespace Video

#endif // VIDEO_MATROSKA_H

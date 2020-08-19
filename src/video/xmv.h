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
 *  Decoding Microsoft Xbox XMV videos.
 */

#ifndef VIDEO_XMV_H
#define VIDEO_XMV_H

#include <vector>
#include <memory>

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

/** A decoder for Microsoft Xbox XMV videos. */
class XboxMediaVideo : public VideoDecoder {
public:
	XboxMediaVideo(Common::SeekableReadStream *xmv);
	~XboxMediaVideo();

protected:
	void decodeNextTrackFrame(VideoTrack &track);

private:
	/** An audio track's information. */
	struct AudioInfo {
		uint16_t compression;   ///< The compression method.
		uint16_t channels;      ///< The number of channels.
		uint32_t rate;          ///< The sampling rate.
		uint16_t bitsPerSample; ///< The number of bits per encoded sample.
		uint16_t flags;         ///< Flags.
	};

	class XMVVideoTrack : public VideoTrack {
	public:
		XMVVideoTrack(uint32_t width, uint32_t height, uint32_t &timestamp);

		bool endOfTrack() const { return _finished; }

		uint32_t getWidth() const { return _width; }
		uint32_t getHeight() const { return _height; }
		int getCurFrame() const { return _curFrame; }
		Common::Timestamp getNextFrameStartTime() const;

		bool decodeFrame(Graphics::Surface &surface, Common::SeekableReadStream &frameData);
		void initCodec(Common::SeekableReadStream &extraData);
		void finish() { _finished = true; }

	private:
		uint32_t _width;
		uint32_t _height;
		uint32_t &_timestamp;
		int _curFrame;
		bool _finished;

		/** The video codec. */
		std::unique_ptr<Codec> _videoCodec;
	};

	/** An XMV audio track. */
	class XMVAudioTrack : public AudioTrack {
	public:
		XMVAudioTrack(const AudioInfo &info);

		/** Queue audio stream data belonging to this track. */
		void queueAudio(Common::SeekableReadStream *stream);

		/** Mark the stream as finished. */
		void finish();

		// AudioTrack API
		bool canBufferData() const;
		Sound::AudioStream *getAudioStream() const;

	private:
		AudioInfo _info;
		std::unique_ptr<Sound::PacketizedAudioStream> _audioStream;
		Sound::PacketizedAudioStream *createStream() const;
	};

	/** An XMV 5.1 audio track. */
	class XMVAudioTrack51 : public AudioTrack {
	public:
		XMVAudioTrack51(XMVAudioTrack *info1, XMVAudioTrack *info2, XMVAudioTrack *info3);

		// AudioTrack API
		bool canBufferData() const;

	protected:
		// AudioTrack API
		Sound::AudioStream *getAudioStream() const;

	private:
		std::unique_ptr<XMVAudioTrack> _realTracks[3];
		std::unique_ptr<Sound::AudioStream> _interleaved;
	};

	/** A video packet. */
	struct PacketVideo {
		uint32_t dataSize;   ///< The video data size.
		uint32_t dataOffset; ///< The video data offset within the XMV stream.

		uint32_t frameCount; ///< Number of frames left in this packet.

		/** The size of the current frame. */
		uint32_t currentFrameSize; ///< The size of the current frame.
		/** The timestamp of when the current frame should be shown. */
		uint32_t currentFrameTimestamp;

		/** The timestamp of when the current last frame was shown. */
		uint32_t lastFrameTime;
	};

	/** An audio packet. */
	struct PacketAudio {
		uint32_t dataSize;   ///< The audio data size.
		uint32_t dataOffset; ///< The audio data offset within the XMV stream.

		XMVAudioTrack *track; ///< The audio track mapping.
		AudioInfo *info; ///< The audio info related to this packet.

		bool newSlice; ///< Is a new slice that needs to be queue available?
	};

	/** An XMV packet. */
	struct Packet {
		size_t thisPacketSize; ///< The current packet's size.
		size_t nextPacketSize; ///< The next packet's size.

		size_t thisPacketOffset; ///< The current packet's offset within the XMV stream.
		size_t nextPacketOffset; ///< The next packet's offset within the XMV stream.

		/** The video part of the packet. */
		PacketVideo video;

		/** The audio part of the packet. */
		std::vector<PacketAudio> audio;
	};

	std::unique_ptr<Common::SeekableReadStream> _xmv;

	/** The current video track. */
	XMVVideoTrack *_videoTrack;

	/** The audio track count. */
	uint32_t _audioTrackCount;

	/** The current packet. */
	Packet _curPacket;


	/** Load an XMV file. */
	void load();

	/** Fetch the next packet. */
	void fetchNextPacket(Packet &packet);

	/** Process a packet's header. */
	void processPacketHeader(Packet &packet);

	/** Queue the data from this audio packet. */
	void queueNewAudio(PacketAudio &audioPacket);
	/** Queue the data from all audio packets in this packet. */
	void queueNewAudio(Packet &packet);

	/** Process the next frame. */
	void processNextFrame(PacketVideo &videoPacket);
};

} // End of namespace Video

#endif // VIDEO_XMV_H

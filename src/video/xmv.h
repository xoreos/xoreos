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

/** @file video/xmv.h
 *  Decoding Microsoft Xbox XMV videos.
 */

#ifndef VIDEO_XMV_H
#define VIDEO_XMV_H

#include "video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {
	class AudioStream;
}

namespace Video {

class Codec;

/** A decoder for Microsoft Xbox XMV videos. */
class XboxMediaVideo : public VideoDecoder {
public:
	XboxMediaVideo(Common::SeekableReadStream *xmv);
	~XboxMediaVideo();

	uint32 getTimeToNextFrame() const;

protected:
	void startVideo();
	void processData();

private:
	/** An audio track. */
	struct AudioTrack {
		uint16 compression;   ///< The compression method.
		uint16 channels;      ///< The number of channels.
		uint32 rate;          ///< The sampling rate.
		uint16 bitsPerSample; ///< The number of bits per encoded sample.
		uint16 flags;         ///< Flags.

		bool supported; ///< Are these audio track parameters supported by xoreos?
		bool enabled;   ///< Should this audio track be played?

		byte audioStreamFlags; ///< Flags for the audio stream creation.
	};

	/** A video packet. */
	struct PacketVideo {
		uint32 dataSize;   ///< The video data size.
		uint32 dataOffset; ///< The video data offset within the XMV stream.

		uint32 frameCount; ///< Number of frames left in this packet.

		/** The size of the current frame. */
		uint32 currentFrameSize; ///< The size of the current frame.
		/** The timestamp of when the current frame should be shown. */
		uint32 currentFrameTimestamp;

		/** The timestamp of when the current last frame was shown. */
		uint32 lastFrameTime;
	};

	/** An audio packet. */
	struct PacketAudio {
		uint32 dataSize;   ///< The audio data size.
		uint32 dataOffset; ///< The audio data offset within the XMV stream.

		AudioTrack *track; ///< The track this packet belongs to.

		bool newSlice; ///< Is a new slice that needs to be queue available?
	};

	/** An XMV packet. */
	struct Packet {
		uint32 thisPacketSize; ///< The current packet's size.
		uint32 nextPacketSize; ///< The next packet's size.

		uint32 thisPacketOffset; ///< The current packet's offset within the XMV stream.
		uint32 nextPacketOffset; ///< The next packet's offset within the XMV stream.

		/** The video part of the packet. */
		PacketVideo video;

		/** The audio part of the packet. */
		std::vector<PacketAudio> audio;
	};

	struct ADPCM51Streams {
		bool enabled;

		std::vector<Sound::AudioStream *> streams;

		ADPCM51Streams();
		~ADPCM51Streams();
	};

	Common::SeekableReadStream *_xmv;

	/** The time the XMV was started. */
	uint32 _startTime;

	/** All audio tracks within the XMV. */
	std::vector<AudioTrack> _audioTracks;

	/** Streams needed for 5.1 interleaving. */
	ADPCM51Streams _adpcm51Streams;

	/** The current packet. */
	Packet _curPacket;

	/** The video codec. */
	Codec *_videoCodec;


	/** Load an XMV file. */
	void load();

	/** Evaluate whether and how we support the format of the audio track. */
	void evaluateAudioTrack(AudioTrack &track);

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

	/** Queue audio stream data belonging to this track. */
	void queueAudioStream(Common::SeekableReadStream *stream, const AudioTrack &track);
};

} // End of namespace Video

#endif // VIDEO_XMV_H

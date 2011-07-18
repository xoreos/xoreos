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

/** @file video/xmv.h
 *  Decoding Microsoft Xbox XMV videos.
 */

#ifndef VIDEO_XMV_H
#define VIDEO_XMV_H

#include "video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Video {

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
	struct AudioTrack {
		uint16 compression;
		uint16 channels;
		uint32 rate;
		uint16 bitsPerSample;
		uint16 flags;

		bool supported;
		bool enabled;

		byte audioStreamFlags;
		uint32 bitRate;
	};

	struct PacketVideoHeader {
		byte header[8];

		uint32 dataSize;
	};

	struct PacketAudioHeader {
		byte header[4];

		uint32 dataSize;
	};

	struct PacketHeader {
		uint32 nextPacketSize;

		PacketVideoHeader video;
		std::vector<PacketAudioHeader> audio;
	};

	Common::SeekableReadStream *_xmv;

	std::vector<AudioTrack> _audioTracks;

	uint32 _thisPacketSize;
	uint32 _nextPacketSize;
	uint32 _nextFrameTime;

	uint32 _audioLength;

	uint32 _curPacket;

	/** Load an XMV file. */
	void load();

	void evalAudioTrack(AudioTrack &track);

	void processPacketHeader(PacketHeader &packetHeader);
	void processVideoData(PacketVideoHeader &videoHeader);
	void processAudioData(PacketAudioHeader &audioHeader, const AudioTrack &track);
	void processAudioData(std::vector<PacketAudioHeader> &audioHeader);

	void queueAudioStream(Common::SeekableReadStream *stream, const AudioTrack &track);
};

} // End of namespace Video

#endif // VIDEO_XMV_H

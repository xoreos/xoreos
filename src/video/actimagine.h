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
 *  Decoding Actimagine videos.
 */

#ifndef VIDEO_ACTIMAGINE_H
#define VIDEO_ACTIMAGINE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Video {

/** A decoder for Actimagine videos. */
class ActimagineDecoder : public VideoDecoder {
public:
	ActimagineDecoder(Common::SeekableReadStream *vx);
	~ActimagineDecoder();

protected:
	void decodeNextTrackFrame(VideoTrack &track);

private:
	void goToKeyFrame(size_t n);

	class ActimagineVideoTrack : public FixedRateVideoTrack {
	public:
		ActimagineVideoTrack(uint32 width, uint32 height, uint32 numFrames, Common::Rational fps);

		uint32 getWidth() const override;
		uint32 getHeight() const override;

		int getCurFrame() const override;

	protected:
		Common::Rational getFrameRate() const override;

	private:
		uint32 _width;
		uint32 _height;
		uint32 _numFrames;

		const Common::Rational _fps;
	};

	// TODO: Audio Track
	// It might be one of FastAudio or GCADPCM
	class ActimagineAudioTrack : public AudioTrack {
	public:
		ActimagineAudioTrack(uint32 sampleRate, uint32 channelCount, uint32 audioOffset);

		bool canBufferData() const override;

	protected:
		Sound::AudioStream *getAudioStream() const override;

	private:
		uint32 _sampleRate;
		uint32 _channelCount;
		uint32 _audioOffset;
	};

	struct KeyFrame {
		uint32 frameNumber;
		uint32 dataOffset;
	};

	Common::ScopedPtr<Common::SeekableReadStream> _vx;

	std::vector<KeyFrame> _keyFrames;

	uint32 _biggestFrame;

	/** Load an Actimagine file. */
	void load();
};

} // End of namespace Video

#endif // VIDEO_ACTIMAGINE_H

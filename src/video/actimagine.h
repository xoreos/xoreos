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

#include <memory>

#include "src/common/types.h"

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
		ActimagineVideoTrack(uint32_t width, uint32_t height, uint32_t numFrames, Common::Rational fps);

		uint32_t getWidth() const override;
		uint32_t getHeight() const override;

		int getCurFrame() const override;

	protected:
		Common::Rational getFrameRate() const override;

	private:
		uint32_t _width;
		uint32_t _height;
		uint32_t _numFrames;

		const Common::Rational _fps;
	};

	// TODO: Audio Track
	// It might be one of FastAudio or GCADPCM
	class ActimagineAudioTrack : public AudioTrack {
	public:
		ActimagineAudioTrack(uint32_t sampleRate, uint32_t channelCount, uint32_t audioOffset);

		bool canBufferData() const override;

	protected:
		Sound::AudioStream *getAudioStream() const override;

	private:
		uint32_t _sampleRate;
		uint32_t _channelCount;
		uint32_t _audioOffset;
	};

	struct KeyFrame {
		uint32_t frameNumber;
		uint32_t dataOffset;
	};

	std::unique_ptr<Common::SeekableReadStream> _vx;

	std::vector<KeyFrame> _keyFrames;

	uint32_t _biggestFrame;

	/** Load an Actimagine file. */
	void load();
};

} // End of namespace Video

#endif // VIDEO_ACTIMAGINE_H

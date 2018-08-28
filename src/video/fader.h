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
 *  Testing implementation for the VideoDecoder interface.
 */

#ifndef VIDEO_FADER_H
#define VIDEO_FADER_H

#include "src/common/rational.h"

#include "src/video/decoder.h"

namespace Video {

/** A quick test fader "video". */
class Fader : public VideoDecoder {
public:
	Fader(uint32 width, uint32 height, int n);

protected:
	void decodeNextTrackFrame(VideoTrack &track);

private:
	class FaderVideoTrack : public FixedRateVideoTrack {
	public:
		FaderVideoTrack(uint32 width, uint32 height, int n);

		uint32 getWidth() const { return _width; }
		uint32 getHeight() const { return _height; }
		int getCurFrame() const { return _curFrame; }
		int getFrameCount() const { return _n * 128; }

		void drawFrame(Graphics::Surface &surface);

	protected:
		Common::Rational getFrameRate() const { return 50; }

	private:
		uint32 _width;
		uint32 _height;
		int _curFrame;
		byte _c;
		int _n;
	};
};

} // End of namespace Video

#endif // VIDEO_FADER_H

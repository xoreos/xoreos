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
 *  Generic video decoder interface.
 */

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/graphics/types.h"
#include "src/graphics/glcontainer.h"
#include "src/graphics/renderable.h"

#include "src/sound/types.h"

namespace Graphics {
	class Surface;
}

namespace Sound {
	class AudioStream;
	class QueuingAudioStream;
}

namespace Video {

/** A generic interface for video decoders. */
class VideoDecoder : public Graphics::GLContainer, public Graphics::Renderable {
public:
	enum Scale {
		kScaleNone,  ///< Don't scale the video.
		kScaleUp,    ///< Only scale the video up, if necessary.
		kScaleDown,  ///< Only scale the video down, if necessary.
		kScaleUpDown ///< Scale the video up and down, if necessary.
	};

	VideoDecoder();
	~VideoDecoder();

	void setScale(Scale scale);

	/** Is the video currently playing? */
	bool isPlaying() const;

	/** Return the size of this video. */
	void getSize(uint32 &width, uint32 &height) const;

	/** Start playing the video. */
	void start();

	/** Abort the playing of the video. */
	void abort();

	/** Returns the time position (in ms) of the current video. */
	uint32 getTime() const;

	/** Return the time, in milliseconds, to the next frame. */
	uint32 getTimeToNextFrame() const;

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

protected:
	bool _started;  ///< Has playback started?
	bool _finished; ///< Has playback finished?
	bool _needCopy; ///< Is new frame content available that needs to by copied?

	uint32 _width;  ///< The video's width.
	uint32 _height; ///< The video's height.

	Common::ScopedPtr<Graphics::Surface> _surface; ///< The video's surface.

	/** Create a surface for video of these dimensions.
	 *
	 *  Since the data will be copied into the graphics card memory, the surface's
	 *  actual dimensions will be rounded up to the next power of two values.
	 *
	 *  The surface's width and height will reflects that, while the video's
	 *  width and height will be stored in _width and _height.
	 *
	 *  The surface's pixel format is always BGRA8888.
	 */
	void initVideo(uint32 width, uint32 height);

	void initSound(uint16 rate, int channels, bool is16);
	void deinitSound();

	void queueSound(const byte *data, uint32 dataSize);
	void queueSound(Sound::AudioStream *stream);

	void finishSound();

	uint32 getNumQueuedStreams() const;

	/** Start the video processing. */
	virtual void startVideo() = 0;
	/** Process the video's image and sound data further. */
	virtual void processData() = 0;

	/** Return the time, in milliseconds, of the next frame's time. */
	virtual uint32 getNextFrameStartTime() const = 0;

	void finish();

	void deinit();

	// GLContainer
	void doRebuild();
	void doDestroy();

private:
	Graphics::TextureID _texture;

	float _textureWidth;
	float _textureHeight;

	Scale _scale;

	Common::ScopedPtr<Sound::QueuingAudioStream> _sound;
	Sound::ChannelHandle _soundHandle;

	uint16 _soundRate;
	byte   _soundFlags;

	/** The start time of the video, or -1 for not set */
	uint32 _startTime;


	/** Update the video, if necessary. */
	void update();

	/** Copy the video image data to the texture. */
	void copyData();

	/** Get the dimensions of the quad to draw the texture on. */
	void getQuadDimensions(float &width, float &height) const;
};

} // End of namespace Video

#endif // VIDEO_DECODER_H

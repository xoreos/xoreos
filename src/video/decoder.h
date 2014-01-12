/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file video/decoder.h
 *  Generic video decoder interface.
 */

#ifndef VIDEO_DECODER_H
#define VIDEO_DECODER_H

#include <OgrePrerequisites.h>

#include "common/types.h"
#include "common/mutex.h"

#include "graphics/types.h"

#include "sound/types.h"

namespace Graphics {
	class Surface;
}

namespace Sound {
	class AudioStream;
	class QueuingAudioStream;
}

namespace Video {

/** A generic interface for video decoders. */
class VideoDecoder {
public:
	VideoDecoder();
	virtual ~VideoDecoder();

	/** Is the video currently playing? */
	bool isPlaying() const;

	/** Start playing the video. */
	void start();

	/** Abort the playing of the video. */
	void abort();

	/** Return the size of the actual video. */
	void getVideoSize(int &width, int &height) const;

	/** Return the size of the video surface. */
	void getSurfaceSize(int &width, int &height) const;

	/** Return the time, in milliseconds, to the next frame. */
	virtual uint32 getTimeToNextFrame() const = 0;

	/** Render the next frame. */
	void renderFrame();

	/** Copy the current frame into texture memory. */
	void copyIntoTexture(Ogre::TexturePtr texture);


protected:
	Common::Mutex _mutex;
	Common::Mutex _surfaceMutex;

	bool _started;  ///< Has playback started?
	bool _finished; ///< Has playback finished?

	uint32 _width;  ///< The video's width.
	uint32 _height; ///< The video's height.

	Graphics::Surface *_surfaces[2];
	Graphics::Surface *_surface; ///< The video's surface.

	int _currentSurface;

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

	void finish();

	void deinit();

private:
	Sound::QueuingAudioStream *_sound;
	Sound::ChannelHandle       _soundHandle;
	uint16                     _soundRate;
	byte                       _soundFlags;


	/** Update the video, if necessary. */
	void update();

	/** Copy the video image data to the texture. */
	void copyData();

	/** Get the dimensions of the quad to draw the texture on. */
	void getQuadDimensions(float &width, float &height) const;
};

} // End of namespace Video

#endif // VIDEO_DECODER_H

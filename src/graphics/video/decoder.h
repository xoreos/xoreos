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

/** @file graphics/video/decoder.h
 *  Generic video decoder interface.
 */

#ifndef GRAPHICS_VIDEO_DECODER_H
#define GRAPHICS_VIDEO_DECODER_H

#include "common/types.h"
#include "common/mutex.h"

#include "graphics/types.h"
#include "graphics/glcontainer.h"
#include "graphics/queueable.h"

#include "sound/types.h"

namespace Sound {
	class AudioStream;
	class QueuingAudioStream;
}

namespace Graphics {

class Surface;

/** A generic interface for video decoders. */
class VideoDecoder : public GLContainer {
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

	/** Update the video. */
	void update();

	/** Render the video to OpenGL. */
	void render();

	/** Abort the playing of the video. */
	void abort();

	/** Is there enough time to spare to sleep for 10ms? */
	virtual bool hasTime() const = 0;

protected:
	volatile bool _started;  ///< Has playback started?
	volatile bool _finished; ///< Has playback finished?
	volatile bool _needCopy; ///< Is new frame content available that needs to by copied?

	uint32 _width;  ///< The video's width.
	uint32 _height; ///< The video's height.

	Surface *_surface; ///< The video's surface.

	/** Create a data area for a video of these dimensions.
	 *
	 *  Since the data will be copied into the graphics card memory, the surface
	 *  actualy dimensions will be rounded up to the next power of two values.
	 *
	 *  The surface's width and height will reflects that, while the video's
	 *  width and height will be stored in _width and _height.
	 *
	 *  The surface's pixel format is always BGRA8888.
	 */
	void createData(uint32 width, uint32 height);

	void initSound(uint16 rate, bool stereo, bool is16);
	void deinitSound();

	void queueSound(const byte *data, uint32 dataSize);
	void queueSound(Sound::AudioStream *stream);

	uint32 getNumQueuedStreams() const;

	/** Process the video's image and sound data further. */
	virtual void processData() = 0;

	// GLContainer
	void doRebuild();
	void doDestroy();

private:
	TextureID _texture;

	float _textureWidth;
	float _textureHeight;

	Scale _scale;

	Common::Mutex _canUpdate;
	Common::Mutex _canCopy;

	Sound::QueuingAudioStream *_sound;
	Sound::ChannelHandle       _soundHandle;
	uint16                     _soundRate;
	byte                       _soundFlags;

	/** Copy the video image data to the texture. */
	void copyData();

	/** Get the dimensions of the quad to draw the texture on. */
	void getQuadDimensions(float &width, float &height) const;
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_DECODER_H

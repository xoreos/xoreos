/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/decoder.h
 *  Generic video decoder interface.
 */

#ifndef GRAPHICS_VIDEO_DECODER_H
#define GRAPHICS_VIDEO_DECODER_H

#include "common/types.h"
#include "common/mutex.h"

#include "graphics/types.h"
#include "graphics/queueable.h"

#include "sound/types.h"

namespace Sound {
	class AudioStream;
	class QueuingAudioStream;
}

namespace Graphics {

/** A generic interface for video decoders. */
class VideoDecoder : public Queueable<VideoDecoder> {
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
	volatile bool _started;
	volatile bool _finished;
	volatile bool _needCopy;

	uint32 _width;
	uint32 _height;
	uint32 _pitch;

	byte *_data;

	/** Create a data area for a video of these dimensions. */
	void createData(uint32 width, uint32 height);

	void initSound(uint16 rate, bool stereo, bool is16);
	void deinitSound();

	void queueSound(const byte *data, uint32 dataSize);
	void queueSound(Sound::AudioStream *stream);

	uint32 getNumQueuedStreams() const;

	/** Process the video's image and sound data further. */
	virtual void processData() = 0;

private:
	TextureID _texture;

	uint32 _realWidth;
	uint32 _realHeight;

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

// To be called from the main/events/graphics thread
public:
	/** Rebuild the video's texture. */
	void rebuild();
	/** Destroy the video's texture. */
	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_DECODER_H

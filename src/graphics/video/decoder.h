/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
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

namespace Graphics {

/** A generic interface for video decoders. */
class VideoDecoder : public Queueable<VideoDecoder> {
public:
	VideoDecoder();
	~VideoDecoder();

	/** Is the video currently playing? */
	bool isPlaying() const;

	/** Update the video. */
	void update();

	/** Render the video to OpenGL. */
	void render();

	/** Abort the playing of the video. */
	void abort();

	/** Got enough time to spare to sleep for 10ms? */
	virtual bool gotTime() const = 0;

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

	/** Process the video's image and sound data further. */
	virtual void processData() = 0;

private:
	TextureID _texture;

	uint32 _realWidth;
	uint32 _realHeight;

	float _textureWidth;
	float _textureHeight;

	Common::Mutex _canUpdate;
	Common::Mutex _canCopy;

	/** Copy the video image data to the texture. */
	void copyData();

// To be called from the main/events/graphics thread
public:
	/** Rebuild the video's texture. */
	void rebuild();
	/** Destroy the video's texture. */
	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_DECODER_H

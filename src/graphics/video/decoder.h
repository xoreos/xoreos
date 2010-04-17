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

#include "graphics/types.h"
#include "graphics/queueable.h"

namespace Graphics {

/** A generic interface for video decoders. */
class VideoDecoder : public Queueable<VideoDecoder> {
public:
	VideoDecoder();
	~VideoDecoder();

	bool isPlaying() const;

	/** Update the video.
	 *
	 *  Draws the next video frame onto the texture and queues the next audio frame,
	 *  if necessary.
	 */
	virtual void update() = 0;

	/** Render the video to OpenGL. */
	void render();

protected:
	volatile bool _finished;

	uint32 _width;
	uint32 _height;
	uint32 _pitch;

	byte *_data;

	/** Create a data area for a video of these dimensions. */
	void createData(uint32 width, uint32 height);
	/** Copy the video image data to the texture. */
	void copyData();

private:
	TextureID _texture;

	uint32 _realWidth;
	uint32 _realHeight;

	float _textureWidth;
	float _textureHeight;

// To be called from the main/events/graphics thread
public:
	/** Rebuild the video's texture. */
	void rebuild();
	/** Destroy the video's texture. */
	void destroy();
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_DECODER_H

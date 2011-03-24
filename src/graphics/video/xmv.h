/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/xmv.h
 *  Decoding Microsoft Xbox XMV videos.
 */

#ifndef GRAPHICS_VIDEO_XMV_H
#define GRAPHICS_VIDEO_XMV_H

#include "graphics/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Graphics {

/** A decoder for Microsoft Xbox XMV videos. */
class XboxMediaVideo : public VideoDecoder {
public:
	XboxMediaVideo(Common::SeekableReadStream *xmv);
	~XboxMediaVideo();

	bool hasTime() const;

protected:
	void processData();

private:
	Common::SeekableReadStream *_xmv;

	/** Load an XMV file. */
	void load();

	/** Create a new audio stream to be queued. */
	void queueAudioStream(Common::SeekableReadStream *stream);

	uint16 _audioCompression;
	uint16 _audioChannels;
	uint32 _audioRate;
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_XMV_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/videoplayer.h
 *  A video player.
 */

#ifndef GRAPHICS_AURORA_VIDEOPLAYER_H
#define GRAPHICS_AURORA_VIDEOPLAYER_H

namespace Common {
	class UString;
}

namespace Graphics {

class VideoDecoder;

namespace Aurora {

class VideoPlayer {
public:
	VideoPlayer(const Common::UString &video);
	~VideoPlayer();

	void play();

private:
	VideoDecoder *_video;

	void load(const Common::UString &name);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_VIDEOPLAYER_H

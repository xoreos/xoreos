/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/video/player.h
 *  A video player.
 */

#ifndef GRAPHICS_VIDEO_PLAYER_H
#define GRAPHICS_VIDEO_PLAYER_H

#include <string>

namespace Graphics {

class VideoDecoder;

class VideoPlayer {
public:
	VideoPlayer(const std::string &video);
	~VideoPlayer();

	void play();

private:
	VideoDecoder *_video;

	void load(const std::string &name);
};

} // End of namespace Graphics

#endif // GRAPHICS_VIDEO_PLAYER_H

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

/** @file video/aurora/videoplayer.h
 *  A video player.
 */

#ifndef VIDEO_AURORA_VIDEOPLAYER_H
#define VIDEO_AURORA_VIDEOPLAYER_H

namespace Common {
	class UString;
}

namespace Video {

class VideoDecoder;

namespace Aurora {

class VideoFrameRenderer;

/** A video player. */
class VideoPlayer {
public:
	VideoPlayer(const Common::UString &video, int width = -1, int height = -1, int x = 0, int y = 0);
	~VideoPlayer();

	bool isPlaying() const;

	void start();
	void abort();

private:
	VideoDecoder *_video;

	VideoFrameRenderer *_renderer;


	void load(const Common::UString &name);
	void deinit();
};

} // End of namespace Aurora

} // End of namespace Video

#endif // VIDEO_AURORA_VIDEOPLAYER_H

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

/** A video player. */
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

} // End of namespace Video

#endif // VIDEO_AURORA_VIDEOPLAYER_H

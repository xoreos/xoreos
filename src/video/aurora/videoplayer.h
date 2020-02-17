/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  A video player.
 */

#ifndef VIDEO_AURORA_VIDEOPLAYER_H
#define VIDEO_AURORA_VIDEOPLAYER_H

#include <memory>
#include <string>

namespace Video {

class VideoDecoder;

namespace Aurora {

/** A video player. */
class VideoPlayer {
public:
	VideoPlayer(const std::string &video);
	~VideoPlayer();

	void play();

private:
	std::unique_ptr<VideoDecoder> _video;

	void load(const std::string &name);
};

} // End of namespace Aurora

} // End of namespace Video

#endif // VIDEO_AURORA_VIDEOPLAYER_H

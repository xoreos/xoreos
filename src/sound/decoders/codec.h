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
 *  Audio codec base class.
 */

#ifndef SOUND_DECODERS_CODEC_H
#define SOUND_DECODERS_CODEC_H

#include <boost/noncopyable.hpp>

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class AudioStream;

class Codec : boost::noncopyable {
public:
	Codec();
	virtual ~Codec();

	virtual AudioStream *decodeFrame(Common::SeekableReadStream &data) = 0;
};

} // End of namespace Sound

#endif // SOUND_DECODERS_CODEC_H

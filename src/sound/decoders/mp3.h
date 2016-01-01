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
 *  Decoding MP3 (MPEG-1 Audio Layer 3).
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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

#ifndef SOUND_DECODERS_MP3_H
#define SOUND_DECODERS_MP3_H

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class AudioStream;
class RewindableAudioStream;

/**
 * Create a new SeekableAudioStream from the MP3 data in the given stream.
 * Allows for seeking (which is why we require a SeekableReadStream).
 *
 * @param stream          The SeekableReadStream from which to read the MP3 data.
 * @param disposeAfterUse Whether to delete the stream after use.
 *
 * @return A new SeekableAudioStream, or 0, if an error occurred.
 */
RewindableAudioStream *makeMP3Stream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse);

} // End of namespace Sound

#endif // SOUND_DECODERS_MP3_H

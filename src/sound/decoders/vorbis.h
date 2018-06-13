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
 *  Decoding Ogg Vorbis.
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

#ifndef SOUND_DECODERS_VORBIS_H
#define SOUND_DECODERS_VORBIS_H

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#ifdef ENABLE_VORBIS

#include "src/common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class PacketizedAudioStream;
class RewindableAudioStream;

/**
 * Create a new RewindableAudioStream from the Ogg Vorbis data in the given stream.
 *
 * @param stream          The RewindableAudioStream from which to read the Ogg Vorbis data.
 * @param disposeAfterUse Whether to delete the stream after use.
 *
 * @return A new RewindableAudioStream, or 0, if an error occurred.
 */
RewindableAudioStream *makeVorbisStream(
	Common::SeekableReadStream *stream,
	bool disposeAfterUse);

/**
 * Create a new PacketizedAudioStream capable of decoding vorbis audio data.
 *
 * @param extraData
 *    The vorbis header data, combined into one stream, as used in non-ogg
 *    containers.
 * @return a new PacketizedAudioStream, or NULL if an error occurred
 */
PacketizedAudioStream *makePacketizedVorbisStream(
	Common::SeekableReadStream &extraData);

/**
 * Create a new PacketizedAudioStream capable of decoding vorbis audio data.
 *
 * @param packet1
 *    The first vorbis header, as used in ogg
 * @param packet2
 *    The second vorbis header, as used in ogg
 * @param packet3
 *    The third vorbis header, as used in ogg
 * @return a new PacketizedAudioStream, or NULL if an error occurred
 */
PacketizedAudioStream *makePacketizedVorbisStream(
	Common::SeekableReadStream &packet1,
	Common::SeekableReadStream &packet2,
	Common::SeekableReadStream &packet3);

} // End of namespace Sound

#endif // ENABLE_VORBIS

#endif // SOUND_DECODERS_VORBIS_H

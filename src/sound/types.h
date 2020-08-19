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
 *  Basic sound types.
 */

#ifndef SOUND_TYPES_H
#define SOUND_TYPES_H

#include <cstddef>

#include "src/common/types.h"

namespace Sound {

static const size_t kChannelInvalid = SIZE_MAX;

/** A handle to a sound channel. */
struct ChannelHandle {
	size_t channel;
	uint32_t id;

	ChannelHandle() : channel(kChannelInvalid), id(0) { }
};

/** The type of a sound. */
enum SoundType {
	kSoundTypeMusic = 0, ///< Music.
	kSoundTypeSFX   = 1, ///< Sound effect.
	kSoundTypeVoice = 2, ///< Voice/Speech.
	kSoundTypeVideo = 3, ///< Video/Movie.

	kSoundTypeMAX   = 4
};

} // End of namespace Sound

#endif // SOUND_TYPES_H

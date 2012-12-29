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

/** @file sound/types.h
 *  Basic sound types.
 */

#ifndef SOUND_TYPES_H
#define SOUND_TYPES_H

#include "common/types.h"

namespace Sound {

/** A handle to a sound channel. */
struct ChannelHandle {
	uint16 channel;
	uint32 id;

	ChannelHandle() : channel(0), id(0) { }
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

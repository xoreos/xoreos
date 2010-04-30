/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file sound/types.h
 *  Basic sound types.
 */

#ifndef SOUND_TYPES_H
#define SOUND_TYPES_H

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

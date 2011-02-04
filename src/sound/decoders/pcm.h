/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the PCM implementation found in ScummVM.

/** @file sound/decoders/pcm.h
 *  Decoding PCM (Pulse Code Modulation).
 */

#ifndef SOUND_DECODERS_RAW_H
#define SOUND_DECODERS_RAW_H

namespace Sound {

/**
 * Various flags which can be bit-ORed and then passed to
 * makeRawMemoryStream and some other AudioStream factories
 * to control their behavior.
 *
 * Engine authors are advised not to rely on a certain value or
 * order of these flags (in particular, do not store them verbatim
 * in savestates).
 */
enum PCMFlags {
	/** unsigned samples (default: signed) */
	FLAG_UNSIGNED = 1 << 0,

	/** sound is 16 bits wide (default: 8bit) */
	FLAG_16BITS = 1 << 1,

	/** samples are little endian (default: big endian) */
	FLAG_LITTLE_ENDIAN = 1 << 2,

	/** sound is in stereo (default: mono) */
	FLAG_STEREO = 1 << 3
};

/**
 * Creates an audio stream, which plays from the given stream.
 *
 * @param stream Stream object to play from.
 * @param rate   Rate of the sound data.
 * @param flags  Audio flags combination.
 * @see RawFlags
 * @param disposeAfterUse Whether to delete the stream after use.
 * @return The new SeekableAudioStream (or 0 on failure).
 */
RewindableAudioStream *makePCMStream(Common::SeekableReadStream *stream,
                                   int rate, byte flags,
                                   bool disposeAfterUse = true);

} // End of namespace Sound

#endif

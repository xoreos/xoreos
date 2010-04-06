/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file sound/sound.h
 *  The global sound manager, handling all sound output.
 */

#ifndef SOUND_SOUND_H
#define SOUND_SOUND_H

#include "common/singleton.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class SoundManager : public Common::Singleton<SoundManager> {
public:
	// Initialize/deinitialize the mixer
	bool initMixer();
	void deinitMixer();
	const char *getMixerError();

	// Play a wave file (and eventually MP3)
	void playSoundFile(Common::SeekableReadStream *wavStream);

private:
	friend class Common::Singleton<SingletonBaseType>;
};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

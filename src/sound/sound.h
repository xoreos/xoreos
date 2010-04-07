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

#include <SDL_mixer.h>
#include <SDL_sound.h>

#include <vector>

#include "common/singleton.h"
#include "common/thread.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

/** The sound manager. */
class SoundManager : public Common::Singleton<SoundManager>, public Common::Thread {
public:
	SoundManager();

	/** Initialize the sound subsystem. */
	bool init();
	/** Deinitialize the sound subsystem. */
	void deinit();

	/** Was the sound subsystem successfully initialized? */
	bool ready() const;

	/** Is that channel currently playing a sound? */
	bool isPlaying(int channel) const;

	/** Play a sound file.
	 *
	 *  @param  wavStream The stream to play. Will be taken over.
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	int playSoundFile(Common::SeekableReadStream *wavStream);

private:
	struct Channel {
		Sound_Sample *sound;
		Mix_Chunk *wav;
	};

	bool _ready; ///< Was the sound subsystem successfully initialized?

	std::vector<Channel> _channels;

	/** Update the sound information. Called regularily from within the thread method. */
	void update();

	void freeChannel(int channel);
	void setChannel(int channel, Sound_Sample *sound, Mix_Chunk *wav);

	void threadMethod();
};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

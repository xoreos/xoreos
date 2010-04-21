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

// Mac OS X has to have this set up separately because of the include
// path for the OpenAL framework.
#ifdef MACOSX
#	include <al.h>
#	include <alc.h>
#else
#	include <AL/al.h>
#	include <AL/alc.h>
#endif

#include <vector>

#include "common/singleton.h"
#include "common/thread.h"
#include "common/mutex.h"
#include "common/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

// Control how many buffers per sound OpenAL will create
// clone2727 says: 5 is just a safe number. Mine only reached a max of 2.
#define NUM_OPENAL_BUFFERS 5

class AudioStream;

typedef uint32 ChannelHandle;

/** The sound manager. */
class SoundManager : public Common::Singleton<SoundManager>, public Common::Thread {
public:
	SoundManager();

	/** Initialize the sound subsystem. */
	void init();
	/** Deinitialize the sound subsystem. */
	void deinit();

	/** Was the sound subsystem successfully initialized? */
	bool ready() const;

	/** Is that channel currently playing a sound? */
	bool isPlaying(ChannelHandle channel) const;

	/** Play a sound file.
	 *
	 *  @param  wavStream The stream to play. Will be taken over.
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playSoundFile(Common::SeekableReadStream *wavStream, bool loop = false);

	/** Play an audio stream.
	 *
	 *  @param  audStream The stream to play. Will be taken over.
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playAudioStream(AudioStream *audStream);

	/** Set the position the channel is being played. */
	void setChannelPosition(ChannelHandle channel, float x, float y, float z);

	/** Get the position of the channel. */
	void getChannelPosition(ChannelHandle channel, float &x, float &y, float &z);

private:
	struct Channel {
		AudioStream *stream;
		ALuint source;
		ALuint *buffers;
	};

	bool _ready; ///< Was the sound subsystem successfully initialized?

	std::vector<Channel*> _channels;

	Common::Mutex _mutex;

	/** Update the sound information. Called regularily from within the thread method. */
	void update();

	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(Channel *channel);
	void bufferData(uint32 channel);
	void fillBuffer(ALuint source, ALuint alBuffer, AudioStream *stream);

	void freeChannel(uint32 channel);
	void threadMethod();
	AudioStream *makeAudioStream(Common::SeekableReadStream *stream);

	ALCdevice *_dev;
    ALCcontext *_ctx;
};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

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
#include <list>

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

struct ChannelHandle {
	uint16 channel;
	uint32 id;

	ChannelHandle();
};

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
	bool isPlaying(const ChannelHandle &handle) const;

	/** Play a sound file.
	 *
	 *  @param  wavStream The stream to play. Will be taken over.
	 *  @param  loop Should the sound loop?
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playSoundFile(Common::SeekableReadStream *wavStream, bool loop = false);

	/** Play an audio stream.
	 *
	 *  @param  audStream The stream to play.
	 *  @param  disposeAfterUse Should the stream be taken over and discarded once it finshed?
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playAudioStream(AudioStream *audStream, bool disposeAfterUse = true);

	/** Set the position the channel is being played. */
	void setChannelPosition(const ChannelHandle &handle, float x, float y, float z);

	/** Get the position of the channel. */
	void getChannelPosition(const ChannelHandle &handle, float &x, float &y, float &z);

	/** Stop the channel. */
	void stopChannel(ChannelHandle &handle);

	/** Signal that one of streams currently being played has changed and should be updated immediately. */
	void triggerUpdate();

private:
	static const int kChannelCount = 65535;

	struct Channel {
		uint32 id; ///< The channel's ID.

		AudioStream *stream;  ///< The actual audio stream.
		bool disposeAfterUse; ///< Delete the audio stream when done playing?

		ALuint source; ///< OpenAL source for this channel.

		/** List of buffers for that channel. */
		std::list<ALuint> buffers;
		/** List of free buffers not filled with data. */
		std::list<ALuint> freeBuffers;
	};

	bool _ready; ///< Was the sound subsystem successfully initialized?

	Channel *_channels[kChannelCount];

	uint16 _curChannel;
	uint32 _curID;

	Common::Mutex _mutex;

	Common::Condition _needUpdate;

	ALCdevice *_dev;
	ALCcontext *_ctx;

	/** Update the sound information. Called regularily from within the thread method. */
	void update();

	ChannelHandle newChannel();

	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(Channel &channel);
	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(uint16 channel);

	/** Is that channel currently playing a sound? */
	bool isPlaying(uint16 channel) const;

	/** Stop and free a channel. */
	void freeChannel(ChannelHandle &handle);
	/** Stop and free a channel. */
	void freeChannel(uint16 channel);

	Channel *getChannel(const ChannelHandle &handle);

	void threadMethod();

	static AudioStream *makeAudioStream(Common::SeekableReadStream *stream);

	/** Fill the buffer with data from the audio stream. */
	static bool fillBuffer(ALuint source, ALuint alBuffer, AudioStream *stream);

};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

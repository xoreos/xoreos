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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file sound/sound.h
 *  The global sound manager, handling all sound output.
 */

#ifndef SOUND_SOUND_H
#define SOUND_SOUND_H

// Mac OS X has to have this set up separately because of the include
// path for the OpenAL framework.
#ifdef MACOSX
	#include <al.h>
	#include <alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include <vector>
#include <list>

#include "common/types.h"
#include "common/singleton.h"
#include "common/thread.h"
#include "common/mutex.h"

#include "sound/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class AudioStream;

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


	/** Signal that one of streams currently being played has changed and should be updated immediately. */
	void triggerUpdate();


	/** Does this channel handle point to an existing channel? */
	bool isValidChannel(const ChannelHandle &handle) const;

	/** Is that channel currently playing a sound? */
	bool isPlaying(const ChannelHandle &handle);


	// Playing sounds

	/** Play a sound file.
	 *
	 *  This only allocate a channel for the sound, to actually start playing it,
	 *  call startChannel().
	 *
	 *  @param  wavStream The stream to play. Will be taken over.
	 *  @param  type The type of the sound.
	 *  @param  loop Should the sound loop?
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playSoundFile(Common::SeekableReadStream *wavStream,
	                            SoundType type, bool loop = false);

	/** Play an audio stream.
	 *
	 *  This only allocate a channel for the sound, to actually start playing it,
	 *  call startChannel().
	 *
	 *  @param  audStream The stream to play.
	 *  @param  type The type of the sound.
	 *  @param  disposeAfterUse Should the stream be taken over and discarded once it finshed?
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playAudioStream(AudioStream *audStream,
	                              SoundType type, bool disposeAfterUse = true);


	// Starting/Pausing/Stopping channels

	/** Start the channel. */
	void startChannel(ChannelHandle &handle);

	/** Pause the channel. */
	void pauseChannel(ChannelHandle &handle, bool pause);

	/** Stop and free the channel. */
	void stopChannel(ChannelHandle &handle);


	// Pausing/Stopping all channels

	/** Pause all channels. */
	void pauseAll(bool pause);

	/** Stop all channels. */
	void stopAll();


	// Listener properties

	/** Set the gain of the listener (= the global master volume). */
	void setListenerGain(float gain);


	// Channel properties

	/** Set the position the channel is being played. */
	void setChannelPosition(const ChannelHandle &handle, float x, float y, float z);

	/** Get the position of the channel. */
	void getChannelPosition(const ChannelHandle &handle, float &x, float &y, float &z);

	/** Set the gain/volume of the channel. */
	void setChannelGain(const ChannelHandle &handle, float gain);

	/** Set the pitch of the channel. */
	void setChannelPitch(const ChannelHandle &handle, float pitch);


	// Type properties

	/** Set the gain/volume of all channels of a specific type. */
	void setTypeGain(SoundType type, float gain);

private:
	static const int kChannelCount = 65535; ///< Maximal number of channels.

	struct Channel;
	typedef std::list<Channel *> TypeList;

	/** A sound type. */
	struct Type {
		float    gain; ///< The sound type's current gain.
		TypeList list; ///< The list of channels for that type.
	};

	/** A sound channel. */
	struct Channel {
		uint32 id; ///< The channel's ID.

		ALint state; ///< The sound's state.

		AudioStream *stream;  ///< The actual audio stream.
		bool disposeAfterUse; ///< Delete the audio stream when done playing?

		ALuint source; ///< OpenAL source for this channel.

		std::list<ALuint> buffers;     ///< List of buffers for that channel.
		std::list<ALuint> freeBuffers; ///< List of free buffers not filled with data.

		SoundType type;            ///< The channel's sound type.
		TypeList::iterator typeIt; ///< Iterator into the type list.

		float gain; ///< The channel's gain.
	};

	bool _ready; ///< Was the sound subsystem successfully initialized?

	bool _hasSound; //< Do we have working sound output?

	bool _hasMultiChannel; ///< Do we have the multi-channel extension?
	ALenum _format51; ///< The value for the 5.1 multi-channel format.

	Channel *_channels[kChannelCount]; ///< The sound channels.
	Type     _types   [kSoundTypeMAX]; ///< The sound types.

	uint16 _curChannel; ///< Position to start looking for a free channel.
	uint32 _curID;      ///< The ID the next sound will get.

	Common::Mutex _mutex;

	/** Condition to signal that an update is needed. */
	Common::Condition _needUpdate;

	ALCdevice *_dev;
	ALCcontext *_ctx;

	/** Check that the SoundManager was properly initialized. */
	void checkReady();

	/** Update the sound information. Called regularily from within the thread method. */
	void update();

	/** Look for a free place in the channel vector. */
	ChannelHandle newChannel();

	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(Channel &channel);
	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(uint16 channel);

	/** Is that channel currently playing a sound? */
	bool isPlaying(uint16 channel) const;

	/** Pause/Unpause a channel. */
	void pauseChannel(Channel *channel, bool pause);

	/** Stop and free a channel. */
	void freeChannel(ChannelHandle &handle);
	/** Stop and free a channel. */
	void freeChannel(uint16 channel);

	/** Return the channel the handle refers to. */
	Channel *getChannel(const ChannelHandle &handle);

	void threadMethod();

	static AudioStream *makeAudioStream(Common::SeekableReadStream *stream);

	/** Fill the buffer with data from the audio stream. */
	bool fillBuffer(ALuint source, ALuint alBuffer, AudioStream *stream) const;
};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

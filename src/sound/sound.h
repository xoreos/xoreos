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
 *  The global sound manager, handling all sound output.
 */

#ifndef SOUND_SOUND_H
#define SOUND_SOUND_H

// Mac OS X has to have this set up separately because of the include
// path for the OpenAL framework.
#ifdef MACOSX
	#include <OpenAL/al.h>
	#include <OpenAL/alc.h>
#else
	#include <AL/al.h>
	#include <AL/alc.h>
#endif

#include <list>
#include <map>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/disposableptr.h"
#include "src/common/singleton.h"
#include "src/common/thread.h"
#include "src/common/mutex.h"
#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

class AudioStream;

/** The sound manager. */
class SoundManager : public Common::Singleton<SoundManager>, public Common::Thread {
public:
	SoundManager();
	~SoundManager();

	/** Initialize the sound subsystem. */
	void init();
	/** Deinitialize the sound subsystem. */
	void deinit();

	/** Was the sound subsystem successfully initialized? */
	bool ready() const;


	/** Signal that one of streams currently being played has changed and should be updated immediately. */
	void triggerUpdate();


	// .--- Channel status
	/** Does this channel handle point to an existing channel? */
	bool isValidChannel(const ChannelHandle &handle) const;

	/** Is that channel currently playing a sound? */
	bool isPlaying(const ChannelHandle &handle);
	/** Is that channel currently paused? */
	bool isPaused(const ChannelHandle &handle);

	/** Return the number of samples this channel has already played. */
	uint64 getChannelSamplesPlayed(const ChannelHandle &handle);
	/** Return the time this channel has already played in milliseconds. */
	uint64 getChannelDurationPlayed(const ChannelHandle &handle);
	// '---

	// .--- Playing sounds
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
	 *  @param  disposeAfterUse Should the stream be taken over and discarded once it finished?
	 *  @return The channel the sound has been assigned to, or -1 on error.
	 */
	ChannelHandle playAudioStream(AudioStream *audStream,
	                              SoundType type, bool disposeAfterUse = true);
	// '---

	// .--- Starting/Pausing/Stopping channels
	/** Start the channel. */
	void startChannel(ChannelHandle &handle);

	/** Pause the channel. */
	void pauseChannel(ChannelHandle &handle, bool pause);
	/** Pause toggle the channel. */
	void pauseChannel(ChannelHandle &handle);

	/** Stop and free the channel. */
	void stopChannel(ChannelHandle &handle);
	// '---

	// .--- Pausing/Stopping all channels
	/** Pause all channels. */
	void pauseAll(bool pause);

	/** Stop all channels. */
	void stopAll();
	// '---


	// .--- Listener properties
	/** Set the gain of the listener (= the global master volume). */
	void setListenerGain(float gain);
	// '---

	// .--- Channel properties
	/** Set the position the channel is being played. */
	void setChannelPosition(const ChannelHandle &handle, float x, float y, float z);

	/** Get the position of the channel. */
	void getChannelPosition(const ChannelHandle &handle, float &x, float &y, float &z);

	/** Set the gain/volume of the channel. */
	void setChannelGain(const ChannelHandle &handle, float gain);

	/** Set the pitch of the channel. */
	void setChannelPitch(const ChannelHandle &handle, float pitch);
	// '---

	// .--- Type properties
	/** Set the gain/volume of all channels of a specific type. */
	void setTypeGain(SoundType type, float gain);
	// '---

	// .--- Utility methods
	/** Create an audio stream from this data stream.
	 *
	 *  The ownership of the data stream is transferred to the audio stream
	 *  if one was created without an exception being thrown.
	 */
	static AudioStream *makeAudioStream(Common::SeekableReadStream *stream);

	/** Return a string representing the channel referenced by this handle. */
	Common::UString formatChannel(const ChannelHandle &handle) const;
	// '---

private:
	static const size_t kChannelCount = 65535; ///< Maximal number of channels.

	struct Channel;
	typedef std::list<Channel *> TypeList;

	/** A sound type. */
	struct Type {
		float    gain; ///< The sound type's current gain.
		TypeList list; ///< The list of channels for that type.
	};

	/** A sound channel. */
	struct Channel {
		uint32 id;    ///< The channel's ID.
		size_t index; ///< The channel's index.

		ALint state; ///< The sound's state.

		Common::DisposablePtr<AudioStream> stream;  ///< The actual audio stream.

		ALuint source; ///< OpenAL source for this channel.

		std::list<ALuint> buffers;     ///< List of buffers for that channel.
		std::list<ALuint> freeBuffers; ///< List of free buffers not filled with data.

		std::map<ALuint, ALsizei> bufferSize; ///< Size of a buffer in bytes.

		SoundType type;            ///< The channel's sound type.
		TypeList::iterator typeIt; ///< Iterator into the type list.

		/** Number of bytes in all buffers that finished playing and were unqueued. */
		uint64 finishedBuffers;

		float gain; ///< The channel's gain.

		Channel(uint32 i, size_t idx, SoundType t, const TypeList::iterator &ti, AudioStream *s, bool d);
	};

	bool _ready; ///< Was the sound subsystem successfully initialized?

	bool _hasSound; ///< Do we have working sound output?

	bool _hasMultiChannel; ///< Do we have the multi-channel extension?
	ALenum _format51; ///< The value for the 5.1 multi-channel format.

	Common::ScopedPtr<Channel> _channels[kChannelCount]; ///< The sound channels.
	Type _types[kSoundTypeMAX]; ///< The sound types.

	uint32 _curID; ///< The ID the next sound will get.

	Common::Mutex _mutex;

	/** Condition to signal that an update is needed. */
	Common::Condition _needUpdate;

	ALCdevice *_dev;
	ALCcontext *_ctx;

	/** Check that the SoundManager was properly initialized. */
	void checkReady();

	/** Update the sound information. Called regularly from within the thread method. */
	void update();

	/** Look for a free place in the channel vector. */
	ChannelHandle newChannel();

	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(Channel &channel);
	/** Buffer more sound from the channel to the OpenAL buffers. */
	void bufferData(size_t channel);

	/** Is that channel currently playing a sound? */
	bool isPlaying(size_t channel) const;

	/** Pause/Unpause a channel. */
	void pauseChannel(Channel *channel, bool pause);
	/** Pause toggle channel. */
	void pauseChannel(Channel *channe);

	/** Stop and free a channel. */
	void freeChannel(ChannelHandle &handle);
	/** Stop and free a channel. */
	void freeChannel(size_t channel);

	/** Return the channel the handle refers to. */
	const Channel *getChannel(const ChannelHandle &handle) const;
	/** Return the channel the handle refers to. */
	Channel *getChannel(const ChannelHandle &handle);

	void threadMethod();

	/** Fill the buffer with data from the audio stream. */
	bool fillBuffer(const Channel &channel, ALuint alBuffer,
	                AudioStream *stream, ALsizei &bufferedSize) const;

	/** Return a string representing this channel. */
	Common::UString formatChannel(const Channel *channel) const;
};

} // End of namespace Sound

/** Shortcut for accessing the sound manager. */
#define SoundMan Sound::SoundManager::instance()

#endif // SOUND_SOUND_H

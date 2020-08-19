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
 *  Streaming audio.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
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
 */

#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include <memory>

#include <boost/noncopyable.hpp>

#include "src/common/util.h"
#include "src/common/types.h"
#include "src/common/disposableptr.h"

namespace Common {
class SeekableReadStream;
}

namespace Sound {

/**
 * Generic audio input stream. Subclasses of this are used to feed arbitrary
 * sampled audio data into xoreos' SoundManager.
 */
class AudioStream : boost::noncopyable {
public:
	static const size_t kSizeInvalid = SIZE_MAX;

	virtual ~AudioStream() {}

	/**
	 * Fill the given buffer with up to numSamples samples. Returns the actual
	 * number of samples read, or kSizeInvalid if a critical error occurred
	 * (note: you *must* check if this value is less than what you requested,
	 * this can happen when the stream is fully used up).
	 *
	 * Data has to be in native endianness, 16 bit per sample, signed. For stereo
	 * stream, buffer will be filled with interleaved left and right channel
	 * samples, starting with a left sample. Furthermore, the samples in the
	 * left and right are summed up. So if you request 4 samples from a stereo
	 * stream, you will get a total of two left channel and two right channel
	 * samples.
	 *
	 * The same holds true for more channels. Channel configurations recognized:
	 * - 5.1: front left, front right, front center, low frequency rear left, rear right
	 */
	virtual size_t readBuffer(int16_t *buffer, const size_t numSamples) = 0;

	/** Return the number channels in this stream. */
	virtual int getChannels() const = 0;

	/** Sample rate of the stream. */
	virtual int getRate() const = 0;

	/**
	 * End of data reached? If this returns true, it means that at this
	 * time there is no data available in the stream. However there may be
	 * more data in the future.
	 * This is used by e.g. a rate converter to decide whether to keep on
	 * converting data or stop.
	 */
	virtual bool endOfData() const = 0;

	/**
	 * End of stream reached? If this returns true, it means that all data
	 * in this stream is used up and no additional data will appear in it
	 * in the future.
	 * This is used by the mixer to decide whether a given stream shall be
	 * removed from the list of active streams (and thus be destroyed).
	 * By default this maps to endOfData()
	 */
	virtual bool endOfStream() const { return endOfData(); }
};

/**
 * A rewindable audio stream. This allows for resetting the AudioStream
 * to its initial state. Note that rewinding itself is not required to
 * be working when the stream is being played by Mixer!
 */
class RewindableAudioStream : public AudioStream {
public:
	static const uint64_t kInvalidLength = UINT64_C(0xFFFFFFFFFFFFFFFF);

	/**
	 * Rewinds the stream to its start.
	 *
	 * @return true on success, false otherwise.
	 */
	virtual bool rewind() = 0;

	/**
	 * Estimate the total number of samples per channel in this stream.
	 * If this value is not calculatable, return kInvalidLength.
	 */
	virtual uint64_t getLength() const { return kInvalidLength; }

	/**
	 * Estimate the total duration of the stream in milliseconds.
	 * If this value is not calculable, return kInvalidLength.
	 */
	virtual uint64_t getDuration() const {
		if ((getLength() == kInvalidLength) || (getRate() <= 0))
			return kInvalidLength;

		return (getLength() * 1000) / getRate();
	}
};

/** An empty audio stream that plays nothing. */
class EmptyAudioStream : public RewindableAudioStream {
public:
	EmptyAudioStream() { }

	size_t readBuffer(int16_t *UNUSED(buffer), const size_t UNUSED(numSamples)) { return 0; }

	int getChannels() const { return 1; }

	int getRate() const { return 44100; }

	bool endOfData() const { return true; }
	bool rewind() { return true; }

	uint64_t getLength() const { return 0; }
};

/**
 * A looping audio stream. This object does nothing besides using
 * a RewindableAudioStream to play a stream in a loop.
 */
class LoopingAudioStream : public AudioStream {
public:
	/**
	 * Creates a looping audio stream object.
	 *
	 * @see makeLoopingAudioStream
	 *
	 * @param stream Stream to loop
	 * @param loops How often to loop (0 = infinite)
	 * @param disposeAfterUse Destroy the stream after the LoopingAudioStream has finished playback.
	 */
	LoopingAudioStream(RewindableAudioStream *stream, size_t loops, bool disposeAfterUse = true);
	~LoopingAudioStream();

	size_t readBuffer(int16_t *buffer, const size_t numSamples);
	bool endOfData() const;

	int getChannels() const { return _parent->getChannels(); }
	int getRate() const { return _parent->getRate(); }

	/** Returns number of loops the stream has played. */
	size_t getCompleteIterations() const { return _completeIterations; }

	bool rewind();

	uint64_t getLength() const;
	uint64_t getDuration() const;

	/** Return the length of one loop. */
	uint64_t getLengthOnce() const;
	/** Return the duration of one loop. */
	uint64_t getDurationOnce() const;

private:
	Common::DisposablePtr<RewindableAudioStream> _parent;

	size_t _loops;
	size_t _completeIterations;
};

/**
 * Wrapper functionality to efficiently create a stream, which might be looped.
 *
 * Note that this function does not return a LoopingAudioStream, because it does
 * not create one when the loop count is "1". This allows to keep the runtime
 * overhead down, when the code does not require any functionality only offered
 * by LoopingAudioStream.
 *
 * @param stream Stream to loop (will be automatically destroyed, when the looping is done)
 * @param loops How often to loop (0 = infinite)
 * @return A new AudioStream, which offers the desired functionality.
 */
AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, size_t loops);

class QueuingAudioStream : public AudioStream {
public:

	/**
	 * Queue an audio stream for playback. This stream plays all queued
	 * streams, in the order they were queued. If disposeAfterUse is set to
	 * DisposeAfterUse::YES, then the queued stream is deleted after all data
	 * contained in it has been played.
	 */
	virtual void queueAudioStream(AudioStream *audStream, bool disposeAfterUse = true) = 0;

	/**
	 * Mark this stream as finished. That is, signal that no further data
	 * will be queued to it. Only after this has been done can this
	 * stream ever 'end'.
	 */
	virtual void finish() = 0;

	/**
	 * Is the stream marked as finished?
	 */
	virtual bool isFinished() const = 0;

	/**
	 * Return the number of streams still queued for playback (including
	 * the currently playing stream).
	 */
	virtual size_t numQueuedStreams() const = 0;
};

/**
 * Factory function for an QueuingAudioStream.
 */
QueuingAudioStream *makeQueuingAudioStream(int rate, int channels);

/**
 * An AudioStream designed to work in terms of packets.
 *
 * It is similar in concept to QueuingAudioStream, but does not
 * necessarily rely on the data from each queued AudioStream
 * being separate.
 */
class PacketizedAudioStream : public virtual AudioStream {
public:
	virtual ~PacketizedAudioStream() {}

	/**
	 * Queue the next packet to be decoded.
	 */
	virtual void queuePacket(Common::SeekableReadStream *data) = 0;

	/**
	 * Mark this stream as finished. That is, signal that no further data
	 * will be queued to it. Only after this has been done can this
	 * stream ever 'end'.
	 */
	virtual void finish() = 0;

	/**
	 * Is the stream marked as finished?
	 */
	virtual bool isFinished() const = 0;
};

/**
 * A PacketizedAudioStream that works closer to a QueuingAudioStream.
 * It queues individual packets as whole AudioStream to an internal
 * QueuingAudioStream. This is used for writing quick wrappers against
 * e.g. PCMStream, which can be made into PacketizedAudioStreams with
 * little effort.
 */
class StatelessPacketizedAudioStream : public PacketizedAudioStream {
public:
	StatelessPacketizedAudioStream(int rate, int channels) :
		_rate(rate), _channels(channels), _stream(makeQueuingAudioStream(rate, channels)) {}
	virtual ~StatelessPacketizedAudioStream() {}

	// AudioStream API
	int getChannels() const { return _channels; }
	int getRate() const { return _rate; }
	size_t readBuffer(int16_t *data, const size_t numSamples) { return _stream->readBuffer(data, numSamples); }
	bool endOfData() const { return _stream->endOfData(); }
	bool endOfStream() const { return _stream->endOfStream(); }

	// PacketizedAudioStream API
	void queuePacket(Common::SeekableReadStream *data) { _stream->queueAudioStream(makeStream(data)); }
	void finish() { _stream->finish(); }
	bool isFinished() const { return _stream->isFinished(); }

protected:
	/**
	 * Make the AudioStream for a given packet
	 */
	virtual AudioStream *makeStream(Common::SeekableReadStream *data) = 0;

private:
	int _rate;
	int _channels;
	std::unique_ptr<QueuingAudioStream> _stream;
};

} // End of namespace Sound

#endif // SOUND_AUDIOSTREAM_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef SOUND_AUDIOSTREAM_H
#define SOUND_AUDIOSTREAM_H

#include "common/util.h"
#include "common/types.h"

namespace Sound {

/**
 * Generic audio input stream. Subclasses of this are used to feed arbitrary
 * sampled audio data into eos' SoundManager.
 */
class AudioStream {
public:
	virtual ~AudioStream() {}

	/**
	 * Fill the given buffer with up to numSamples samples. Returns the actual
	 * number of samples read, or -1 if a critical error occurred (note: you
	 * *must* check if this value is less than what you requested, this can
	 * happen when the stream is fully used up).
	 *
	 * Data has to be in native endianess, 16 bit per sample, signed. For stereo
	 * stream, buffer will be filled with interleaved left and right channel
	 * samples, starting with a left sample. Furthermore, the samples in the
	 * left and right are summed up. So if you request 4 samples from a stereo
	 * stream, you will get a total of two left channel and two right channel
	 * samples.
	 */
	virtual int readBuffer(int16 *buffer, const int numSamples) = 0;

	/** Is this a stereo stream? */
	virtual bool isStereo() const = 0;

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
 * A rewindable audio stream. This allows for reseting the AudioStream
 * to its initial state. Note that rewinding itself is not required to
 * be working when the stream is being played by Mixer!
 */
class RewindableAudioStream : public AudioStream {
public:
	/**
	 * Rewinds the stream to its start.
	 *
	 * @return true on success, false otherwise.
	 */
	virtual bool rewind() = 0;
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
	LoopingAudioStream(RewindableAudioStream *stream, uint loops, DisposeAfterUse::Flag disposeAfterUse = DisposeAfterUse::YES);
	~LoopingAudioStream();

	int readBuffer(int16 *buffer, const int numSamples);
	bool endOfData() const;

	bool isStereo() const { return _parent->isStereo(); }
	int getRate() const { return _parent->getRate(); }

	/**
	 * Returns number of loops the stream has played.
	 * @param numLoops number of loops to play, 0 - infinite
	 */
	uint getCompleteIterations() const { return _completeIterations; }
private:
	RewindableAudioStream *_parent;
	DisposeAfterUse::Flag _disposeAfterUse;

	uint _loops;
	uint _completeIterations;
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
AudioStream *makeLoopingAudioStream(RewindableAudioStream *stream, uint loops);

} // End of namespace Sound

#endif

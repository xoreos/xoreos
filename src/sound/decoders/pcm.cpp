/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

// Largely based on the PCM implementation found in ScummVM.

/** @file sound/decoders/pcm.cpp
 *  Decoding PCM (Pulse Code Modulation).
 */

#include "common/stream.h"

#include "sound/audiostream.h"
#include "sound/decoders/pcm.h"

namespace Sound {

// This used to be an inline template function, but
// buggy template function handling in MSVC6 forced
// us to go with the macro approach. So far this is
// the only template function that MSVC6 seemed to
// compile incorrectly. Knock on wood.
#define READ_ENDIAN_SAMPLE(is16Bit, isUnsigned, ptr, isLE) \
	((is16Bit ? (isLE ? _stream->readUint16LE() : _stream->readUint16BE()) : (_stream->readByte() << 8)) ^ (isUnsigned ? 0x8000 : 0))


/**
 * This is a stream, which allows for playing raw PCM data from a stream.
 * It also features playback of multiple blocks from a given stream.
 */
template<bool is16Bit, bool isUnsigned, bool isLE>
class PCMStream : public RewindableAudioStream {

protected:
	const int _rate;                               ///< Sample rate of stream
	const bool _isStereo;                          ///< Whether this is an stereo stream

	Common::SeekableReadStream *_stream;           ///< Stream to read data from
	const DisposeAfterUse::Flag _disposeAfterUse;  ///< Indicates whether the stream object should be deleted when this RawStream is destructed

public:
	PCMStream(int rate, bool stereo, DisposeAfterUse::Flag disposeStream, Common::SeekableReadStream *stream)
		: _rate(rate), _isStereo(stereo), _stream(stream), _disposeAfterUse(disposeStream) {

	}

	virtual ~PCMStream() {
		if (_disposeAfterUse == DisposeAfterUse::YES)
			delete _stream;
	}

	int readBuffer(int16 *buffer, const int numSamples);
	bool isStereo() const           { return _isStereo; }
	bool endOfData() const          { return _stream->pos() >= _stream->size(); }
	int getRate() const         { return _rate; }
	bool rewind();
};

template<bool is16Bit, bool isUnsigned, bool isLE>
int PCMStream<is16Bit, isUnsigned, isLE>::readBuffer(int16 *buffer, const int numSamples) {
	int samples = numSamples;

	while (samples > 0 && !endOfData())
		*buffer++ = READ_ENDIAN_SAMPLE(is16Bit, isUnsigned, _ptr, isLE);

	return numSamples - samples;
}

template<bool is16Bit, bool isUnsigned, bool isLE>
bool PCMStream<is16Bit, isUnsigned, isLE>::rewind() {
	// Easy peasy, lemon squeezee
	_stream->seek(0);
	return true;
}

/* In the following, we use preprocessor / macro tricks to simplify the code
 * which instantiates the input streams. We used to use template functions for
 * this, but MSVC6 / EVC 3-4 (used for WinCE builds) are extremely buggy when it
 * comes to this feature of C++... so as a compromise we use macros to cut down
 * on the (source) code duplication a bit.
 * So while normally macro tricks are said to make maintenance harder, in this
 * particular case it should actually help it :-)
 */

#define MAKE_RAW_STREAM(UNSIGNED) \
		if (is16Bit) { \
			if (isLE) \
				return new PCMStream<true, UNSIGNED, true>(rate, isStereo, disposeAfterUse, stream); \
			else  \
				return new PCMStream<true, UNSIGNED, false>(rate, isStereo, disposeAfterUse, stream); \
		} else \
			return new PCMStream<false, UNSIGNED, false>(rate, isStereo, disposeAfterUse, stream)


RewindableAudioStream *makePCMStream(Common::SeekableReadStream *stream,
                                   int rate, byte flags,
                                   DisposeAfterUse::Flag disposeAfterUse) {


	const bool isStereo   = (flags & FLAG_STEREO) != 0;
	const bool is16Bit    = (flags & FLAG_16BITS) != 0;
	const bool isUnsigned = (flags & FLAG_UNSIGNED) != 0;
	const bool isLE       = (flags & FLAG_LITTLE_ENDIAN) != 0;

	if (isUnsigned) {
		MAKE_RAW_STREAM(true);
	} else {
		MAKE_RAW_STREAM(false);
	}
}

} // End of namespace Sound

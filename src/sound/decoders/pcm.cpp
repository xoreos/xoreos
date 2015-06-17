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

// Largely based on the PCM implementation found in ScummVM.

/** @file
 *  Decoding PCM (Pulse Code Modulation).
 */

#include "src/common/readstream.h"

#include "src/sound/audiostream.h"
#include "src/sound/decoders/pcm.h"

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
	const int _rate;                     ///< Sample rate of stream
	const int _channels;                 ///< Amount of channels

	Common::SeekableReadStream *_stream; ///< Stream to read data from
	const bool _disposeAfterUse;         ///< Indicates whether the stream object should be deleted when this RawStream is destructed

public:
	PCMStream(int rate, int channels, bool disposeStream, Common::SeekableReadStream *stream)
		: _rate(rate), _channels(channels), _stream(stream), _disposeAfterUse(disposeStream) {

	}

	virtual ~PCMStream() {
		if (_disposeAfterUse)
			delete _stream;
	}

	size_t readBuffer(int16 *buffer, const size_t numSamples);
	int getChannels() const           { return _channels; }
	bool endOfData() const          { return _stream->pos() >= _stream->size(); }
	int getRate() const         { return _rate; }
	bool rewind();
};

template<bool is16Bit, bool isUnsigned, bool isLE>
size_t PCMStream<is16Bit, isUnsigned, isLE>::readBuffer(int16 *buffer, const size_t numSamples) {
	size_t samples = numSamples;

	while (samples > 0 && !endOfData()) {
		*buffer++ = READ_ENDIAN_SAMPLE(is16Bit, isUnsigned, _ptr, isLE);
		samples--;
	}

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
			return new PCMStream<true, UNSIGNED, true>(rate, channels, disposeAfterUse, stream); \
		else  \
			return new PCMStream<true, UNSIGNED, false>(rate, channels, disposeAfterUse, stream); \
	} else \
		return new PCMStream<false, UNSIGNED, false>(rate, channels, disposeAfterUse, stream)


RewindableAudioStream *makePCMStream(Common::SeekableReadStream *stream,
                                   int rate, byte flags, int channels,
                                   bool disposeAfterUse) {

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

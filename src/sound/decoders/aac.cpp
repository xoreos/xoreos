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

// Largely based on the AAC implementation found in ScummVM.

/** @file
 *  Decoding AAC.
 */

#include <cstring>

#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/sound/audiostream.h"
#include "src/sound/decoders/aac.h"
#include "src/sound/decoders/codec.h"
#include "src/sound/decoders/pcm.h"

#include <neaacdec.h>

namespace Sound {

class AACDecoder : public Codec {
public:
	AACDecoder(Common::SeekableReadStream *extraData,
	           bool disposeExtraData);
	~AACDecoder();

	AudioStream *decodeFrame(Common::SeekableReadStream &stream);

private:
	NeAACDecHandle _handle;
	byte _channels;
	unsigned long _rate;
};

AACDecoder::AACDecoder(Common::SeekableReadStream *extraData, bool disposeExtraData) {
	// Open the library
	_handle = NeAACDecOpen();

	// Configure the library to our needs
	NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(_handle);
	conf->outputFormat = FAAD_FMT_16BIT; // We only support 16bit audio
	conf->downMatrix = 1;                // Convert from 5.1 to stereo if required
	NeAACDecSetConfiguration(_handle, conf);

	// Copy the extra data to a buffer
	extraData->seek(0);
	byte *extraDataBuf = new byte[extraData->size()];
	extraData->read(extraDataBuf, extraData->size());

	// Initialize with our extra data
	// NOTE: This code assumes the extra data is coming from an MPEG-4 file!
	int err = NeAACDecInit2(_handle, extraDataBuf, extraData->size(), &_rate, &_channels);
	delete[] extraDataBuf;

	if (err < 0) {
		NeAACDecClose(_handle);

		throw Common::Exception("Could not initialize AAC decoder: %s", NeAACDecGetErrorMessage(err));
	}

	if (disposeExtraData)
		delete extraData;
}

AACDecoder::~AACDecoder() {
	NeAACDecClose(_handle);
}

AudioStream *AACDecoder::decodeFrame(Common::SeekableReadStream &stream) {
	// read everything into a buffer
	size_t inBufferPos = 0;
	size_t inBufferSize = stream.size();
	byte *inBuffer = new byte[inBufferSize];
	stream.read(inBuffer, inBufferSize);

	QueuingAudioStream *audioStream = makeQueuingAudioStream(_rate, _channels);

	// Decode until we have enough samples (or there's no more left)
	while (inBufferPos < inBufferSize) {
		NeAACDecFrameInfo frameInfo;
		void *decodedSamples = NeAACDecDecode(_handle, &frameInfo, inBuffer + inBufferPos, inBufferSize - inBufferPos);

		if (frameInfo.error != 0)
			throw Common::Exception("Failed to decode AAC frame: %s", NeAACDecGetErrorMessage(frameInfo.error));

		byte *buffer = new byte[frameInfo.samples * 2];
		memcpy(buffer, decodedSamples, frameInfo.samples * 2);

		byte flags = FLAG_16BITS;

#ifdef XOREOS_LITTLE_ENDIAN
		flags |= FLAG_LITTLE_ENDIAN;
#endif

		audioStream->queueAudioStream(makePCMStream(new Common::MemoryReadStream(buffer, frameInfo.samples * 2, true), _rate, flags, _channels, true), true);

		inBufferPos += frameInfo.bytesconsumed;
	}

	return audioStream;
}

// Factory function
Codec *makeAACDecoder(Common::SeekableReadStream *extraData, bool disposeExtraData) {
	return new AACDecoder(extraData, disposeExtraData);
}

} // End of namespace Sound

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
 *  Decoding AAC.
 */

#include <cstring>

#include <memory>

#include <neaacdec.h>

#include <boost/scoped_array.hpp>

#include "src/common/disposableptr.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/sound/audiostream.h"

#include "src/sound/decoders/aac.h"
#include "src/sound/decoders/pcm.h"

namespace Sound {

class AACDecoder : public PacketizedAudioStream {
public:
	AACDecoder(Common::SeekableReadStream &stream);
	~AACDecoder();

	// AudioStream API
	int getChannels() const { return _channels; }
	int getRate() const { return _rate; }
	bool endOfData() const { return _audStream->endOfData(); }
	bool endOfStream() const { return _audStream->endOfStream(); }
	size_t readBuffer(int16_t *buffer, const size_t numSamples) { return _audStream->readBuffer(buffer, numSamples); }

	// PacketizedAudioStream API
	void finish() { _audStream->finish(); }
	bool isFinished() const { return _audStream->isFinished(); }
	void queuePacket(Common::SeekableReadStream *data);

private:
	NeAACDecHandle _handle;
	byte _channels;
	unsigned long _rate;

	// Backing stream for PacketizedAudioStream
	std::unique_ptr<QueuingAudioStream> _audStream;

	AudioStream *decodeFrame(Common::SeekableReadStream &stream);
};

AACDecoder::AACDecoder(Common::SeekableReadStream &extraData) {
	// Open the library
	_handle = NeAACDecOpen();

	// Configure the library to our needs
	NeAACDecConfigurationPtr conf = NeAACDecGetCurrentConfiguration(_handle);
	conf->outputFormat = FAAD_FMT_16BIT; // We only support 16bit audio
	conf->downMatrix = 1;                // Convert from 5.1 to stereo if required
	NeAACDecSetConfiguration(_handle, conf);

	// Copy the extra data to a buffer
	extraData.seek(0);
	std::unique_ptr<byte[]> extraDataBuf = std::make_unique<byte[]>(extraData.size());
	extraData.read(extraDataBuf.get(), extraData.size());

	// Initialize with our extra data
	// NOTE: This code assumes the extra data is coming from an MPEG-4 file!
	int err = NeAACDecInit2(_handle, extraDataBuf.get(), extraData.size(), &_rate, &_channels);
	if (err < 0) {
		NeAACDecClose(_handle);

		throw Common::Exception("Could not initialize AAC decoder: %s", NeAACDecGetErrorMessage(err));
	}

	_audStream.reset(makeQueuingAudioStream(_rate, _channels));
}

AACDecoder::~AACDecoder() {
	NeAACDecClose(_handle);
}

AudioStream *AACDecoder::decodeFrame(Common::SeekableReadStream &stream) {
	// read everything into a buffer
	size_t inBufferPos = 0;
	size_t inBufferSize = stream.size();

	boost::scoped_array<byte> inBuffer(new byte[inBufferSize]);
	if (stream.read(inBuffer.get(), inBufferSize) != inBufferSize)
		throw Common::Exception(Common::kReadError);

	QueuingAudioStream *audioStream = makeQueuingAudioStream(_rate, _channels);

	// Decode until we have enough samples (or there's no more left)
	while (inBufferPos < inBufferSize) {
		NeAACDecFrameInfo frameInfo;
		void *decodedSamples = NeAACDecDecode(_handle, &frameInfo, inBuffer.get() + inBufferPos, inBufferSize - inBufferPos);

		if (frameInfo.error != 0)
			throw Common::Exception("Failed to decode AAC frame: %s", NeAACDecGetErrorMessage(frameInfo.error));

		byte *buffer = new byte[frameInfo.samples * 2];
		std::memcpy(buffer, decodedSamples, frameInfo.samples * 2);

		static const byte flags = FLAG_16BITS | FLAG_NATIVE_ENDIAN;

		audioStream->queueAudioStream(makePCMStream(new Common::MemoryReadStream(buffer, frameInfo.samples * 2, true), _rate, flags, _channels, true), true);

		inBufferPos += frameInfo.bytesconsumed;
	}

	audioStream->finish();
	return audioStream;
}

void AACDecoder::queuePacket(Common::SeekableReadStream *data) {
	std::unique_ptr<Common::SeekableReadStream> capture(data);
	_audStream->queueAudioStream(decodeFrame(*data));
}

PacketizedAudioStream *makeAACStream(Common::SeekableReadStream &extraData) {
	return new AACDecoder(extraData);
}

} // End of namespace Sound

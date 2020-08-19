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
 *  Decoding RIFF WAVE (Resource Interchange File Format Waveform).
 */

#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/readstream.h"

#include "src/sound/audiostream.h"
#include "src/sound/decoders/wave.h"
#include "src/sound/decoders/adpcm.h"
#include "src/sound/decoders/pcm.h"
#include "src/sound/decoders/wave_types.h"

namespace Sound {

RewindableAudioStream *makeWAVStream(Common::SeekableReadStream *stream, bool disposeAfterUse) {
	uint32_t riffTag = stream->readUint32BE();
	if (riffTag != MKTAG('R', 'I', 'F', 'F'))
		throw Common::Exception("makeWAVStream(): No 'RIFF' header (%s)", Common::debugTag(riffTag).c_str());

	/* uint32_t fileSize = */ stream->readUint32LE();

	uint32_t waveTag = stream->readUint32BE();
	if (waveTag != MKTAG('W', 'A', 'V', 'E'))
		throw Common::Exception("makeWAVStream(): No 'WAVE' RIFF type (%s)", Common::debugTag(waveTag).c_str());

	uint32_t fmtTag = stream->readUint32BE();
	if (fmtTag != MKTAG('f', 'm', 't', ' '))
		throw Common::Exception("makeWAVStream(): No 'fmt ' chunk (%s)", Common::debugTag(fmtTag).c_str());

	uint32_t fmtLength = stream->readUint32LE();
	if (fmtLength < 16) // A valid fmt chunk always contains at least 16 bytes
		throw Common::Exception("makeWAVStream(): Invalid wave format size %d", fmtLength);

	// Now parse the WAVEFORMAT(EX) structure
	uint16_t compression = stream->readUint16LE();
	uint16_t channels = stream->readUint16LE();
	uint32_t sampleRate = stream->readUint32LE();
	/* uint32_t avgBytesPerSecond = */ stream->readUint32LE();
	uint16_t blockAlign = stream->readUint16LE();
	uint16_t bitsPerSample = stream->readUint16LE();

	// Skip over the rest of the fmt chunk.
	stream->skip(fmtLength - 16);

	// Skip over all chunks until we hit the data
	for (;;) {
		if (stream->readUint32BE() == MKTAG('d', 'a', 't', 'a'))
			break;

		if (stream->eos())
			throw Common::Exception("makeWAVStream(): Unexpected eos");

		stream->skip(stream->readUint32LE());
	}

	uint32_t size = stream->readUint32LE();
	Common::SeekableSubReadStream *subStream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->pos() + size, disposeAfterUse);

	// Return the decoder we need
	switch (compression) {
	case kWavePCM: {
		byte flags = 0;

		// 8 bit data is unsigned, 16 bit data signed
		if (bitsPerSample == 8)
			flags |= FLAG_UNSIGNED;
		else if (bitsPerSample == 16)
			flags |= (FLAG_16BITS | FLAG_LITTLE_ENDIAN);
		else
			throw Common::Exception("makeWAVStream(): Unsupported PCM bits per sample %d", bitsPerSample);

		return makePCMStream(subStream, sampleRate, flags, channels, true);
	}
	case kWaveMSIMAADPCM:
	case kWaveMSIMAADPCM2:
		return makeADPCMStream(subStream, true, size, kADPCMMSIma, sampleRate, channels, blockAlign);
	case kWaveMSADPCM:
		return makeADPCMStream(subStream, true, size, kADPCMMS, sampleRate, channels, blockAlign);
	}

	throw Common::Exception("makeWAVStream(): Unhandled wave type 0x%04x", compression);
	return 0;
}

} // End of namespace Sound

/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

// Largely based on the Wave implementation found in ScummVM.

/** @file sound/decoders/wave.cpp
 *  Decoding RIFF WAVE (Resource Interchange File Format Waveform).
 */

#include "common/util.h"
#include "common/stream.h"

#include "sound/audiostream.h"
#include "sound/decoders/wave.h"
#include "sound/decoders/adpcm.h"
#include "sound/decoders/pcm.h"

namespace Sound {

bool loadWAVFromStream(Common::SeekableReadStream &stream, int &size, int &rate, byte &flags, uint16 *wavType, int *blockAlign_) {
	const int32 initialPos = stream.pos();
	byte buf[4+1];

	buf[4] = 0;

	stream.read(buf, 4);
	if (memcmp(buf, "RIFF", 4) != 0) {
		warning("getWavInfo: No 'RIFF' header");
		return false;
	}

	int32 wavLength = stream.readUint32LE();

	stream.read(buf, 4);
	if (memcmp(buf, "WAVE", 4) != 0) {
		warning("getWavInfo: No 'WAVE' header");
		return false;
	}

	stream.read(buf, 4);
	if (memcmp(buf, "fmt ", 4) != 0) {
		warning("getWavInfo: No 'fmt' header");
		return false;
	}

	uint32 fmtLength = stream.readUint32LE();
	if (fmtLength < 16) {
		// A valid fmt chunk always contains at least 16 bytes
		warning("getWavInfo: 'fmt' header is too short");
		return false;
	}

	// Next comes the "type" field of the fmt header. Some typical
	// values for it:
	// 1  -> uncompressed PCM
	// 2  -> MS ADPCM compressed WAVE
	// 17 -> MS IMA ADPCM compressed WAVE
	// See <http://www.saettler.com/RIFFNEW/RIFFNEW.htm> for a more complete
	// list of common WAVE compression formats...
	uint16 type = stream.readUint16LE();
	uint16 numChannels = stream.readUint16LE();	// 1 for mono, 2 for stereo
	uint32 samplesPerSec = stream.readUint32LE();	// in Hz
	/* uint32 avgBytesPerSec = */ stream.readUint32LE();

	uint16 blockAlign = stream.readUint16LE();	// == NumChannels * BitsPerSample/8
	uint16 bitsPerSample = stream.readUint16LE();	// 8, 16 ...
	// 8 bit data is unsigned, 16 bit data signed


	if (wavType != 0)
		*wavType = type;

	if (blockAlign_ != 0)
		*blockAlign_ = blockAlign;
#if 0
	printf("WAVE information:\n");
	printf("  total size: %d\n", wavLength);
	printf("  fmt size: %d\n", fmtLength);
	printf("  type: %d\n", type);
	printf("  numChannels: %d\n", numChannels);
	printf("  samplesPerSec: %d\n", samplesPerSec);
	printf("  avgBytesPerSec: %d\n", avgBytesPerSec);
	printf("  blockAlign: %d\n", blockAlign);
	printf("  bitsPerSample: %d\n", bitsPerSample);
#endif

	if (type != 1 && type != 2 && type != 17) {
		warning("getWavInfo: only PCM, MS ADPCM or IMA ADPCM data is supported (type %d)", type);
		return false;
	}

	// Prepare the return values.
	rate = samplesPerSec;

	flags = 0;

	if (bitsPerSample == 8)		// 8 bit data is unsigned
		flags |= FLAG_UNSIGNED;
	else if (bitsPerSample == 16)	// 16 bit data is signed little endian
		flags |= (FLAG_16BITS | FLAG_LITTLE_ENDIAN);
	else if (bitsPerSample == 4 && (type == 2 || type == 17))
		flags |= FLAG_16BITS;
	else {
		warning("getWavInfo: unsupported bitsPerSample %d", bitsPerSample);
		return false;
	}

	if (numChannels == 2)
		flags |= FLAG_STEREO;
	else if (numChannels != 1) {
		warning("getWavInfo: unsupported number of channels %d", numChannels);
		return false;
	}

	// It's almost certainly a WAV file, but we still need to find its
	// 'data' chunk.

	// Skip over the rest of the fmt chunk.
	int offset = fmtLength - 16;

	do {
		stream.seek(offset, SEEK_CUR);
		if (stream.pos() >= initialPos + wavLength + 8) {
			warning("getWavInfo: Can't find 'data' chunk");
			return false;
		}
		stream.read(buf, 4);
		offset = stream.readUint32LE();
	} while (memcmp(buf, "data", 4) != 0);

	// Stream now points at 'offset' bytes of sample data...
	size = offset;

	return true;
}

RewindableAudioStream *makeWAVStream(Common::SeekableReadStream *stream, bool disposeAfterUse) {
	int size, rate;
	byte flags;
	uint16 type;
	int blockAlign;

	if (!loadWAVFromStream(*stream, size, rate, flags, &type, &blockAlign)) {
		if (disposeAfterUse)
			delete stream;
		return 0;
	}

	Common::SeekableSubReadStream *subStream = new Common::SeekableSubReadStream(stream, stream->pos(), stream->pos() + size, disposeAfterUse);

	if (type == 17) // MS IMA ADPCM
		return makeADPCMStream(subStream, true, size, kADPCMMSIma, rate, (flags & FLAG_STEREO) ? 2 : 1, blockAlign);
	else if (type == 2) // MS ADPCM
		return makeADPCMStream(subStream, true, size, kADPCMMS, rate, (flags & FLAG_STEREO) ? 2 : 1, blockAlign);

	// Raw PCM
	return makePCMStream(subStream, rate, flags, true);
}

} // End of namespace Sound

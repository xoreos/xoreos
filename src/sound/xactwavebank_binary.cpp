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
 *  A binary XACT WaveBank, found in the Xbox version of Jade Empire as XWB files.
 */

/* Based heavily on Luigi Auriemma's unxwb tool
 * (<http://aluigi.altervista.org/papers.htm#xbox>), which is licensed
 * under the terms of the GPLv2.
 *
 * The original copyright note in unxwb reads as follows:
 *
 * Copyright 2005-2016 Luigi Auriemma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 * http://www.gnu.org/licenses/gpl-2.0.txt
 *
 * people who supplied xwb files to analyze [to Luigi]: john deo, antti
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/memreadstream.h"

#include "src/sound/xactwavebank_binary.h"

#include "src/sound/decoders/pcm.h"
#include "src/sound/decoders/adpcm.h"
#include "src/sound/decoders/asf.h"

namespace Sound {

enum XWBFlags {
	kXWBFlagsStreaming    = 0x00000001,
	kXWBFlagsEntryNames   = 0x00010000,
	kXWBFlagsCompact      = 0x00020000,
	kXWBFlagsSyncDisabled = 0x00040000,
	kXWBFlagsSeekTables   = 0x00080000
};

enum WaveFlags {
	kWaveFlagsReadAhead      = 0x00000001, ///< Performance hint: read ahead while streaming.
	kWaveFlagsLoopCache      = 0x00000002, ///< Audio file is used by at least one looping sound.
	kWaveFlagsRemoveLoopTail = 0x00000004, ///< Ignore the data after the looping section.
	kWaveFlagsIgnoreLoop     = 0x00000008  ///< Don't loop this sound.
};

XACTWaveBank_Binary::XACTWaveBank_Binary(Common::SeekableReadStream *xwb) : _xwb(xwb) {
	assert(_xwb);

	load(*_xwb);
}

bool XACTWaveBank_Binary::isStreaming() const {
	return (_flags & kXWBFlagsStreaming) != 0;
}

size_t XACTWaveBank_Binary::getWaveCount() const {
	return _waves.size();
}

RewindableAudioStream *XACTWaveBank_Binary::getWave(size_t index) const {
	if (index >= _waves.size())
		throw Common::Exception("XACTWaveBank_Binary::getWave(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_waves.size()).c_str());

	const Wave &wave = _waves[index];

	_xwb->seek(wave.offset);
	Common::ScopedPtr<Common::SeekableReadStream> dataStream(_xwb->readStream(wave.size));

	switch (wave.codec) {
		case kCodecPCM:
			return makePCMStream(dataStream.release(), wave.samplingRate,
			                     (wave.bitRate == 16) ? (FLAG_16BITS | FLAG_LITTLE_ENDIAN) : FLAG_UNSIGNED,
			                     wave.channels);

		case kCodecADPCM:
			return makeADPCMStream(dataStream.release(), true, dataStream->size(),
			                       kADPCMXbox, wave.samplingRate,  wave.channels);

		case kCodecWMA:
			return makeASFStream(dataStream.release());

		default:
			throw Common::Exception("XACTWaveBank_Binary::getWave(): Unknown encoding %u",
			                        static_cast<uint>(wave.codec));
	}
}


enum Segments {
	kSegmentBankData      = 0,
	kSegmentEntryMetaData    ,
	kSegmentEntryNames       ,
	kSegmentWaveData         ,

	kSegmentMAX
};

struct Segment {
	size_t offset;
	size_t size;
};

void XACTWaveBank_Binary::load(Common::SeekableReadStream &xwb) {
	static const uint32 kXWBID = MKTAG('W', 'B', 'N', 'D');

	const uint32 id = xwb.readUint32BE();
	if (id != kXWBID)
		throw Common::Exception("Not a XWB file (%s)", Common::debugTag(id).c_str());

	const uint32 version = xwb.readUint32LE();
	if (version != 3)
		throw Common::Exception("Unsupported XWB file version %u", version);

	Segment segments[kSegmentMAX];

	for (size_t i = 0; i < kSegmentMAX; i++) {
		segments[i].offset = xwb.readUint32LE();
		segments[i].size   = xwb.readUint32LE();
	}

	xwb.seek(segments[kSegmentBankData].offset);

	_flags = xwb.readUint32LE();
	if (_flags & kXWBFlagsCompact)
		throw Common::Exception("XACTWaveBank_Binary::load(): TODO: Compact format");

	const size_t waveCount = xwb.readUint32LE();

	_name = Common::readStringFixed(xwb, Common::kEncodingASCII, 16);

	const size_t waveMetaSize = xwb.readUint32LE();
	if (waveMetaSize < 24)
		throw Common::Exception("XACTWaveBank_Binary::load(): Wave meta data size too small (%s)",
		                        Common::composeString(waveMetaSize).c_str());

	xwb.skip(4); // Size of a wave name
	xwb.skip(4); // Alignment

	uint32 indexOffset = segments[kSegmentEntryMetaData].offset;

	uint32 dataOffset = segments[kSegmentWaveData].offset;
	if (dataOffset == 0)
		dataOffset = indexOffset + waveCount * waveMetaSize;

	_waves.resize(waveCount);
	for (std::vector<Wave>::iterator w = _waves.begin(); w != _waves.end(); ++w) {
		xwb.seek(indexOffset);

		w->flags = xwb.readUint32LE();

		const uint32 formatCode = xwb.readUint32LE();

		w->offset = xwb.readUint32LE() + dataOffset;
		w->size   = xwb.readUint32LE();

		w->loopOffset = xwb.readUint32LE();
		w->loopLength = xwb.readUint32LE();

		w->codec        = static_cast<Codec>(((formatCode      ) & ((1 <<  2) - 1)));
		w->channels     =                     (formatCode >>  2) & ((1 <<  3) - 1);
		w->samplingRate =                     (formatCode >>  5) & ((1 << 18) - 1);
		w->blockAlign   =                     (formatCode >> 23) & ((1 <<  8) - 1);
		w->bitRate      =                    ((formatCode >> 31) & ((1 <<  1) - 1)) ? 16 : 8;

		switch (w->codec) {
			case kCodecPCM:
			case kCodecADPCM:
			case kCodecWMA:
				break;

			default:
				throw Common::Exception("XACTWaveBank_Binary::load(): Unknown encoding %u",
				                        static_cast<uint>(w->codec));
		}

		indexOffset += waveMetaSize;
	}
}

} // End of namespace Sound

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

static constexpr uint32_t kXWBFlagsStreaming    = 0x00000001;
static constexpr uint32_t kXWBFlagsEntryNames   = 0x00010000;
static constexpr uint32_t kXWBFlagsCompact      = 0x00020000;
static constexpr uint32_t kXWBFlagsSyncDisabled = 0x00040000;
static constexpr uint32_t kXWBFlagsSeekTables   = 0x00080000;

static constexpr uint32_t kWaveFlagsReadAhead      = 0x00000001; ///< Performance hint: read ahead while streaming.
static constexpr uint32_t kWaveFlagsLoopCache      = 0x00000002; ///< Audio file is used by at least one looping sound.
static constexpr uint32_t kWaveFlagsRemoveLoopTail = 0x00000004; ///< Ignore the data after the looping section.
static constexpr uint32_t kWaveFlagsIgnoreLoop     = 0x00000008; ///< Don't loop this sound.

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
	std::unique_ptr<Common::SeekableReadStream> dataStream(_xwb->readStream(wave.size));

	switch (wave.codec) {
		case Codec::PCM:
			return makePCMStream(dataStream.release(), wave.samplingRate,
			                     (wave.bitRate == 16) ? (FLAG_16BITS | FLAG_LITTLE_ENDIAN) : FLAG_UNSIGNED,
			                     wave.channels);

		case Codec::ADPCM:
			return makeADPCMStream(dataStream.release(), true, dataStream->size(),
			                       kADPCMXbox, wave.samplingRate,  wave.channels);

		case Codec::WMA:
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
	static constexpr uint32_t kXWBID = MKTAG('W', 'B', 'N', 'D');

	const uint32_t id = xwb.readUint32BE();
	if (id != kXWBID)
		throw Common::Exception("Not a XWB file (%s)", Common::debugTag(id).c_str());

	const uint32_t version = xwb.readUint32LE();
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

	uint32_t indexOffset = segments[kSegmentEntryMetaData].offset;

	uint32_t dataOffset = segments[kSegmentWaveData].offset;
	if (dataOffset == 0)
		dataOffset = indexOffset + waveCount * waveMetaSize;

	_waves.resize(waveCount);
	for (auto &wave : _waves) {
		xwb.seek(indexOffset);

		wave.flags = xwb.readUint32LE();

		const uint32_t formatCode = xwb.readUint32LE();

		wave.offset = xwb.readUint32LE() + dataOffset;
		wave.size   = xwb.readUint32LE();

		wave.loopOffset = xwb.readUint32LE();
		wave.loopLength = xwb.readUint32LE();

		wave.codec        = static_cast<Codec>(((formatCode      ) & ((1 <<  2) - 1)));
		wave.channels     =                     (formatCode >>  2) & ((1 <<  3) - 1);
		wave.samplingRate =                     (formatCode >>  5) & ((1 << 18) - 1);
		wave.blockAlign   =                     (formatCode >> 23) & ((1 <<  8) - 1);
		wave.bitRate      =                    ((formatCode >> 31) & ((1 <<  1) - 1)) ? 16 : 8;

		switch (wave.codec) {
			case Codec::PCM:
			case Codec::ADPCM:
			case Codec::WMA:
				break;

			default:
				throw Common::Exception("XACTWaveBank_Binary::load(): Unknown encoding %u",
				                        static_cast<uint>(wave.codec));
		}

		indexOffset += waveMetaSize;
	}
}

} // End of namespace Sound

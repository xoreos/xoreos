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
 *  An FMOD SampleBank, found in Dragon Age: Origins as FSB files.
 */

/* Based heavily on Luigi Auriemma's fsbext tool
 * (<http://aluigi.altervista.org/papers.htm#others-file>), which is licensed
 * under the terms of the GPLv2.
 *
 * The original copyright note in fsbext reads as follows:
 *
 *  Copyright 2005-2012 Luigi Auriemma
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *  http://www.gnu.org/licenses/gpl-2.0.txt
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"

#include "src/aurora/resman.h"

#include "src/sound/fmodsamplebank.h"

#ifdef ENABLE_MAD
#include "src/sound/decoders/mp3.h"
#endif
#include "src/sound/decoders/adpcm.h"

namespace Sound {

FMODSampleBank::FMODSampleBank(Common::SeekableReadStream *fsb) : _fsb(fsb) {
	assert(_fsb);

	load(*_fsb);
}

FMODSampleBank::FMODSampleBank(const Common::UString &name) {
	_fsb.reset(ResMan.getResource(name, Aurora::kFileTypeFSB));
	if (!_fsb)
		throw Common::Exception("No such FSB resource \"%s\"", name.c_str());

	load(*_fsb);
}

size_t FMODSampleBank::getSampleCount() const {
	return _samples.size();
}

const Common::UString &FMODSampleBank::getSampleName(size_t index) const {
	if (index >= _samples.size())
		throw Common::Exception("FMODSampleBank::getSampleName(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_samples.size()).c_str());

	return _samples[index].name;
}

bool FMODSampleBank::hasSample(const Common::UString &name) const {
	return _sampleMap.find(name) != _sampleMap.end();
}

static constexpr uint32_t kSampleFlagMP3      = 0x00000200;
static constexpr uint32_t kSampleFlagIMAADPCM = 0x00400000;

RewindableAudioStream *FMODSampleBank::getSample(const Sample &sample) const {
	_fsb->seek(sample.offset);
	std::unique_ptr<Common::SeekableReadStream> dataStream(_fsb->readStream(sample.size));

	if (sample.flags & kSampleFlagMP3) {
		warning("MP3");
#ifdef ENABLE_MAD
		return makeMP3Stream(dataStream.release(), true);
#else
		throw Common::Exception("MP3 decoding disabled when building without libmad");
#endif
	}

	if (sample.flags & kSampleFlagIMAADPCM) {
		warning("APCM");
		return makeADPCMStream(dataStream.release(), true, dataStream->size(),
		                       kADPCMMSIma, sample.defFreq, sample.channels, 36 * sample.channels);
	}

	throw Common::Exception("FMODSampleBank::getSample(): Unknown format (0x%08X)", sample.flags);
}

RewindableAudioStream *FMODSampleBank::getSample(size_t index) const {
	if (index >= _samples.size())
		throw Common::Exception("FMODSampleBank::getSampleName(): Index out of range (%s >= %s)",
		                        Common::composeString(index).c_str(),
		                        Common::composeString(_samples.size()).c_str());

	return getSample(_samples[index]);
}

RewindableAudioStream *FMODSampleBank::getSample(const Common::UString &name) const {
	std::map<Common::UString, const Sample *>::const_iterator s = _sampleMap.find(name);
	if (s == _sampleMap.end())
		throw Common::Exception("FMODSampleBank::getSampleName(): No such sample \"%s\"", name.c_str());

	return getSample(*s->second);
}

static constexpr uint32_t kHeaderFlagSimpleInfo = 0x00000002;

void FMODSampleBank::load(Common::SeekableReadStream &fsb) {
	static constexpr uint32_t kFSBID = MKTAG('F', 'S', 'B', '4');

	const uint32_t id = fsb.readUint32BE();
	if (id != kFSBID)
		throw Common::Exception("Not a FSB file (%s)", Common::debugTag(id).c_str());

	const size_t sampleCount = fsb.readUint32LE();

	const size_t sampleInfoSize = fsb.readUint32LE();
	fsb.skip(4); // sampleDataSize

	fsb.skip(4); // version
	const uint32_t flags   = fsb.readUint32LE();

	fsb.skip(24); // Unknown

	const size_t offsetInfo = 48;
	size_t offsetData = offsetInfo + sampleInfoSize;

	fsb.seek(offsetInfo);

	_samples.resize(sampleCount);
	for (auto &sample : _samples) {
		const bool isSimple = (flags & kHeaderFlagSimpleInfo) && (&sample != &_samples.front());

		if (isSimple) {
			sample = _samples[0];

			sample.name.clear();

			sample.length = fsb.readUint32LE();
			sample.size   = fsb.readUint32LE();

		} else {
			const size_t infoSize = fsb.readUint16LE();

			if (infoSize < 80)
				throw Common::Exception("FMODSampleBank::load(): Invalid sample info size %s",
				                        Common::composeString(infoSize).c_str());

			sample.name = Common::readStringFixed(fsb, Common::kEncodingASCII, 30);

			sample.length = fsb.readUint32LE();
			sample.size   = fsb.readUint32LE();

			sample.loopStart = fsb.readUint32LE();
			sample.loopEnd   = fsb.readUint32LE();

			sample.flags = fsb.readUint32LE();

			sample.defFreq = fsb.readSint32LE();
			sample.defVol  = fsb.readUint16LE();
			sample.defPan  = fsb.readSint16LE();
			sample.defPri  = fsb.readUint16LE();

			sample.channels = fsb.readUint16LE();

			sample.minDistance = fsb.readIEEEFloatLE();
			sample.maxDistance = fsb.readIEEEFloatLE();

			sample.varFreq = fsb.readSint32LE();
			sample.varVol  = fsb.readUint16LE();
			sample.varPan  = fsb.readSint16LE();

			fsb.skip(infoSize - 80);
		}

		sample.offset   = offsetData;
		offsetData += sample.size;

		if (!sample.name.empty())
			_sampleMap.insert(std::make_pair(sample.name, &sample));
	}
}

} // End of namespace Sound

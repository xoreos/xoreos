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

#ifndef SOUND_FMODSAMPLEBANK_H
#define SOUND_FMODSAMPLEBANK_H

#include <memory>
#include <vector>
#include <map>

#include "src/common/ustring.h"
#include "src/common/readstream.h"

namespace Sound {

class RewindableAudioStream;

/** Class to hold audio resource data of an FMOD samplebank file.
 *
 *  An FSB file is a samplebank, i.e. an archive containing one or more
 *  audio files. It's part of FMOD sound middleware, and commonly used
 *  together with FEV files, which define events and effect chains.
 *
 *  FSB files are found in Dragon Age: Origins.
 *
 *  Only version 4 of the FSB format is supported, because that's the
 *  version used by Dragon Age: Origins.
 */
class FMODSampleBank {
public:
	FMODSampleBank(Common::SeekableReadStream *fsb);
	FMODSampleBank(const Common::UString &name);
	~FMODSampleBank() = default;

	/** Return the number of sample files. */
	size_t getSampleCount() const;

	/** Return the name of a sample. */
	const Common::UString &getSampleName(size_t index) const;
	/** Does a sample with this name exist in the sample bank? */
	bool hasSample(const Common::UString &name) const;

	/** Return the audio stream of a sample, by index. */
	RewindableAudioStream *getSample(size_t index) const;
	/** Return the audio stream of a sample, by name. */
	RewindableAudioStream *getSample(const Common::UString &name) const;

private:
	struct Sample {
		Common::UString name; ///< Name of the sample.

		uint32_t length; ///< Length of the sample, in audio samples.

		uint32_t loopStart;
		uint32_t loopEnd;

		uint32_t flags;

		int32_t  defFreq;
		uint16_t defVol;
		int16_t  defPan;
		uint16_t defPri;

		uint16_t channels;

		float minDistance;
		float maxDistance;

		int32_t  varFreq;
		uint16_t varVol;
		int16_t  varPan;

		size_t offset; ///< Offset to the sample within the FSB.
		size_t size;   ///< Size of the sample in bytes.
	};


	std::unique_ptr<Common::SeekableReadStream> _fsb;

	std::vector<Sample> _samples;

	std::map<Common::UString, const Sample *> _sampleMap;


	void load(Common::SeekableReadStream &fsb);

	RewindableAudioStream *getSample(const Sample &sample) const;
};

} // End of namespace Sound

#endif // SOUND_FMODSAMPLEBANK_H

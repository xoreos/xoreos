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
 *  A loader for FEV (FMOD Event) files.
 */

#ifndef AURORA_FEVFILE_H
#define AURORA_FEVFILE_H

#include "src/common/readstream.h"
#include "src/common/ustring.h"

namespace Aurora {

/**
 * An fev file is used to define events for the FMOD system
 * and categorize them.
 *
 * Theres is currently only one relevant version of fev files
 * with the FourCC "FEV1".
 */
class FEVFile {
public:
	/** Possible types of streaming for FMOD. */
	enum FEVStreamingType {
		kDecompressIntoMemory,
		kLoadIntoMemory,
		kStreamFromDisk
	};

	/** Reference to an external wave bank. */
	struct FEVWaveBank {
		uint32 maxStreams;
		FEVStreamingType streamingType;
		Common::UString name;
	};

	/** A category which is organized hierarchically. */
	struct FEVCategory {
		Common::UString name;
		uint32 volume;
		uint32 pitch;
	};

	/**
	 * An FMOD event.
	 *
	 * @note
	 * 	Most of the floating point values only represent a
	 * 	range from 0.0f to 1.0f, which is mapped to different
	 * 	dezibel values.
	 */
	struct FEVEvent {
		Common::UString name;

		float volume;
		float pitch;
		float pitchRandomization;
		float volumeRandomization;
		uint32 priority;
		uint32 mode;
		uint32 maxPlaybacks;

		float Speaker2DL;
		float Speaker2DR;
		float Speaker2DC;

		float SpeakerLFE;

		float Speaker2DLR;
		float Speaker2DRR;
		float Speaker2DLS;
		float Speaker2DRS;

		float ReverbDryLevel;
		float ReverbWetLevel;

		uint32 fadeInTime;
		uint32 fadeOutTime;

		float spawnIntensity;
		float spawnIntensityRandomization;

		Common::UString category;
	};
	
	FEVFile(const Common::UString &resRef);
	FEVFile(Common::SeekableReadStream &fev);

	const Common::UString &getBankName();

	const std::vector<FEVWaveBank> &getWaveBanks();
	const std::vector<FEVCategory> &getCategories();

private:
	void load(Common::SeekableReadStream &fev);

	/** Read a general category. */ 
	void readCategory(Common::SeekableReadStream &fev);
	/** Read a category for events. */
	void readEventCategory(Common::SeekableReadStream &fev);
	/** Read an event. */
	void readEvent(Common::SeekableReadStream &fev);

	/** Read an FEV length prefixed string. */
	Common::UString readLengthPrefixedString(Common::SeekableReadStream &fev);

	Common::UString _bankName;

	std::vector<FEVWaveBank> _waveBanks;
	std::vector<FEVCategory> _categories;
};

} // End of namespace Aurora

#endif // AURORA_FEVFILE_H

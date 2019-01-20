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

#ifndef SOUND_FEVFILE_H
#define SOUND_FEVFILE_H

#include <map>

#include <boost/variant.hpp>

#include "src/common/readstream.h"
#include "src/common/ustring.h"

namespace Sound {

/** An FEV file is used to define events for the FMOD system
 *  and categorize them.
 *
 *  There is currently only one relevant version of fev files
 *  with the FourCC "FEV1".
 */
class FEVFile {
public:
	/** If an event is 2D or 3D. */
	enum EventMode {
		k2D,
		k3D
	};

	/** The Rollof 3D type. */
	enum Rollof3DType {
		kLogarithmic,
		kLinear,
		kCustom,
		kUnspecified
	};

	/** The type of the 3D position. */
	enum Position3DType {
		kWorldRelative,
		kHeadRelative
	};

	/** Possible types of streaming for FMOD. */
	enum StreamingType {
		kDecompressIntoMemory,
		kLoadIntoMemory,
		kStreamFromDisk
	};

	/** Possible Play modes. */
	enum PlayMode {
		kSequential = 0,
		kRandom,
		kRandomNoRepeat,
		kSequentialNoRepeat,
		kShuffle,
		kProgrammerSelected
	};

	/** Possible Property types. */
	enum PropertyType {
		kPropertyInt = 0,
		kPropertyFloat,
		kPropertyString
	};

	/** Some objects in FMOD can have generic properties. */
	struct Property {
		PropertyType type;
		boost::variant<int32, float, Common::UString> value;
	};

	/** Reference to an external wave bank. */
	struct WaveBank {
		uint32 maxStreams;
		StreamingType streamingType;
		Common::UString name;
	};

	/** A category which is organized hierarchically. */
	struct Category {
		Common::UString name;
		uint32 volume;
		uint32 pitch;
	};

	/** An event category for storing events. */
	struct EventCategory {
		Common::UString name;
	};

	struct EventLayer {
		int16 priority;
	};

	/** An FMOD event.
	 *
	 *  @note
	 *  Most of the floating point values only represent a
	 *  range from 0.0f to 1.0f, which are mapped to different
	 *  decibel values.
	 */
	struct Event {
		Common::UString name;

		float volume;
		float pitch;
		float pitchRandomization;
		float volumeRandomization;
		uint32 priority;
		EventMode mode;
		uint32 maxPlaybacks;
		uint32 maxPlaybacksBehavior;

		Rollof3DType rollof3D;
		Position3DType position3D;

		uint32 positionRandomization3D;

		float coneInsideAngle3D;
		float coneOutsideAngle3D;
		float coneOutsideVolume3D; // [0, 1] -> [0, -1024]

		float dopplerFactor3D;
		float speakerSpread3D;

		float panLevel3D;

		float minDistance3D;
		float maxDistance3D;

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

		std::map<Common::UString, Property> userProperties;
		Common::UString category;

		std::vector<EventLayer> layers;
	};

	/** A sound definition. */
	struct SoundDefinition {
		PlayMode playMode;

		Common::UString name;

		uint32 spawnTimeMin;
		uint32 spawnTimeMax;
		uint32 maximumSpawnedSounds;
		float volume;
		float volumeRandomization;
		float pitch;
		float pitchRandomization;
		float position3DRandomization;
	};

	FEVFile(const Common::UString &resRef);
	FEVFile(Common::SeekableReadStream &fev);

	const Common::UString &getBankName();

	const std::vector<WaveBank> &getWaveBanks();
	const std::vector<Category> &getCategories();

private:
	void load(Common::SeekableReadStream &fev);

	/** Read a general category. */
	void readCategory(Common::SeekableReadStream &fev);
	/** Read a category for events. */
	void readEventCategory(Common::SeekableReadStream &fev);
	/** Read an event. */
	void readEvent(Common::SeekableReadStream &fev);

	/** Read properties. */
	std::map<Common::UString, Property> readProperties(Common::SeekableReadStream &fev);
	/** Read an FEV length prefixed string. */
	Common::UString readLengthPrefixedString(Common::SeekableReadStream &fev);

	Common::UString _bankName;

	std::vector<WaveBank> _waveBanks;
	std::vector<Category> _categories;
	std::vector<Event> _events;
	std::vector<SoundDefinition> _definitions;
};

} // End of namespace Sound

#endif // SOUND_FEVFILE_H

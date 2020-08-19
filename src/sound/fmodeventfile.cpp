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

#include "src/common/endianness.h"

#include "src/aurora/resman.h"

#include "src/sound/fmodeventfile.h"

namespace Sound {

static const uint32_t kFEVID = MKTAG('F', 'E', 'V', '1');

FMODEventFile::FMODEventFile(const Common::UString &resRef) {
	std::unique_ptr<Common::SeekableReadStream> fev(ResMan.getResource(resRef, Aurora::kFileTypeFEV));

	if (!fev)
		throw Common::Exception("FMODEventFile::FMODEventFile(): Resource %s not found", resRef.c_str());

	load(*fev);
}

FMODEventFile::FMODEventFile(Common::SeekableReadStream &fev) {
	load(fev);
}

const Common::UString &FMODEventFile::getBankName() {
	return _bankName;
}

const std::vector<FMODEventFile::WaveBank> &FMODEventFile::getWaveBanks() {
	return _waveBanks;
}

const std::vector<FMODEventFile::Category> &FMODEventFile::getCategories() {
	return _categories;
}

void FMODEventFile::load(Common::SeekableReadStream &fev) {
	const uint32_t magic = fev.readUint32BE();
	if (magic != kFEVID)
		throw Common::Exception("FMODEventFile::load(): Invalid magic number");

	fev.skip(12); // Unknown values

	_bankName = readLengthPrefixedString(fev);

	const uint32_t numWaveBanks = fev.readUint32LE();
	_waveBanks.resize(numWaveBanks);
	for (uint32_t i = 0; i < numWaveBanks; ++i) {
		WaveBank wb;

		wb.maxStreams = fev.readUint32LE();
		const uint32_t streamingType = fev.readUint32LE();

		switch (streamingType) {
			default:
			case 0x00010000:
				wb.streamingType = kDecompressIntoMemory;
				break;
			case 0x00020000:
				wb.streamingType = kLoadIntoMemory;
				break;
			case 0x0B000000:
				wb.streamingType = kStreamFromDisk;
				break;
		}

		wb.name = readLengthPrefixedString(fev);

		_waveBanks[i] = wb;
	}

	readCategory(fev);

	uint32_t numEventGroups = fev.readUint32LE();
	for (uint32_t i = 0; i < numEventGroups; ++i) {
		readEventCategory(fev);
	}

	uint32_t numSoundDefinitionTemplates = fev.readUint32LE();
	std::vector<SoundDefinition> definitions(numSoundDefinitionTemplates);
	for (uint32_t i = 0; i < numSoundDefinitionTemplates; ++i) {
		definitions[i].playMode = PlayMode(fev.readUint32LE());
		definitions[i].spawnTimeMin = fev.readUint32LE();
		definitions[i].spawnTimeMax = fev.readUint32LE();
		definitions[i].maximumSpawnedSounds = fev.readUint32LE();
		definitions[i].volume = fev.readIEEEFloatLE();

		fev.skip(12);

		definitions[i].volumeRandomization = fev.readIEEEFloatLE();
		definitions[i].pitch = fev.readIEEEFloatLE();

		fev.skip(12);

		definitions[i].pitchRandomization = fev.readIEEEFloatLE();
		definitions[i].position3DRandomization = fev.readIEEEFloatLE();
	}

	uint32_t numSoundDefinitions = fev.readUint32LE();
	_definitions.resize(numSoundDefinitions);
	for (uint32_t i = 0; i < numSoundDefinitions; ++i) {
		Common::UString name = readLengthPrefixedString(fev);
		fev.skip(4);
		uint32_t templateId = fev.readUint32LE();

		SoundDefinition definition = definitions[templateId];

		definition.name = name;

		_definitions.push_back(definition);
	}

	const uint32_t numReverbDefinitions = fev.readUint32LE();
	for (uint32_t i = 0; i < numReverbDefinitions; ++i) {
		ReverbDefinition reverb;

		reverb.name = readLengthPrefixedString(fev);

		reverb.room = fev.readSint32LE();
		reverb.roomHF = fev.readSint32LE();

		reverb.roomRollof = fev.readIEEEFloatLE();

		reverb.decayTime = fev.readIEEEFloatLE();
		reverb.decayHFRatio = fev.readIEEEFloatLE();

		reverb.reflections = fev.readSint32LE();
		reverb.reflectDelay = fev.readIEEEFloatLE();

		reverb.reverb = fev.readSint32LE();
		reverb.reverbDelay = fev.readIEEEFloatLE();

		reverb.diffusion = fev.readIEEEFloatLE();
		reverb.density = fev.readIEEEFloatLE();

		reverb.hfReference = fev.readIEEEFloatLE();
		reverb.roomLF = fev.readSint32LE();
		reverb.lfReference = fev.readIEEEFloatLE();

		fev.skip(76);
	}
}

void FMODEventFile::readCategory(Common::SeekableReadStream &fev) {
	const Common::UString name = readLengthPrefixedString(fev);

	Category category;
	category.volume = fev.readIEEEFloatLE();
	category.pitch = fev.readIEEEFloatLE();

	fev.skip(8); // Unknown values

	const uint32_t numSubCategories = fev.readUint32LE();
	for (uint32_t i = 0; i < numSubCategories; ++i) {
		readCategory(fev);
	}

	_categories.push_back(category);
}

void FMODEventFile::readEventCategory(Common::SeekableReadStream &fev) {
	Common::UString name = readLengthPrefixedString(fev);

	// Read user properties
	readProperties(fev);

	const uint32_t numSubEventCategories = fev.readUint32LE();
	const uint32_t numEvents = fev.readUint32LE();

	for (uint32_t i = 0; i < numSubEventCategories; ++i) {
		readEventCategory(fev);
	}

	for (uint32_t i = 0; i < numEvents; ++i) {
		readEvent(fev);
	}
}

void FMODEventFile::readEvent(Common::SeekableReadStream &fev) {
	Event event;

	fev.skip(4);

	event.name = readLengthPrefixedString(fev);

	event.volume = fev.readIEEEFloatLE();
	event.pitch = fev.readIEEEFloatLE();
	event.pitchRandomization = fev.readIEEEFloatLE();
	event.volumeRandomization = fev.readIEEEFloatLE();
	event.priority = fev.readUint32LE();
	event.maxPlaybacks = fev.readUint32LE();

	uint32_t mode = fev.readUint32BE();

	if (mode & 0x10000000)
		event.mode = k3D;
	else if (mode & 0x08000000)
		event.mode = k2D;
	else
		throw Common::Exception("Invalid event mode");

	if (mode & 0x00001000)
		event.rollof3D = kLogarithmic;
	else if (mode & 0x00002000)
		event.rollof3D = kLinear;
	else if (mode & 0x00000004)
		event.rollof3D = kCustom;
	else
		event.rollof3D = kUnspecified;

	if (mode & 0x00000400)
		event.position3D = kHeadRelative;
	else
		event.position3D = kWorldRelative;

	event.minDistance3D = fev.readIEEEFloatLE();
	event.maxDistance3D = fev.readIEEEFloatLE();

	event.Speaker2DL = fev.readIEEEFloatLE();
	event.Speaker2DR = fev.readIEEEFloatLE();
	event.Speaker2DC = fev.readIEEEFloatLE();

	event.SpeakerLFE = fev.readIEEEFloatLE();

	event.Speaker2DLR = fev.readIEEEFloatLE();
	event.Speaker2DRR = fev.readIEEEFloatLE();
	event.Speaker2DLS = fev.readIEEEFloatLE();
	event.Speaker2DRS = fev.readIEEEFloatLE();

	fev.skip(4);

	event.coneInsideAngle3D = fev.readIEEEFloatLE();
	event.coneOutsideAngle3D = fev.readIEEEFloatLE();
	event.coneOutsideVolume3D = fev.readIEEEFloatLE();

	event.maxPlaybacksBehavior = fev.readUint32LE();

	event.dopplerFactor3D = fev.readIEEEFloatLE();

	event.ReverbDryLevel = fev.readIEEEFloatLE();
	event.ReverbWetLevel = fev.readIEEEFloatLE();

	event.speakerSpread3D = fev.readIEEEFloatLE();

	event.fadeInTime = fev.readUint32LE();
	event.fadeOutTime = fev.readUint32LE();

	event.spawnIntensity = fev.readIEEEFloatLE();
	event.spawnIntensityRandomization = fev.readIEEEFloatLE();

	event.panLevel3D = fev.readIEEEFloatLE();

	event.positionRandomization3D = fev.readUint32LE();

	uint32_t numLayers = fev.readUint32LE();
	event.layers.resize(numLayers);
	for (uint32_t i = 0; i < numLayers; ++i) {
		EventLayer layer;

		fev.skip(2);

		layer.priority = fev.readSint16LE();

		fev.skip(6);

		event.layers[i] = layer;
	}

	fev.skip(4);

	event.userProperties = readProperties(fev);

	fev.skip(4); // Always 1?

	event.category = readLengthPrefixedString(fev);

	_events.push_back(event);
}

std::map<Common::UString, FMODEventFile::Property> FMODEventFile::readProperties(Common::SeekableReadStream &fev) {
	const uint32_t numUserProperties = fev.readUint32LE();
	std::map<Common::UString, FMODEventFile::Property> properties;
	for (uint32_t i = 0; i < numUserProperties; ++i) {
		Common::UString propertyName = readLengthPrefixedString(fev);
		Property property;
		property.type = PropertyType(fev.readUint32LE());
		switch (property.type) {
			case kPropertyInt: // Integer value
				property.value = fev.readSint32LE();
				break;
			case kPropertyFloat: // Floating point value
				property.value = fev.readIEEEFloatLE();
				break;
			case kPropertyString: // String value
				property.value = readLengthPrefixedString(fev);
				break;
			default:
				throw Common::Exception("FMODEventFile::readEventCategory() Invalid property type %i", property.type);
		}

		properties.insert(std::make_pair(propertyName, property));
	}

	return properties;
}

Common::UString FMODEventFile::readLengthPrefixedString(Common::SeekableReadStream &fev) {
	const uint32_t length = fev.readUint32LE();
	return Common::readStringFixed(fev, Common::kEncodingASCII, length);
}

} // End of namespace Sound

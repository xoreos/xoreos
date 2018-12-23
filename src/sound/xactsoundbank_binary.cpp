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
 *  A binary XACT SoundBank, found in the Xbox version of Jade Empire as XSB files.
 */

#include "src/common/strutil.h"
#include "src/common/encoding.h"
#include "src/common/readstream.h"

#include "src/sound/xactsoundbank_binary.h"

// Disable the "unused variable" warnings while most stuff is still stubbed
IGNORE_UNUSED_VARIABLES

namespace Sound {

static const size_t kCueDefinitionSize   = 20;
static const size_t kSoundDefinitionSize = 20;

enum XSBFlags {
	kXSBNoCueNames = 1
};

enum SoundFlags {
	kSound3D      = 0x01,
	kSoundTrivial = 0x08,
	kSoundSimple  = 0x10
};

enum PlayEventFlags {
	kPlayEventMultipleVariations = 0x04
};

XACTSoundBank_Binary::XACTSoundBank_Binary(Common::SeekableReadStream &xsb) {
	load(xsb);
}

static void readVariationData(Common::SeekableReadStream &xsb, uint16 &count, uint16 &current,
                              uint8 &selectMethod, uint8 &flags) {

	const uint32 variationData = xsb.readUint32LE();

	flags        =  variationData >> 30;
	current      = (variationData >> 17) & 0x1FFF;
	selectMethod = (variationData >> 13) & 0x000F;
	count        =  variationData        & 0x1FFF;
}

void XACTSoundBank_Binary::readCueVarations(Common::SeekableReadStream &xsb, Cue &cue, uint32 offset) {
	xsb.seek(offset);

	uint16 variationCount, currentVariation;
	uint8 selectMethod, flags;

	readVariationData(xsb, variationCount, currentVariation, selectMethod, flags);

	cue.variationSelectMethod = selectMethod;

	cue.variations.resize(variationCount);
	for (CueVariations::iterator variation = cue.variations.begin(); variation != cue.variations.end(); ++variation) {
		variation->soundIndex = xsb.readUint16LE();
		xsb.skip(2); // Unknown

		variation->weightMin = xsb.readUint16LE();
		variation->weightMax = xsb.readUint16LE();
	}
}

void XACTSoundBank_Binary::readWaveVariations(Common::SeekableReadStream &xsb, Track &track, uint32 offset,
                                              const std::vector<Common::UString> &banks) {

	xsb.seek(offset);

	uint16 variationCount, currentVariation;
	uint8 selectMethod, flags;

	readVariationData(xsb, variationCount, currentVariation, selectMethod, flags);

	track.variationSelectMethod = selectMethod;

	for (size_t i = 0; i < variationCount; i++) {
		const uint32 indices = xsb.readUint32LE();

		xsb.skip(4); // Unknown

		addWaveVariation(track, indices, banks);
	}
}

void XACTSoundBank_Binary::addWaveVariation(Track &track, uint32 indices,
                                            const std::vector<Common::UString> &banks) {

	const uint32 bankIndex  = indices >> 16;
	const uint32 soundIndex = indices & 0xFFFF;

	track.waves.push_back(WaveVariation());
	WaveVariation &wave = track.waves.back();

	wave.index = soundIndex;
	if (bankIndex < banks.size())
		wave.bank = banks[bankIndex];
}

std::vector<Common::UString> XACTSoundBank_Binary::readWaveBanks(Common::SeekableReadStream &xsb,
                                                                 uint32 offset, uint32 count) {

	std::vector<Common::UString> waveBanks;
	waveBanks.reserve(count);

	xsb.seek(offset);
	for (size_t i = 0; i < count; i++)
		waveBanks.push_back(Common::readStringFixed(xsb, Common::kEncodingASCII, 16));

	return waveBanks;
}

void XACTSoundBank_Binary::readCues(Common::SeekableReadStream &xsb, uint32 xsbFlags,
                                    uint32 offset, uint32 count) {

	_cues.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Cue &cue = _cues[i];
		xsb.seek(offset + i * kCueDefinitionSize);

		xsb.skip(2); // Unknown

		const uint16 soundIndex = xsb.readUint16LE();
		const uint32 offsetName = xsb.readUint32LE();
		const uint32 offsetEntry = xsb.readUint32LE();

		xsb.skip(4); // Unknown
		xsb.skip(4); // Unknown. Some kind of offset? Can be 0x07FFFFFF.

		if (!(xsbFlags & kXSBNoCueNames) && (offsetName != 0xFFFFFFFF)) {
			xsb.seek(offsetName);

			cue.name = Common::readString(xsb, Common::kEncodingASCII);
		}

		if (offsetEntry != 0xFFFFFFFF) {
			readCueVarations(xsb, cue, offsetEntry);

		} else if (soundIndex != 0xFFFF) {
			cue.variationSelectMethod = 0;

			cue.variations.resize(1);

			cue.variations.back().soundIndex = soundIndex;
			cue.variations.back().weightMin = 0;
			cue.variations.back().weightMax = 0;
		}
	}
}

void XACTSoundBank_Binary::readComplexTrack(Common::SeekableReadStream &xsb, Track &track,
                                            const std::vector<Common::UString> &banks) {

	const uint32 trackData = xsb.readUint32LE();

	const uint8  eventCount   = trackData & 0xFF;
	const uint32 eventsOffset = trackData >> 8;

	uint32 wavesOffset = 0xFFFFFFFF;

	xsb.seek(eventsOffset);
	for (size_t i = 0; i < eventCount; i++) {
		track.events.push_back(Event((EventType) xsb.readByte()));
		Event &event = track.events.back();

		xsb.skip(3); // Unknown

		uint8 parameterSize = xsb.readByte();

		const uint8 eventFlags = xsb.readByte();

		xsb.skip(2); // Unknown

		if (((event.type == kEventTypePlay) || (event.type == kEventTypePlayComplex)) &&
				 (parameterSize >= 4)) {

			const uint32 indicesOrOffset = xsb.readUint32LE();
			parameterSize -= 4;

			if (eventFlags & kPlayEventMultipleVariations)
				wavesOffset = indicesOrOffset;
			else
				addWaveVariation(track, indicesOrOffset, banks);
		}

		xsb.skip(parameterSize);
	}

	if (wavesOffset != 0xFFFFFFFF)
		readWaveVariations(xsb, track, wavesOffset, banks);
}

void XACTSoundBank_Binary::readTracks(Common::SeekableReadStream &xsb, Sound &sound, uint32 indicesOrOffset,
                                      uint32 count, uint8 flags, const std::vector<Common::UString> &banks) {

	if ((flags & (kSoundTrivial | kSoundSimple)) && (count != 1))
		throw Common::Exception("XACTSoundBank_Binary::readTracks(): Trivial/simple sound, but trackCount == %u",
		                        count);

	sound.tracks.resize(count);

	if (flags & kSoundTrivial) {
		// One track, one event, one wave variation

		addWaveVariation(sound.tracks[0], indicesOrOffset, banks);
		sound.tracks[0].events.push_back(Event(kEventTypePlay));

		return;
	}

	if (flags & kSoundSimple) {
		// One track, one event, multiple wave variations

		readWaveVariations(xsb, sound.tracks[0], indicesOrOffset, banks);
		sound.tracks[0].events.push_back(Event(kEventTypePlay));

		return;
	}

	// Complex

	static const size_t kTrackDefinitionSize = 4;

	for (size_t i = 0; i < count; ++i) {
		Track &track = sound.tracks[i];
		xsb.seek(indicesOrOffset + i * kTrackDefinitionSize);

		readComplexTrack(xsb, track, banks);
	}
}

void XACTSoundBank_Binary::readSounds(Common::SeekableReadStream &xsb, uint32 offset, uint32 count,
                                      const std::vector<Common::UString> &banks) {

	_sounds.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Sound &sound = _sounds[i];
		xsb.seek(offset + i * kSoundDefinitionSize);

		sound.category = 0;

		const uint32 indicesOrOffset = xsb.readUint32LE();

		const uint16 volume = xsb.readUint16LE();
		const uint16 pitch  = xsb.readUint16LE();

		const uint8 trackCount = xsb.readByte();

		const uint8 layer    = xsb.readByte();
		const uint8 category = xsb.readByte();

		const uint8 soundFlags = xsb.readByte();

		const uint16 index3DParam = xsb.readUint16LE();

		const uint8 priority = xsb.readByte();
		const uint8 volume3D = xsb.readByte();

		const uint16 eqGain = xsb.readUint16LE();
		const uint16 eq = xsb.readUint16LE();


		readTracks(xsb, sound, indicesOrOffset, trackCount, soundFlags, banks);
	}
}

void XACTSoundBank_Binary::load(Common::SeekableReadStream &xsb) {
	static const uint32 kXSBID = MKTAG('S', 'D', 'B', 'K');

	const uint32 id = xsb.readUint32BE();
	if (id != kXSBID)
		throw Common::Exception("Not a XSB file (%s)", Common::debugTag(id).c_str());

	const uint16 version = xsb.readUint16LE();
	if (version != 11)
		throw Common::Exception("Unsupported XSB file version %u", version);

	xsb.skip(2); // CRC. We're ignoring it (for now?)

	const uint32 offsetWaveBanks = xsb.readUint32LE();
	const uint32 offset2         = xsb.readUint32LE();
	const uint32 offset3DParams  = xsb.readUint32LE();
	const uint32 offset4         = xsb.readUint32LE();

	const uint16 xsbFlags = xsb.readUint16LE();

	const uint16 count1     = xsb.readUint16LE();
	const uint16 soundCount = xsb.readUint16LE();
	const uint16 cueCount   = xsb.readUint16LE();
	const uint16 count4     = xsb.readUint16LE();
	const uint16 bankCount  = xsb.readUint16LE();

	xsb.skip(4); // Unknown

	_name = Common::readStringFixed(xsb, Common::kEncodingASCII, 16);

	const size_t offsetCues   = xsb.pos();
	const size_t offsetSounds = offsetCues + cueCount * kCueDefinitionSize;


	const std::vector<Common::UString> waveBanks = readWaveBanks(xsb, offsetWaveBanks, bankCount);

	readCues(xsb, xsbFlags, offsetCues, cueCount);
	readSounds(xsb, offsetSounds, soundCount, waveBanks);
}

} // End of namespace Sound

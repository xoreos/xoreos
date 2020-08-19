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

namespace Sound {

static constexpr size_t k3DDefinitionSize    = 40;
static constexpr size_t kCueDefinitionSize   = 20;
static constexpr size_t kSoundDefinitionSize = 20;
static constexpr size_t kFadeDefinitionSize  = 16;
static constexpr size_t kTrackDefinitionSize =  4;

static constexpr uint16_t kXSBNoCueNames = 0x0001;

static constexpr uint16_t kCueSequential   = 0x0001;
static constexpr uint16_t kCueCrossfade    = 0x0002;
static constexpr uint16_t kCueStopOnStarve = 0x0004;
static constexpr uint16_t kCueInteractive  = 0x0008;

static constexpr uint8_t kSound3D        = 0x01;
static constexpr uint8_t kSoundGainBoost = 0x02;
static constexpr uint8_t kSoundEQ        = 0x04;
static constexpr uint8_t kSoundTrivial   = 0x08;
static constexpr uint8_t kSoundSimple    = 0x10;
static constexpr uint8_t kSoundLinger    = 0x20;

static constexpr uint8_t kPlayEventMultipleVariations = 0x04;
static constexpr uint8_t kPlayEventLoopNewVariation   = 0x40;

static constexpr uint8_t kPitchEventVariation = 0x0;
static constexpr uint8_t kPitchEventRelative  = 0x1;
static constexpr uint8_t kPitchEventFade      = 0x2;

static constexpr uint8_t kVolumeEventVariation = 0x0;
static constexpr uint8_t kVolumeEventRelative  = 0x1;
static constexpr uint8_t kVolumeEventFade      = 0x2;

static constexpr uint8_t kLowPassEventRandom   = 0x0;
static constexpr uint8_t kLowPassEventRelative = 0x1;
static constexpr uint8_t kLowPassEventSweep    = 0x2;

static constexpr uint8_t kMarkerEventRepeat = 0x2;

XACTSoundBank_Binary::XACTSoundBank_Binary(Common::SeekableReadStream &xsb) {
	load(xsb);
}

static void readVariationData(Common::SeekableReadStream &xsb, uint16_t &count, uint16_t &current,
                              uint8_t &selectMethod, uint8_t &flags) {

	const uint32_t variationData = xsb.readUint32LE();

	flags        =  variationData >> 30;
	current      = (variationData >> 17) & 0x1FFF;
	selectMethod = (variationData >> 13) & 0x000F;
	count        =  variationData        & 0x1FFF;
}

void XACTSoundBank_Binary::readCueVarations(Common::SeekableReadStream &xsb, Cue &cue, uint32_t offset) {
	xsb.seek(offset);

	uint16_t variationCount, currentVariation;
	uint8_t selectMethod, flags;

	readVariationData(xsb, variationCount, currentVariation, selectMethod, flags);

	cue.variationSelectMethod = static_cast<SelectMethod>(selectMethod);

	cue.variations.resize(variationCount);
	for (auto &variation : cue.variations) {
		variation.soundIndex = xsb.readUint16LE();
		xsb.skip(2); // Unknown

		if (variation.soundIndex == 0xFFFF)
			variation.soundIndex = kSoundSilence;

		variation.weightMin = CLIP<size_t>(xsb.readUint16LE(), kWeightMinimum, kWeightMaximum);
		variation.weightMax = CLIP<size_t>(xsb.readUint16LE(), kWeightMinimum, kWeightMaximum);

		if (variation.weightMin > variation.weightMax)
			std::swap(variation.weightMin, variation.weightMax);
	}
}

void XACTSoundBank_Binary::readWaveVariations(Common::SeekableReadStream &xsb, Track &track, uint32_t offset) {
	xsb.seek(offset);

	uint16_t variationCount, currentVariation;
	uint8_t selectMethod, flags;

	readVariationData(xsb, variationCount, currentVariation, selectMethod, flags);

	track.variationSelectMethod = static_cast<SelectMethod>(selectMethod);

	for (size_t i = 0; i < variationCount; i++) {
		const uint32_t indices = xsb.readUint32LE();

		const uint16_t weightMin = xsb.readUint16LE();
		const uint16_t weightMax = xsb.readUint16LE();

		addWaveVariation(track, indices, weightMin, weightMax);
	}
}

void XACTSoundBank_Binary::addWaveVariation(Track &track, uint32_t indices,
                                            uint32_t weightMin, uint32_t weightMax) {

	const uint32_t bankIndex  = indices >> 16;
	const uint32_t soundIndex = indices & 0xFFFF;

	track.waves.push_back(WaveVariation());
	WaveVariation &wave = track.waves.back();

	wave.index = soundIndex;
	if (bankIndex < _waveBanks.size())
		wave.bank = _waveBanks[bankIndex].name;

	if (wave.index == 0xFFFF)
		wave.index = kSoundSilence;

	wave.weightMin = CLIP<size_t>(weightMin, kWeightMinimum, kWeightMaximum);
	wave.weightMax = CLIP<size_t>(weightMax, kWeightMinimum, kWeightMaximum);

	if (wave.weightMin > wave.weightMax)
		std::swap(wave.weightMin, wave.weightMax);
}

void XACTSoundBank_Binary::readWaveBanks(Common::SeekableReadStream &xsb, uint32_t offset, uint32_t count) {
	xsb.seek(offset);

	_waveBanks.resize(count);
	for (auto &bank : _waveBanks) {
		bank.name = Common::readStringFixed(xsb, Common::kEncodingASCII, 16);

		_waveBankMap[bank.name] = &bank;
	}
}

void XACTSoundBank_Binary::readCues(Common::SeekableReadStream &xsb, uint32_t xsbFlags,
                                    uint32_t offset, uint32_t count, uint32_t offsetFadeParams) {

	_cues.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Cue &cue = _cues[i];
		xsb.seek(offset + i * kCueDefinitionSize);

		const uint16_t cueFlags = xsb.readUint16LE();

		cue.sequential   = cueFlags & kCueSequential;
		cue.crossfade    = cueFlags & kCueCrossfade;
		cue.stopOnStarve = cueFlags & kCueStopOnStarve;
		cue.interactive  = cueFlags & kCueInteractive;

		const uint16_t soundIndex = xsb.readUint16LE();
		const uint32_t offsetName = xsb.readUint32LE();
		const uint32_t offsetEntry = xsb.readUint32LE();

		const uint16_t fadeParamIndex = xsb.readUint16LE();

		xsb.skip(2); // Unknown

		uint32_t offsetTransitions = 0;
		offsetTransitions += xsb.readByte();
		offsetTransitions += xsb.readByte() <<  8;
		offsetTransitions += xsb.readByte() << 16;

		cue.transitionTrigger = xsb.readByte() >> 3;

		if (!(xsbFlags & kXSBNoCueNames) && (offsetName != 0xFFFFFFFF)) {
			xsb.seek(offsetName);

			cue.name = Common::readString(xsb, Common::kEncodingASCII);
			_cueMap[cue.name] = &cue;
		}

		if (cue.crossfade) {
			xsb.seek(offsetFadeParams + fadeParamIndex * kFadeDefinitionSize);

			cue.fadeIn.duration  = xsb.readUint32LE() / 10000;
			cue.fadeIn.volume    = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);
			cue.fadeIn.type      = static_cast<CrossfadeType>(xsb.readByte() >> 4);
			cue.fadeIn.stepCount = xsb.readByte();

			cue.fadeOut.duration  = xsb.readUint32LE() / 10000;
			cue.fadeOut.volume    = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);
			cue.fadeOut.type      = static_cast<CrossfadeType>(xsb.readByte() >> 4);
			cue.fadeOut.stepCount = xsb.readByte();
		}

		if (offsetEntry != 0xFFFFFFFF) {
			readCueVarations(xsb, cue, offsetEntry);

		} else if (soundIndex != 0xFFFF) {
			cue.variationSelectMethod = SelectMethod::Ordered;

			cue.variations.resize(1);

			cue.variations.back().soundIndex = soundIndex;
			cue.variations.back().weightMin = kWeightMinimum;
			cue.variations.back().weightMax = kWeightMaximum;
		}

		if (cue.interactive && (offsetTransitions != 0x00FFFFFF)) {
			xsb.seek(offsetTransitions);

			const uint32_t transitionCount = xsb.readUint32LE();
			cue.transitions.resize(transitionCount);

			size_t transFrom = kSoundSilence;
			size_t transTo   = 0;

			for (auto &transition : cue.transitions) {
				transition.from = transFrom;
				transition.to   = transTo;

				const uint16_t transFlags = xsb.readUint16LE();

				transition.sourceWhen      = static_cast<TransitionSource>     ((transFlags     ) & 0xF);
				transition.destinationWhen = static_cast<TransitionDestination>( transFlags >> 7  & 0xF);
				transition.effect          = static_cast<TransitionEffect>     ((transFlags >> 4) & 0x7);

				transition.transitionSound = xsb.readUint16LE();
				if (transition.transitionSound == 0xFFFF)
					transition.transitionSound = kSoundSilence;

				transition.sourceFadeDuration      = xsb.readUint16LE();
				transition.destinationFadeDuration = xsb.readUint16LE();

				transition.sourceMarkerLow  = xsb.readUint32LE();
				transition.sourceMarkerHigh = xsb.readUint32LE();

				transition.destinationMarkerLow  = xsb.readUint32LE();
				transition.destinationMarkerHigh = xsb.readUint32LE();

				if (transFrom == ++transTo)
					transTo++;

				if (transTo >= cue.variations.size()) {
					transFrom++;
					transTo = kSoundSilence;
				}
			}
		}
	}
}

void XACTSoundBank_Binary::readComplexTrack(Common::SeekableReadStream &xsb, Track &track, Sound &sound) {
	const uint32_t trackData = xsb.readUint32LE();

	const uint8_t  eventCount   = trackData & 0xFF;
	const uint32_t eventsOffset = trackData >> 8;

	uint32_t wavesOffset = 0xFFFFFFFF;

	xsb.seek(eventsOffset);
	for (size_t i = 0; i < eventCount; i++) {
		track.events.push_back(Event((EventType) xsb.readByte()));
		Event &event = track.events.back();

		event.timestamp  = xsb.readByte();
		event.timestamp += xsb.readByte() << 8;
		event.timestamp += xsb.readByte() << 16;

		uint8_t parameterSize = xsb.readByte();

		const uint8_t eventFlags = xsb.readByte();

		switch (event.type) {
			case EventType::Play:
			case EventType::PlayComplex:
				sound.loopCount = xsb.readUint16LE();

				if (parameterSize >= 4) {
					const uint32_t indicesOrOffset = xsb.readUint32LE();
					parameterSize -= 4;

					if (parameterSize >= 12) {
						sound.pitchVariationMin = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);
						sound.pitchVariationMax = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);

						sound.volumeVariationMin = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 64.0f);
						sound.volumeVariationMax = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 64.0f);

						sound.delay = xsb.readUint16LE();

						xsb.skip(2); // Unknown

						parameterSize -= 12;
					}

					sound.loopNewVariation = eventFlags & kPlayEventLoopNewVariation;

					if (!(eventFlags & kPlayEventMultipleVariations)) {
						track.variationSelectMethod = SelectMethod::Ordered;

						addWaveVariation(track, indicesOrOffset, kWeightMinimum, kWeightMaximum);

					} else
						wavesOffset = indicesOrOffset;
				}
				break;

			case EventType::Pitch:
				event.params.pitch.fadeStepCount = xsb.readUint16LE();

				event.params.pitch.isRelative      = eventFlags & kPitchEventRelative;
				event.params.pitch.enableFade      = eventFlags & kPitchEventFade;
				event.params.pitch.enableVariation = eventFlags & kPitchEventVariation;

				if (parameterSize >= 8) {
					event.params.pitch.pitchStart = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);
					event.params.pitch.pitchEnd   = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);

					xsb.skip(1); // Unknown

					event.params.pitch.fadeDuration  = xsb.readByte();
					event.params.pitch.fadeDuration += xsb.readByte() <<  8;
					event.params.pitch.fadeDuration += xsb.readByte() << 16;

					parameterSize -= 8;
				}
				break;

			case EventType::Volume:
				event.params.volume.fadeStepCount = xsb.readUint16LE();

				event.params.volume.isRelative      = eventFlags & kVolumeEventRelative;
				event.params.volume.enableFade      = eventFlags & kVolumeEventFade;
				event.params.volume.enableVariation = eventFlags & kVolumeEventVariation;

				if (parameterSize >= 8) {
					event.params.volume.volumeStart = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 64.0f);
					event.params.volume.volumeEnd   = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 64.0f);

					xsb.skip(1); // Unknown

					event.params.volume.fadeDuration  = xsb.readByte();
					event.params.volume.fadeDuration += xsb.readByte() <<  8;
					event.params.volume.fadeDuration += xsb.readByte() << 16;

					parameterSize -= 8;
				}
				break;

			case EventType::LowPass:
				event.params.lowpass.isRelative  = eventFlags & kLowPassEventRelative;
				event.params.lowpass.random      = eventFlags & kLowPassEventRandom;
				event.params.lowpass.sweepCutOff = eventFlags & kLowPassEventSweep;

				event.params.lowpass.sweepStepCount = xsb.readUint16LE();

				if (parameterSize >= 12) {
					event.params.lowpass.cutOffStart = CLIP<uint16_t>(xsb.readUint16LE(), 0, 8192);
					event.params.lowpass.cutOffEnd   = CLIP<uint16_t>(xsb.readUint16LE(), 0, 8192);

					xsb.skip(1); // Unknown

					event.params.lowpass.sweepDuration  = xsb.readByte();
					event.params.lowpass.sweepDuration += xsb.readByte() <<  8;
					event.params.lowpass.sweepDuration += xsb.readByte() << 16;

					event.params.lowpass.resonanceStart = CLIP(xsb.readSint16LE() / 100.0f, 0.0f, 32.0f);
					event.params.lowpass.resonanceEnd   = CLIP(xsb.readSint16LE() / 100.0f, 0.0f, 32.0f);

					parameterSize -= 12;
				}
				break;

			case EventType::LFOPitch:
				xsb.skip(2); // Unused

				if (parameterSize >= 4) {
					xsb.skip(2); // Unknown

					event.params.lfopitch.delta = xsb.readByte()  * 23.4f / 255.0f;
					event.params.lfopitch.pitch = xsb.readSByte() * 12.0f / 128.0f;

					parameterSize -= 4;
				}
				break;

			case EventType::LFOMulti:
				xsb.skip(2); // Unused

				if (parameterSize >= 6) {
					xsb.skip(2); // Unknown

					event.params.lfomulti.delta     = xsb.readByte()  * 23.4f / 255.0f;
					event.params.lfomulti.pitch     = xsb.readSByte() * 12.0f / 128.0f;
					event.params.lfomulti.filter    = xsb.readSByte() * 96.0f / 128.0f;
					event.params.lfomulti.amplitude = xsb.readSByte() * 16.0f / 128.0f;

					parameterSize -= 6;
				}
				break;

			case EventType::EnvelopeAmplitude:
				xsb.skip(2); // Unused

				if (parameterSize >= 12) {
					event.params.aeg.delay   = (xsb.readUint16LE() * 43680) / 4095;
					event.params.aeg.attack  = (xsb.readUint16LE() * 43680) / 4095;
					event.params.aeg.hold    = (xsb.readUint16LE() * 43680) / 4095;
					event.params.aeg.decay   = (xsb.readUint16LE() * 43680) / 4095;
					event.params.aeg.release = (xsb.readUint16LE() * 43680) / 4095;

					event.params.aeg.sustain = xsb.readByte() / 255.0f;

					xsb.skip(1); // Unknown

					parameterSize -= 12;
				}
				break;

			case EventType::EnvelopePitch:
				xsb.skip(2); // Unused

				if (parameterSize >= 16) {
					event.params.pfeg.delay   = (xsb.readUint16LE() * 43680) / 4095;
					event.params.pfeg.attack  = (xsb.readUint16LE() * 43680) / 4095;
					event.params.pfeg.hold    = (xsb.readUint16LE() * 43680) / 4095;
					event.params.pfeg.decay   = (xsb.readUint16LE() * 43680) / 4095;
					event.params.pfeg.release = (xsb.readUint16LE() * 43680) / 4095;

					event.params.pfeg.sustain = xsb.readByte() / 255.0f;

					xsb.skip(3); // Unknown

					event.params.pfeg.pitch  = xsb.readSByte() * 12.0f / 128.0f;
					event.params.pfeg.filter = xsb.readSByte() * 96.0f / 128.0f;

					parameterSize -= 16;
				}
				break;

			case EventType::Loop:
				event.params.loop.count = xsb.readUint16LE();
				break;

			case EventType::Marker:
				event.params.marker.repeat = eventFlags & kMarkerEventRepeat;

				event.params.marker.repeatCount = xsb.readUint16LE();

				if (parameterSize >= 8) {
					event.params.marker.value = xsb.readUint32LE();

					xsb.skip(1); // Unknown

					event.params.marker.repeatDuration  = xsb.readByte();
					event.params.marker.repeatDuration += xsb.readByte() <<  8;
					event.params.marker.repeatDuration += xsb.readByte() << 16;

					parameterSize -= 8;
				}
				break;

			case EventType::MixBins:
				xsb.skip(2); // Unused

				for (size_t j = 0; j < ARRAYSIZE(event.params.mixbins.bins) && parameterSize >= 4; j++, parameterSize -= 4) {
					event.params.mixbins.bins[j].channel = xsb.readByte();

					xsb.skip(1); // Unknown

					event.params.mixbins.bins[j].volume = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);
				}
				break;

			case EventType::EnvironmentReverb:
				xsb.skip(2); // Unused

				if (parameterSize >= 48) {
					event.params.envreverb.room              = CLIP(xsb.readSint32LE() / 10000.0f, -1.0f, 0.0f);
					event.params.envreverb.roomHF            = CLIP(xsb.readSint32LE() / 10000.0f, -1.0f, 0.0f);
					event.params.envreverb.roomRollOffFactor = CLIP(xsb.readIEEEFloatLE(), 0.0f, 10.0f);

					event.params.envreverb.decay        = CLIP(xsb.readIEEEFloatLE(), 0.1f, 20.0f) * 1000;
					event.params.envreverb.decayHFRatio = CLIP(xsb.readIEEEFloatLE(), 0.1f, 2.0f);

					event.params.envreverb.reflection      = CLIP(xsb.readSint32LE() / 10000.0f, -1.0f, 0.0f);
					event.params.envreverb.reflectionDelay = CLIP(xsb.readIEEEFloatLE(), 0.0f, 0.3f) * 1000;

					event.params.envreverb.reverb      = CLIP(xsb.readSint32LE() / 10000.0f, -1.0f, 0.0f);
					event.params.envreverb.reverbDelay = CLIP(xsb.readIEEEFloatLE(), 0.0f, 0.1f) * 1000;

					event.params.envreverb.diffusion = CLIP(xsb.readIEEEFloatLE(), 0.0f, 100.0f) / 100.0f;
					event.params.envreverb.density   = CLIP(xsb.readIEEEFloatLE(), 0.0f, 100.0f) / 100.0f;

					event.params.envreverb.referenceHF = CLIP(xsb.readIEEEFloatLE(), 0.0f, 20000.0f);

					parameterSize -= 48;
				}
				break;

			case EventType::MixBinsPan:
				xsb.skip(2); // Unused

				if (parameterSize >= 4) {
					event.params.mixbinspan.speakerChannelCount = xsb.readByte() ? 4 : 5;

					uint32_t angles = 0;
					angles += xsb.readByte();
					angles += xsb.readByte() << 8;
					angles += xsb.readByte() << 16;

					event.params.mixbinspan.angleStart =  angles       & 0x1FF;
					event.params.mixbinspan.angleEnd   = (angles >> 9) & 0x1FF;

					event.params.mixbinspan.use3D = (angles >> 18) & 1;

					parameterSize -= 4;

					for (size_t j = 0; j < ARRAYSIZE(event.params.mixbinspan.bins) && parameterSize >= 4; j++, parameterSize -= 4) {
						event.params.mixbinspan.bins[j].channel = xsb.readByte();

						xsb.skip(1); // Unknown

						event.params.mixbinspan.bins[j].volume = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);
					}
				}
				break;

			default:
				xsb.skip(2); // Unknown
				break;
		}

		xsb.skip(parameterSize);
	}

	if (wavesOffset != 0xFFFFFFFF)
		readWaveVariations(xsb, track, wavesOffset);
}

void XACTSoundBank_Binary::readTracks(Common::SeekableReadStream &xsb, Sound &sound,
                                      uint32_t indicesOrOffset, uint32_t count, uint8_t flags) {

	if ((flags & (kSoundTrivial | kSoundSimple)) && (count != 1))
		throw Common::Exception("XACTSoundBank_Binary::readTracks(): Trivial/simple sound, but trackCount == %u",
		                        count);

	sound.tracks.resize(count);

	if (flags & kSoundTrivial) {
		// One track, one event, one wave variation

		sound.tracks[0].variationSelectMethod = SelectMethod::Ordered;

		addWaveVariation(sound.tracks[0], indicesOrOffset, kWeightMinimum, kWeightMaximum);
		sound.tracks[0].events.push_back(Event(EventType::Play));

		return;
	}

	if (flags & kSoundSimple) {
		// One track, one event, multiple wave variations

		readWaveVariations(xsb, sound.tracks[0], indicesOrOffset);
		sound.tracks[0].events.push_back(Event(EventType::Play));

		return;
	}

	// Complex

	for (size_t i = 0; i < count; ++i) {
		Track &track = sound.tracks[i];
		xsb.seek(indicesOrOffset + i * kTrackDefinitionSize);

		readComplexTrack(xsb, track, sound);
	}
}

void XACTSoundBank_Binary::readSounds(Common::SeekableReadStream &xsb, uint32_t offset, uint32_t count,
                                      uint32_t offset3DParams) {

	_sounds.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Sound &sound = _sounds[i];
		xsb.seek(offset + i * kSoundDefinitionSize);

		const uint32_t indicesOrOffset = xsb.readUint32LE();

		const uint16_t volume = xsb.readUint16LE();
		sound.volume = -((int16_t) (volume & 0x1FF)) * 0.16f;

		sound.pitch = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);

		const uint8_t trackCount = xsb.readByte();

		sound.layer         = xsb.readByte();
		sound.categoryIndex = xsb.readByte();

		const uint8_t soundFlags = xsb.readByte();

		const uint16_t index3DParam = xsb.readUint16LE();

		sound.priority = xsb.readByte();

		const uint8_t volume3D = xsb.readByte();

		sound.parametricEQ = soundFlags & kSoundEQ;

		sound.parametricEQGain = CLIP(xsb.readSint16LE() / 8192.0f, -1.0f, 4.0f);

		const uint16_t eq = xsb.readUint16LE();

		sound.parametricEQQ    = 1.0f / (1 << (eq & 7));
		sound.parametricEQFreq = CLIP<uint16_t>((eq >> 3) & 0x1FFF, 30, 8000);

		sound.gainBoost = soundFlags & kSoundGainBoost;
		sound.linger    = soundFlags & kSoundLinger;

		sound.is3D = soundFlags & kSound3D;
		if (sound.is3D) {
			sound.params3D.volumeLFE   = -((int16_t) ((volume >> 9) & 0x7F)) * 0.50f;
			sound.params3D.volumeI3DL2 = CLIP(-((int16_t) volume3D) * 2.56f, -64.0f, 0.0f);

			xsb.seek(offset3DParams + index3DParam * k3DDefinitionSize);

			sound.params3D.coneInsideAngle   = CLIP<uint16_t>(xsb.readUint16LE(), 0, 360);
			sound.params3D.coneOutsideAngle  = CLIP<uint16_t>(xsb.readUint16LE(), 0, 360);
			sound.params3D.coneOutsideVolume = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);

			xsb.skip(2); // Unknown;

			sound.params3D.distanceMin = xsb.readIEEEFloatLE();
			sound.params3D.distanceMax = xsb.readIEEEFloatLE();

			sound.params3D.distanceFactor = xsb.readIEEEFloatLE();
			sound.params3D.rollOffFactor  = xsb.readIEEEFloatLE();
			sound.params3D.dopplerFactor  = xsb.readIEEEFloatLE();

			sound.params3D.mode = static_cast<Mode3D>(xsb.readByte());

			const size_t rollOffCurveSize = CLIP<size_t>(xsb.readByte(), 0, 10);

			sound.params3D.rollOffCurve.reserve(rollOffCurveSize);
			for (size_t j = 0; j < rollOffCurveSize; ++j)
				sound.params3D.rollOffCurve.push_back(xsb.readByte() / 255.0f);
		}

		readTracks(xsb, sound, indicesOrOffset, trackCount, soundFlags);
	}
}

void XACTSoundBank_Binary::load(Common::SeekableReadStream &xsb) {
	static constexpr uint32_t kXSBID = MKTAG('S', 'D', 'B', 'K');

	const uint32_t id = xsb.readUint32BE();
	if (id != kXSBID)
		throw Common::Exception("Not a XSB file (%s)", Common::debugTag(id).c_str());

	const uint16_t version = xsb.readUint16LE();
	if (version != 11)
		throw Common::Exception("Unsupported XSB file version %u", version);

	xsb.skip(2); // CRC. We're ignoring it (for now?)

	const uint32_t offsetWaveBanks  = xsb.readUint32LE();
	const uint32_t offsetFadeParams = xsb.readUint32LE();
	const uint32_t offset3DParams   = xsb.readUint32LE();
	xsb.skip(4); // Some offset

	const uint16_t xsbFlags = xsb.readUint16LE();

	xsb.skip(2); // Some count
	const uint16_t soundCount = xsb.readUint16LE();
	const uint16_t cueCount   = xsb.readUint16LE();
	xsb.skip(2); // Some count
	const uint16_t bankCount  = xsb.readUint16LE();

	xsb.skip(4); // Unknown

	_name = Common::readStringFixed(xsb, Common::kEncodingASCII, 16);

	const size_t offsetCues   = xsb.pos();
	const size_t offsetSounds = offsetCues + cueCount * kCueDefinitionSize;


	readWaveBanks(xsb, offsetWaveBanks, bankCount);
	readCues(xsb, xsbFlags, offsetCues, cueCount, offsetFadeParams);
	readSounds(xsb, offsetSounds, soundCount, offset3DParams);
}

} // End of namespace Sound

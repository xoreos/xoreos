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

enum XSBFlags {
	kXSBNoCueNames = 1
};

enum CueFlags {
	kCueSequential   = 0x0001,
	kCueCrossfade    = 0x0002,
	kCueStopOnStarve = 0x0004,
	kCueInteractive  = 0x0008
};

enum SoundFlags {
	kSound3D        = 0x01,
	kSoundGainBoost = 0x02,
	kSoundEQ        = 0x04,
	kSoundTrivial   = 0x08,
	kSoundSimple    = 0x10,
	kSoundLinger    = 0x20
};

enum PlayEventFlags {
	kPlayEventMultipleVariations = 0x04,
	kPlayEventLoopNewVariation   = 0x40
};

enum PitchEventFlags {
	kPitchEventVariation = 0x04,
	kPitchEventRelative  = 0x10,
	kPitchEventFade      = 0x20
};

enum VolumeEventFlags {
	kVolumeEventVariation = 0x04,
	kVolumeEventRelative  = 0x10,
	kVolumeEventFade      = 0x20
};

enum LowPassEventFlags {
	kLowPassEventRandom   = 0x04,
	kLowPassEventRelative = 0x10,
	kLowPassEventSweep    = 0x20
};

enum MarkerEventFlags {
	kMarkerEventRepeat = 0x20
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

	cue.variationSelectMethod = static_cast<SelectMethod>(selectMethod);

	cue.variations.resize(variationCount);
	for (CueVariations::iterator variation = cue.variations.begin(); variation != cue.variations.end(); ++variation) {
		variation->soundIndex = xsb.readUint16LE();
		xsb.skip(2); // Unknown

		if (variation->soundIndex == 0xFFFF)
			variation->soundIndex = kSoundSilence;

		variation->weightMin = CLIP<size_t>(xsb.readUint16LE(), kWeightMinimum, kWeightMaximum);
		variation->weightMax = CLIP<size_t>(xsb.readUint16LE(), kWeightMinimum, kWeightMaximum);

		if (variation->weightMin > variation->weightMax)
			SWAP(variation->weightMin, variation->weightMax);
	}
}

void XACTSoundBank_Binary::readWaveVariations(Common::SeekableReadStream &xsb, Track &track, uint32 offset) {
	xsb.seek(offset);

	uint16 variationCount, currentVariation;
	uint8 selectMethod, flags;

	readVariationData(xsb, variationCount, currentVariation, selectMethod, flags);

	track.variationSelectMethod = static_cast<SelectMethod>(selectMethod);

	for (size_t i = 0; i < variationCount; i++) {
		const uint32 indices = xsb.readUint32LE();

		const uint16 weightMin = xsb.readUint16LE();
		const uint16 weightMax = xsb.readUint16LE();

		addWaveVariation(track, indices, weightMin, weightMax);
	}
}

void XACTSoundBank_Binary::addWaveVariation(Track &track, uint32 indices,
                                            uint32 weightMin, uint32 weightMax) {

	const uint32 bankIndex  = indices >> 16;
	const uint32 soundIndex = indices & 0xFFFF;

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
		SWAP(wave.weightMin, wave.weightMax);
}

void XACTSoundBank_Binary::readWaveBanks(Common::SeekableReadStream &xsb, uint32 offset, uint32 count) {
	xsb.seek(offset);

	_waveBanks.resize(count);
	for (WaveBanks::iterator bank = _waveBanks.begin(); bank != _waveBanks.end(); ++bank) {
		bank->name = Common::readStringFixed(xsb, Common::kEncodingASCII, 16);

		_waveBankMap[bank->name] = &*bank;
	}
}

void XACTSoundBank_Binary::readCues(Common::SeekableReadStream &xsb, uint32 xsbFlags,
                                    uint32 offset, uint32 count, uint32 offsetFadeParams) {

	_cues.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Cue &cue = _cues[i];
		xsb.seek(offset + i * kCueDefinitionSize);

		const uint16 cueFlags = xsb.readUint16LE();

		cue.sequential   = cueFlags & kCueSequential;
		cue.crossfade    = cueFlags & kCueCrossfade;
		cue.stopOnStarve = cueFlags & kCueStopOnStarve;
		cue.interactive  = cueFlags & kCueInteractive;

		const uint16 soundIndex = xsb.readUint16LE();
		const uint32 offsetName = xsb.readUint32LE();
		const uint32 offsetEntry = xsb.readUint32LE();

		const uint16 fadeParamIndex = xsb.readUint16LE();

		xsb.skip(2); // Unknown

		uint32 offsetTransitions = 0;
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
			cue.variationSelectMethod = kSelectMethodOrdered;

			cue.variations.resize(1);

			cue.variations.back().soundIndex = soundIndex;
			cue.variations.back().weightMin = kWeightMinimum;
			cue.variations.back().weightMax = kWeightMaximum;
		}

		if (cue.interactive && (offsetTransitions != 0x00FFFFFF)) {
			xsb.seek(offsetTransitions);

			const uint32 transitionCount = xsb.readUint32LE();
			cue.transitions.resize(transitionCount);

			size_t transFrom = kSoundSilence;
			size_t transTo   = 0;

			for (Transitions::iterator t = cue.transitions.begin(); t != cue.transitions.end(); ++t) {
				t->from = transFrom;
				t->to   = transTo;

				const uint16 transFlags = xsb.readUint16LE();

				t->sourceWhen      = static_cast<TransitionSource>     ((transFlags     ) & 0xF);
				t->destinationWhen = static_cast<TransitionDestination>( transFlags >> 7  & 0xF);
				t->effect          = static_cast<TransitionEffect>     ((transFlags >> 4) & 0x7);

				t->transitionSound = xsb.readUint16LE();
				if (t->transitionSound == 0xFFFF)
					t->transitionSound = kSoundSilence;

				t->sourceFadeDuration      = xsb.readUint16LE();
				t->destinationFadeDuration = xsb.readUint16LE();

				t->sourceMarkerLow  = xsb.readUint32LE();
				t->sourceMarkerHigh = xsb.readUint32LE();

				t->destinationMarkerLow  = xsb.readUint32LE();
				t->destinationMarkerHigh = xsb.readUint32LE();

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
	const uint32 trackData = xsb.readUint32LE();

	const uint8  eventCount   = trackData & 0xFF;
	const uint32 eventsOffset = trackData >> 8;

	uint32 wavesOffset = 0xFFFFFFFF;

	xsb.seek(eventsOffset);
	for (size_t i = 0; i < eventCount; i++) {
		track.events.push_back(Event((EventType) xsb.readByte()));
		Event &event = track.events.back();

		event.timestamp  = xsb.readByte();
		event.timestamp += xsb.readByte() << 8;
		event.timestamp += xsb.readByte() << 16;

		uint8 parameterSize = xsb.readByte();

		const uint8 eventFlags = xsb.readByte();

		switch (event.type) {
			case kEventTypePlay:
			case kEventTypePlayComplex:
				sound.loopCount = xsb.readUint16LE();

				if (parameterSize >= 4) {
					const uint32 indicesOrOffset = xsb.readUint32LE();
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
						track.variationSelectMethod = kSelectMethodOrdered;

						addWaveVariation(track, indicesOrOffset, kWeightMinimum, kWeightMaximum);

					} else
						wavesOffset = indicesOrOffset;
				}
				break;

			case kEventTypePitch:
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

			case kEventTypeVolume:
				event.params.volume.fadeStepCount = xsb.readUint16LE();

				event.params.volume.isRelative      = eventFlags & kPitchEventRelative;
				event.params.volume.enableFade      = eventFlags & kPitchEventFade;
				event.params.volume.enableVariation = eventFlags & kPitchEventVariation;

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

			case kEventTypeLowPass:
				event.params.lowpass.isRelative  = eventFlags & kLowPassEventRelative;
				event.params.lowpass.random      = eventFlags & kLowPassEventRandom;
				event.params.lowpass.sweepCutOff = eventFlags & kLowPassEventSweep;

				event.params.lowpass.sweepStepCount = xsb.readUint16LE();

				if (parameterSize >= 12) {
					event.params.lowpass.cutOffStart = CLIP<uint16>(xsb.readUint16LE(), 0, 8192);
					event.params.lowpass.cutOffEnd   = CLIP<uint16>(xsb.readUint16LE(), 0, 8192);

					xsb.skip(1); // Unknown

					event.params.lowpass.sweepDuration  = xsb.readByte();
					event.params.lowpass.sweepDuration += xsb.readByte() <<  8;
					event.params.lowpass.sweepDuration += xsb.readByte() << 16;

					event.params.lowpass.resonanceStart = CLIP(xsb.readSint16LE() / 100.0f, 0.0f, 32.0f);
					event.params.lowpass.resonanceEnd   = CLIP(xsb.readSint16LE() / 100.0f, 0.0f, 32.0f);

					parameterSize -= 12;
				}
				break;

			case kEventTypeLFOPitch:
				xsb.skip(2); // Unused

				if (parameterSize >= 4) {
					xsb.skip(2); // Unknown

					event.params.lfopitch.delta = xsb.readByte()  * 23.4f / 255.0f;
					event.params.lfopitch.pitch = xsb.readSByte() * 12.0f / 128.0f;

					parameterSize -= 4;
				}
				break;

			case kEventTypeLFOMulti:
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

			case kEventTypeEnvelopeAmplitude:
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

			case kEventTypeEnvelopePitch:
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

			case kEventTypeLoop:
				event.params.loop.count = xsb.readUint16LE();
				break;

			case kEventTypeMarker:
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

			case kEventTypeMixBins:
				xsb.skip(2); // Unused

				for (size_t j = 0; j < ARRAYSIZE(event.params.mixbins.bins) && parameterSize >= 4; j++, parameterSize -= 4) {
					event.params.mixbins.bins[j].channel = xsb.readByte();

					xsb.skip(1); // Unknown

					event.params.mixbins.bins[j].volume = CLIP(xsb.readSint16LE() / 100.0f, -64.0f, 0.0f);
				}
				break;

			case kEventTypeEnvironmentReverb:
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

			case kEventTypeMixBinsPan:
				xsb.skip(2); // Unused

				if (parameterSize >= 4) {
					event.params.mixbinspan.speakerChannelCount = xsb.readByte() ? 4 : 5;

					uint32 angles = 0;
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
                                      uint32 indicesOrOffset, uint32 count, uint8 flags) {

	if ((flags & (kSoundTrivial | kSoundSimple)) && (count != 1))
		throw Common::Exception("XACTSoundBank_Binary::readTracks(): Trivial/simple sound, but trackCount == %u",
		                        count);

	sound.tracks.resize(count);

	if (flags & kSoundTrivial) {
		// One track, one event, one wave variation

		sound.tracks[0].variationSelectMethod = kSelectMethodOrdered;

		addWaveVariation(sound.tracks[0], indicesOrOffset, kWeightMinimum, kWeightMaximum);
		sound.tracks[0].events.push_back(Event(kEventTypePlay));

		return;
	}

	if (flags & kSoundSimple) {
		// One track, one event, multiple wave variations

		readWaveVariations(xsb, sound.tracks[0], indicesOrOffset);
		sound.tracks[0].events.push_back(Event(kEventTypePlay));

		return;
	}

	// Complex

	static constexpr size_t kTrackDefinitionSize = 4;

	for (size_t i = 0; i < count; ++i) {
		Track &track = sound.tracks[i];
		xsb.seek(indicesOrOffset + i * kTrackDefinitionSize);

		readComplexTrack(xsb, track, sound);
	}
}

void XACTSoundBank_Binary::readSounds(Common::SeekableReadStream &xsb, uint32 offset, uint32 count,
                                      uint32 offset3DParams) {

	_sounds.resize(count);
	for (size_t i = 0; i < count; ++i) {
		Sound &sound = _sounds[i];
		xsb.seek(offset + i * kSoundDefinitionSize);

		const uint32 indicesOrOffset = xsb.readUint32LE();

		const uint16 volume = xsb.readUint16LE();
		sound.volume = -((int16) (volume & 0x1FF)) * 0.16f;

		sound.pitch = CLIP((xsb.readSint16LE() * 12) / 4096.0f, -24.0f, 24.0f);

		const uint8 trackCount = xsb.readByte();

		sound.layer         = xsb.readByte();
		sound.categoryIndex = xsb.readByte();

		const uint8 soundFlags = xsb.readByte();

		const uint16 index3DParam = xsb.readUint16LE();

		sound.priority = xsb.readByte();

		const uint8 volume3D = xsb.readByte();

		sound.parametricEQ = soundFlags & kSoundEQ;

		sound.parametricEQGain = CLIP(xsb.readSint16LE() / 8192.0f, -1.0f, 4.0f);

		const uint16 eq = xsb.readUint16LE();

		sound.parametricEQQ    = 1.0f / (1 << (eq & 7));
		sound.parametricEQFreq = CLIP<uint16>((eq >> 3) & 0x1FFF, 30, 8000);

		sound.gainBoost = soundFlags & kSoundGainBoost;
		sound.linger    = soundFlags & kSoundLinger;

		sound.is3D = soundFlags & kSound3D;
		if (sound.is3D) {
			sound.params3D.volumeLFE   = -((int16) ((volume >> 9) & 0x7F)) * 0.50f;
			sound.params3D.volumeI3DL2 = CLIP(-((int16) volume3D) * 2.56f, -64.0f, 0.0f);

			xsb.seek(offset3DParams + index3DParam * k3DDefinitionSize);

			sound.params3D.coneInsideAngle   = CLIP<uint16>(xsb.readUint16LE(), 0, 360);
			sound.params3D.coneOutsideAngle  = CLIP<uint16>(xsb.readUint16LE(), 0, 360);
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
	static constexpr uint32 kXSBID = MKTAG('S', 'D', 'B', 'K');

	const uint32 id = xsb.readUint32BE();
	if (id != kXSBID)
		throw Common::Exception("Not a XSB file (%s)", Common::debugTag(id).c_str());

	const uint16 version = xsb.readUint16LE();
	if (version != 11)
		throw Common::Exception("Unsupported XSB file version %u", version);

	xsb.skip(2); // CRC. We're ignoring it (for now?)

	const uint32 offsetWaveBanks  = xsb.readUint32LE();
	const uint32 offsetFadeParams = xsb.readUint32LE();
	const uint32 offset3DParams   = xsb.readUint32LE();
	xsb.skip(4); // Some offset

	const uint16 xsbFlags = xsb.readUint16LE();

	xsb.skip(2); // Some count
	const uint16 soundCount = xsb.readUint16LE();
	const uint16 cueCount   = xsb.readUint16LE();
	xsb.skip(2); // Some count
	const uint16 bankCount  = xsb.readUint16LE();

	xsb.skip(4); // Unknown

	_name = Common::readStringFixed(xsb, Common::kEncodingASCII, 16);

	const size_t offsetCues   = xsb.pos();
	const size_t offsetSounds = offsetCues + cueCount * kCueDefinitionSize;


	readWaveBanks(xsb, offsetWaveBanks, bankCount);
	readCues(xsb, xsbFlags, offsetCues, cueCount, offsetFadeParams);
	readSounds(xsb, offsetSounds, soundCount, offset3DParams);
}

} // End of namespace Sound

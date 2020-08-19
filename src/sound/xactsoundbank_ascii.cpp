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
 *  An ASCII XACT SoundBank, found in the non-Xbox versions of Jade Empire as _xsb.txt files.
 */

#include <set>

#include "src/common/strutil.h"
#include "src/common/readstream.h"
#include "src/common/streamtokenizer.h"

#include "src/sound/xactsoundbank_ascii.h"

namespace Sound {

XACTSoundBank_ASCII::XACTSoundBank_ASCII(Common::SeekableReadStream &xsb) {
	load(xsb);
}

static Common::UString stripZeros(Common::UString str) {
	while ((str.size() > 1) && (*str.begin() == '0'))
		str.erase(str.begin());

	return str;
}

static int64_t getNumber(Common::UString str) {
	int64_t number = 0;

	Common::parseString(stripZeros(str), number);

	return number;
}

static float getFloat(Common::UString str) {
	float number = 0;

	Common::parseString(str, number);

	return number;
}

static size_t getAmount(Common::UString str) {
	size_t amount = 0;

	Common::parseString(stripZeros(str), amount);

	return amount;
}

static size_t getAmount(Common::StreamTokenizer &tokenizer, Common::SeekableReadStream &stream) {
	return getAmount(tokenizer.getToken(stream));
}

size_t XACTSoundBank_ASCII::findSound(const Common::UString &name, Cue &cue) const {
	if (name == "0")
		return kSoundSilence;

	for (size_t i = 0; i < cue.variations.size(); i++)
		if (cue.variations[i].soundName == name)
			return i;

	return kSoundSilence;
}

size_t XACTSoundBank_ASCII::findSound(const Common::UString &name) const {
	if (name == "0")
		return kSoundSilence;

	SoundMap::const_iterator it = _soundMap.find(name);
	if (it == _soundMap.end())
		return kSoundSilence;

	return it->second - _sounds.data();
}

void XACTSoundBank_ASCII::load(Common::SeekableReadStream &xsb) {
	Common::StreamTokenizer tokenizer(Common::StreamTokenizer::kRuleIgnoreAll);
	tokenizer.addSeparator(' ');
	tokenizer.addChunkEnd('\n');
	tokenizer.addIgnore('\r');
	tokenizer.addQuote('\"');

	_name = tokenizer.getToken(xsb);
	tokenizer.nextChunk(xsb);

	const size_t categoryCount = getAmount(tokenizer, xsb);
	tokenizer.nextChunk(xsb);

	std::vector<Common::UString> categories(categoryCount);
	for (auto &category : categories) {
		category = tokenizer.getToken(xsb);
		tokenizer.nextChunk(xsb);
	}

	const size_t soundCount = getAmount(tokenizer, xsb);
	tokenizer.nextChunk(xsb);

	std::set<Common::UString> bankNames;

	_sounds.resize(soundCount);
	for (auto &sound : _sounds) {
		std::vector<Common::UString> tokens;
		tokenizer.getTokens(xsb, tokens, 2);
		tokenizer.nextChunk(xsb);

		sound.name = tokens[0];

		const int64_t categoryIndex = getNumber(tokens[1]);
		if ((categoryIndex >= 0) && (static_cast<size_t>(categoryIndex) < categories.size())) {
			sound.categoryIndex = categoryIndex;
			sound.categoryName  = categories[categoryIndex];
		}

		if (tokens.size() > 5)
			sound.volume = CLIP((getNumber(tokens[4]) + getNumber(tokens[5])) / 100.0f, -64.0f, 0.0f);

		if (tokens.size() > 6)
			sound.pitch = CLIP(getNumber(tokens[6]) / 100.0f, -24.0f, 24.0f);

		if (tokens.size() > 7) {
			const int64_t priority = getNumber(tokens[7]);
			if (priority > 0)
				sound.priority = priority;
		}

		if (tokens.size() > 8)
			sound.gainBoost = tokens[8] == "1";

		if (tokens.size() > 9)
			sound.params3D.volumeLFE   = CLIP(getNumber(tokens[ 9]) / 100.0f, -64.0f, 0.0f);
		if (tokens.size() > 10)
			sound.params3D.volumeI3DL2 = CLIP(getNumber(tokens[10]) / 100.0f, -64.0f, 0.0f);

		_soundMap[sound.name] = &sound;

		sound.tracks.resize(1);
		Track &track = sound.tracks.back();

		size_t waveCount = 0;

		while (!xsb.eos()) {
			tokenizer.getTokens(xsb, tokens);
			tokenizer.nextChunk(xsb);

			if (tokens.empty())
				break;

			if ((tokens.size() == 1) && (tokens[0].size() == 3)) {
				waveCount = getAmount(tokens[0]);
				break;
			}

			if        (tokens[0] == "3D") {
				sound.is3D = true;

				if (tokens.size() > 1)
					sound.params3D.mode = static_cast<Mode3D>(getNumber(tokens[1]));

				if (tokens.size() > 2)
					sound.params3D.coneInsideAngle   = CLIP<uint16_t>(getNumber(tokens[2]), 0, 360);
				if (tokens.size() > 3)
					sound.params3D.coneOutsideAngle  = CLIP<uint16_t>(getNumber(tokens[3]), 0, 360);
				if (tokens.size() > 4)
					sound.params3D.coneOutsideVolume = CLIP(getNumber(tokens[4]) / 100.0f, -64.0f, 0.0f);

				if (tokens.size() > 5)
					sound.params3D.distanceMin = getFloat(tokens[5]);
				if (tokens.size() > 6)
					sound.params3D.distanceMax = getFloat(tokens[6]);

				if (tokens.size() > 7)
					sound.params3D.distanceFactor = getFloat(tokens[7]);
				if (tokens.size() > 8)
					sound.params3D.rollOffFactor  = getFloat(tokens[8]);
				if (tokens.size() > 9)
					sound.params3D.dopplerFactor  = getFloat(tokens[9]);

				if ((tokens.size() > 10) && (tokens[10] != "0"))
					for (size_t i = 10; i < tokens.size(); i++)
						sound.params3D.rollOffCurve.push_back(getFloat(tokens[i]));

			} else if (tokens[0] == "PEQ") {
				sound.parametricEQ = true;

				if (tokens.size() > 1)
					sound.parametricEQGain = CLIP(getNumber(tokens[1]) / 1000.0f, -1.0f, 4.0f);
				if (tokens.size() > 2)
					sound.parametricEQQ = 1.0f / (1 << CLIP<int64_t>(getNumber(tokens[2]), 0, 7));
				if (tokens.size() > 3)
					sound.parametricEQFreq = CLIP<uint16_t>(getNumber(tokens[3]), 30, 8000);

			} else if (tokens[0] == "PLAY") {
				bool isComplex = false;

				if (tokens.size() > 1)
					sound.loopCount = getNumber(tokens[1]);

				if (tokens.size() > 2)
					sound.loopNewVariation = tokens[2] == "1";

				if (tokens.size() > 5) {
					sound.pitchVariationMin = CLIP(getNumber(tokens[4]) / 100.0f, -24.0f, 24.0f);
					sound.pitchVariationMax = CLIP(getNumber(tokens[5]) / 100.0f, -24.0f, 24.0f);

					if (sound.pitchVariationMin != sound.pitchVariationMax)
						isComplex = true;
				}

				if (tokens.size() > 7) {
					sound.volumeVariationMin = CLIP(getNumber(tokens[6]) / 100.0f, -64.0f, 64.0f);
					sound.volumeVariationMax = CLIP(getNumber(tokens[7]) / 100.0f, -64.0f, 64.0f);

					if (sound.volumeVariationMin != sound.volumeVariationMax)
						isComplex = true;
				}

				if (tokens.size() > 8) {
					sound.delay = getNumber(tokens[8]);

					if (sound.delay != 0)
						isComplex = true;
				}

				if (isComplex)
					track.events.push_back(Event(EventType::PlayComplex));
				else
					track.events.push_back(Event(EventType::Play));

				if (tokens.size() > 9)
					track.events.back().timestamp = getNumber(tokens[9]);

			} else if (tokens[0] == "LOOP") {
				track.events.push_back(Event(EventType::Loop));
				track.events.back().params.loop.count = kLoopCountInfinite;

				if (tokens.size() > 1)
					track.events.back().params.loop.count = getNumber(tokens[1]);

				track.variationSelectMethod = SelectMethod::Ordered;
				if (tokens.size() > 3)
					track.variationSelectMethod = static_cast<SelectMethod>(getNumber(tokens[3]));

			} else if (tokens[0] == "VOLUME") {
				track.events.push_back(Event(EventType::Volume));
				Event &event = track.events.back();

				if (tokens.size() > 1)
					event.params.volume.enableVariation = tokens[1] == "1";
				if (tokens.size() > 2)
					event.params.volume.isRelative      = tokens[2] == "1";

				if (tokens.size() > 3)
					event.params.volume.fadeStepCount = getNumber(tokens[3]);
				if (tokens.size() > 5)
					event.params.volume.fadeDuration  = getNumber(tokens[5]);

				if (tokens.size() > 6)
					event.params.volume.volumeStart = CLIP(getNumber(tokens[6]) / 100.0f, -64.0f, 64.0f);
				if (tokens.size() > 7)
					event.params.volume.volumeEnd   = CLIP(getNumber(tokens[7]) / 100.0f, -64.0f, 64.0f);

			} else if (tokens[0] == "PITCH") {
				track.events.push_back(Event(EventType::Pitch));
				Event &event = track.events.back();

				if (tokens.size() > 1)
					event.params.pitch.enableVariation = tokens[1] == "1";
				if (tokens.size() > 2)
					event.params.pitch.isRelative      = tokens[2] == "1";

				if (tokens.size() > 3)
					event.params.pitch.fadeStepCount = getNumber(tokens[3]);
				if (tokens.size() > 5)
					event.params.pitch.fadeDuration  = getNumber(tokens[5]);

				if (tokens.size() > 6)
					event.params.pitch.pitchStart = CLIP(getNumber(tokens[6]) / 100.0f, -24.0f, 24.0f);
				if (tokens.size() > 7)
					event.params.pitch.pitchEnd   = CLIP(getNumber(tokens[7]) / 100.0f, -24.0f, 24.0f);

			} else if (tokens[0] == "LPF") {
				track.events.push_back(Event(EventType::LowPass));
				Event &event = track.events.back();

				if (tokens.size() > 1)
					event.params.lowpass.isRelative  = tokens[1] == "1";
				if (tokens.size() > 2)
					event.params.lowpass.sweepCutOff = tokens[2] == "1";

				if (tokens.size() > 3)
					event.params.lowpass.sweepStepCount = getNumber(tokens[3]);
				if (tokens.size() > 5)
					event.params.lowpass.sweepDuration  = getNumber(tokens[5]);

				if (tokens.size() > 6)
					event.params.lowpass.resonanceEnd   = CLIP(getNumber(tokens[6]) / 100.0f, 0.0f, 32.0f);
				if (tokens.size() > 7)
					event.params.lowpass.resonanceStart = CLIP(getNumber(tokens[7]) / 100.0f, 0.0f, 32.0f);

				if (tokens.size() > 8)
					event.params.lowpass.cutOffEnd   = getNumber(tokens[8]);
				if (tokens.size() > 9)
					event.params.lowpass.cutOffStart = getNumber(tokens[9]);

				if (tokens.size() > 4)
					event.timestamp = getNumber(tokens[4]);

			} else if (tokens[0] == "MFLFO") {
				track.events.push_back(Event(EventType::LFOMulti));
				Event &event = track.events.back();

				if (tokens.size() > 3)
					event.params.lfomulti.delta     = CLIP(getNumber(tokens[3]) /  10.0f,   0.0f, 23.4f);
				if (tokens.size() > 4)
					event.params.lfomulti.pitch     = CLIP(getNumber(tokens[4]) / 100.0f, -12.0f, 12.0f);
				if (tokens.size() > 5)
					event.params.lfomulti.filter    = CLIP(getNumber(tokens[5]) / 100.0f, -96.0f, 96.0f);
				if (tokens.size() > 6)
					event.params.lfomulti.amplitude = CLIP(getNumber(tokens[6]) / 100.0f, -16.0f, 16.0f);

				if (tokens.size() > 1)
					event.timestamp = getNumber(tokens[1]);

			} else if (tokens[0] == "MARKER") {
				track.events.push_back(Event(EventType::Marker));

				/* The first parameter is the name of the marker. This information
				 * is stripped in the binary version, so we don't need it.
				 *
				 * The second or third parameter is the value of the marker. Both
				 * are always 0 in all the files we have, so we can't know which is
				 * which. And it doesn't matter, then, since Jade Empire apparently
				 * only cares that there is a marker happening, not what the value
				 * is, or even when it occurs. */

			} else if (tokens[0] == "AEG") {
				track.events.push_back(Event(EventType::EnvelopeAmplitude));
				Event &event = track.events.back();

				if (tokens.size() > 1)
					event.params.aeg.delay   = getNumber(tokens[1]);
				if (tokens.size() > 2)
					event.params.aeg.attack  = getNumber(tokens[2]);
				if (tokens.size() > 3)
					event.params.aeg.hold    = getNumber(tokens[3]);
				if (tokens.size() > 4)
					event.params.aeg.decay   = getNumber(tokens[4]);
				if (tokens.size() > 5)
					event.params.aeg.release = getNumber(tokens[5]);
				if (tokens.size() > 6)
					event.params.aeg.sustain = CLIP(getNumber(tokens[6]) / 100.0f, 0.0f, 1.0f);

			} else {
				warning("Unknown event \"%s\"", tokens[0].c_str());
			}
		}

		const size_t weightPerWave = waveCount ? (kWeightMaximum / waveCount) : kWeightMaximum;

		track.waves.resize(waveCount);
		for (size_t i = 0; i < waveCount; i++) {
			WaveVariation &wave = track.waves[i];

			tokenizer.getTokens(xsb, tokens, 3);
			tokenizer.nextChunk(xsb);

			wave.name = tokens[1];
			wave.bank = tokens[0];

			wave.index = getNumber(tokens[2]);

			wave.weightMin = i * weightPerWave;
			wave.weightMax = (i != (waveCount - 1)) ? ((i + 1) * weightPerWave - 1) : kWeightMaximum;

			bankNames.insert(wave.bank);
		}
	}

	_waveBanks.reserve(bankNames.size());
	for (auto &name : bankNames) {
		_waveBanks.push_back(WaveBank(name));
		WaveBank &bank = _waveBanks.back();

		_waveBankMap[bank.name] = &bank;
	}

	const size_t cueCount = getAmount(tokenizer, xsb);
	tokenizer.nextChunk(xsb);

	_cues.resize(cueCount);
	for (auto &cue : _cues) {
		std::vector<Common::UString> tokens;
		tokenizer.getTokens(xsb, tokens, 3);
		tokenizer.nextChunk(xsb);

		cue.name = tokens[0];
		_cueMap[cue.name] = &cue;

		cue.variationSelectMethod = static_cast<SelectMethod>(getNumber(tokens[1]));

		cue.interactive = tokens[2] == "1";

		const size_t variationCount = getAmount(tokenizer, xsb);
		tokenizer.nextChunk(xsb);

		const size_t weightPerVar = variationCount ? (kWeightMaximum / variationCount) : kWeightMaximum;

		cue.variations.resize(variationCount);
		for (size_t i = 0; i < variationCount; i++) {
			CueVariation &variation = cue.variations[i];

			tokenizer.getTokens(xsb, tokens, 4);
			tokenizer.nextChunk(xsb);

			variation.soundName = tokens[0];
			variation.soundIndex = getNumber(tokens[1]);

			if (cue.variationSelectMethod == SelectMethod::Parameter) {
				variation.weightMax = getNumber(tokens[2]);
				variation.weightMin = getNumber(tokens[3]);

				/* BUGFIX: This happens in Jade Empire's MusicBank_xsb.txt.
				 *         Jade Empire's cue selection code probably only
				 *         checks the lower bounds of parameter-controlled
				 *         cue selections. */
				if ((variation.weightMin > 0) && (variation.weightMax == 0))
					variation.weightMax = variation.weightMin;

				if (variation.weightMin > variation.weightMax)
					std::swap(variation.weightMin, variation.weightMax);

			} else {
				variation.weightMin = i * weightPerVar;
				variation.weightMax = (i != (variationCount - 1)) ? ((i + 1) * weightPerVar - 1) : kWeightMaximum;
			}
		}

		if (!cue.interactive)
			continue;

		const size_t transitionCount = getAmount(tokenizer, xsb);
		tokenizer.nextChunk(xsb);

		cue.transitions.resize(transitionCount);
		for (auto &transition : cue.transitions) {
			tokenizer.getTokens(xsb, tokens, 2);
			tokenizer.nextChunk(xsb);

			transition.from = findSound(tokens[0], cue);
			transition.to   = findSound(tokens[1], cue);

			if (tokens.size() > 2)
				transition.transitionSound = findSound(tokens[2]);

			if (tokens.size() > 3) {
				if      (tokens[3] == "Crossfade")
					transition.effect = TransitionEffect::Crossfade;
				else if (tokens[3] == "Sound")
					transition.effect = TransitionEffect::SoundFadeTo;
			}

			// The 4th token might be destinationWhen, but it's always 0 in Jade Empire

			if (tokens.size() > 5) {
				if (tokens[5] == "End")
					transition.sourceWhen = TransitionSource::EndOfSound;
			}

			if (tokens.size() > 7) {
				transition.sourceFadeDuration      = getNumber(tokens[6]);
				transition.destinationFadeDuration = getNumber(tokens[7]);
			}
		}
	}
}

} // End of namespace Sound

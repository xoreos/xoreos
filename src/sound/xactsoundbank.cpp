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
 *  An abstract XACT SoundBank, containing sound files.
 */

#include "src/common/scopedptr.h"
#include "src/common/error.h"

#include "src/aurora/resman.h"

#include "src/sound/xactsoundbank.h"
#include "src/sound/xactsoundbank_ascii.h"
#include "src/sound/xactsoundbank_binary.h"
#include "src/sound/xactwavebank.h"
#include "src/sound/audiostream.h"
#include "src/sound/sound.h"

namespace Sound {

XACTSoundBank::WaveBank::~WaveBank() {
	delete bank;
}


XACTSoundBank::Event::Event(EventType t) : type(t), timestamp(0) {
	switch (type) {
		case kEventTypePitch:
			params.pitch.isRelative = false;
			params.pitch.enableFade = false;
			params.pitch.enableVariation = false;

			params.pitch.fadeStepCount = 0;
			params.pitch.fadeDuration = 0;

			params.pitch.pitchStart = 0.0f;
			params.pitch.pitchEnd = 0.0f;
			break;

		case kEventTypeVolume:
			params.volume.isRelative = false;
			params.volume.enableFade = false;
			params.volume.enableVariation = false;

			params.volume.fadeStepCount = 0;
			params.volume.fadeDuration = 0;

			params.volume.volumeStart = 0.0f;
			params.volume.volumeEnd = 0.0f;
			break;

		case kEventTypeLoop:
			params.loop.count = 0;
			break;

		default:
			break;
	}
}


XACTSoundBank *XACTSoundBank::load(const Common::UString &name) {
	try {
		Common::ScopedPtr<Common::SeekableReadStream> stream;

		stream.reset(ResMan.getResource(name, Aurora::kFileTypeXSB));
		if (stream)
			return new XACTSoundBank_Binary(*stream);

		stream.reset(ResMan.getResource(name + "_xsb", Aurora::kFileTypeTXT));
		if (stream)
			return new XACTSoundBank_ASCII(*stream);

		throw Common::Exception("No such SoundBank");

	} catch (Common::Exception &e) {
		e.add("Failed loading XACT SoundBank \"%s\"", name.c_str());
		throw;
	}

	return 0;
}

ChannelHandle XACTSoundBank::playCue(size_t index, size_t variation, SoundType soundType) {
	if (index >= _cues.size())
		throw Common::Exception("XACTSoundBank::playCue(): Cue index out of range (%u > %u)",
		                        (uint)index, (uint)_cues.size());

	return playCue(_cues[index], variation, soundType);
}

ChannelHandle XACTSoundBank::playCue(const Common::UString &name, size_t variation, SoundType soundType) {
	CueMap::iterator cue = _cueMap.find(name);
	if (cue == _cueMap.end())
		throw Common::Exception("XACTSoundBank::playCue(): Cue \"%s\" doesn't exist", name.c_str());

	return playCue(*cue->second, variation, soundType);
}

ChannelHandle XACTSoundBank::playCue(Cue &cue, size_t variation, SoundType soundType) {
	for (CueVariations::iterator var = cue.variations.begin(); var != cue.variations.end(); ++var) {
		if ((variation >= var->weightMin) && (variation <= var->weightMax)) {
			if (var->soundIndex >= _sounds.size())
				throw Common::Exception("XACTSoundBank::playCue(): Sound for variation %u out of range (%u > %u)",
				                        (uint)variation, (uint)var->soundIndex, (uint)_sounds.size());

			return playSound(_sounds[var->soundIndex], soundType);
		}
	}

	throw Common::Exception("XACTSoundBank::playCue(): No variation %u", (uint)variation);
}

ChannelHandle XACTSoundBank::playSound(Sound &sound, SoundType soundType) {
	if (sound.tracks.empty())
		return ChannelHandle();

	if (sound.tracks.size() > 1)
		warning("XACTSoundBank::playSound(): Unsupported feature: more than one track");

	return playTrack(sound.tracks[0], sound, soundType);
}

ChannelHandle XACTSoundBank::playTrack(Track &track, const Sound &UNUSED(sound), SoundType soundType) {
	if (track.waves.empty())
		return ChannelHandle();

	if (track.waves.size() > 1)
		warning("XACTSoundBank::playTrack(): TODO: select a wave variation");

	WaveVariation &wave = track.waves[0];

	if (wave.bank.empty())
		return ChannelHandle();

	const XACTWaveBank *bank = getWaveBank(wave.bank);

	Common::ScopedPtr<RewindableAudioStream> stream(bank->getWave(wave.index));

	size_t loops = 1;
	for (Events::const_iterator event = track.events.begin(); event != track.events.end(); ++event)
		if (event->type == kEventTypeLoop)
			loops = (event->params.loop.count == kLoopCountInfinite) ? 0 : (event->params.loop.count + 1);

	if (loops == 1)
		return SoundMan.playAudioStream(stream.release(), soundType);

	return SoundMan.playAudioStream(new LoopingAudioStream(stream.release(), loops), soundType);
}

const XACTWaveBank *XACTSoundBank::getWaveBank(const Common::UString &name) {
	WaveBankMap::iterator bank = _waveBankMap.find(name);
	if (bank == _waveBankMap.end())
		throw Common::Exception("XACTSoundBank::getWaveBank(): Don't know wave bank \"%s\"", name.c_str());

	if (!bank->second->bank)
		bank->second->bank = XACTWaveBank::load(name);

	return bank->second->bank;
}

} // End of namespace Sound

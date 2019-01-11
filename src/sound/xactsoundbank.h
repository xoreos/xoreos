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
 *  An abstract XACT SoundBank, containing sound definitions.
 */

#ifndef SOUND_XACTSOUNDBANK_H
#define SOUND_XACTSOUNDBANK_H

#include <vector>
#include <map>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/sound/types.h"

namespace Sound {

class XACTWaveBank;

/** An abstract XACT SoundBank interface.
 *
 *  This interface allows us to treat both binary and ASCII variants
 *  of the XSB file the same.
 *
 *  An XACT SoundBank is part of Microsoft's Cross-platform Audio
 *  Creation Tool (XACT), and commonly used together with WaveBanks,
 *  which contain the actual audio data.
 *
 *  Conceptually, a soundbank consists of cues and sounds. A sound is a
 *  collection of tracks, which each contain a list of events and a list
 *  of wave variations, while a cue consists of a list of references to
 *  sounds.
 *
 *  Both the cue variation and the wave variation can be selected using
 *  different methods. For example, they could be played in order, or
 *  randomly. Or they could be explicitly selected by game.
 *
 *  See also xactsoundbank_binary.h for the original Xbox version of
 *  the XSB file and xactsoundbank_ascii.h for the textual representation.
 *
 *  For the abstract WaveBank interface, see xactwavebank.h.
 */
class XACTSoundBank {
public:
	virtual ~XACTSoundBank() { }

	/** Load an XACT SoundBank, of either ASCII or Binary format. */
	static XACTSoundBank *load(const Common::UString &name);

	/** Return the internal name of the SoundBank. */
	const Common::UString &getName() { return _name; }

	/** Play a sound cue by index, manually selecting the variation.
	 *
	 *  @param  index Index of the cue to play.
	 *  @param  variation Index of the cue variation to play.
	 *  @param  soundType What kind of sound is this?
	 *  @return A handle to the sound channel, not yet started. If the cue
	 *          variation is empty, an invalid handle is returned.
	 *          On error, an exception is thrown.
	 */
	ChannelHandle playCue(size_t index, size_t variation, SoundType soundType);

	/** Play a sound cue by name, manually selecting the variation.
	 *
	 *  @param  name Name of the cue to play.
	 *  @param  variation Index of the cue variation to play.
	 *  @param  soundType What kind of sound is this?
	 *  @return A handle to the sound channel, not yet started. If the cue
	 *          variation is empty, an invalid handle is returned.
	 *          On error, an exception is thrown.
	 */
	ChannelHandle playCue(const Common::UString &name, size_t variation, SoundType soundType);


protected:
	static constexpr uint8 kCategoryNone = 0xFF; ///< No category assigned.
	static constexpr uint8 kLayerNone    = 0xFF; ///< No layer assigned.

	/** Special value to mean "loop forever". */
	static constexpr uint16 kLoopCountInfinite = 0xFFFF;

	static constexpr size_t kWeightMinimum = 0;     ///< Absolute lower bound on variation weights.
	static constexpr size_t kWeightMaximum = 10000; ///< Absolute upper bound on variation weights

	/** Sound inded to denote silence or stopped. */
	static constexpr size_t kSoundSilence = SIZE_MAX;

	/** The type of an event. */
	enum class EventType {
		Play              = 0x00, ///< Just play a wave, from start to finish.
		PlayComplex       = 0x01, ///< Play a wave, with parameters.
		Stop              = 0x03, ///< Stop playing.
		Pitch             = 0x04, ///< Set the pitch.
		Volume            = 0x05, ///< Set the volume.
		LowPass           = 0x07, ///< Low-pass filter.
		LFOPitch          = 0x08, ///< Low-frequency oscillator on the pitch.
		LFOMulti          = 0x09, ///< Low-frequency oscillator on the pitch and amplitude.
		EnvelopeAmplitude = 0x0A, ///< DAHDSR envelope on the amplitude.
		EnvelopePitch     = 0x0B, ///< DAHDSR envelope on the pitch.
		Loop              = 0x0C, ///< Set loop parameters.
		Marker            = 0x0E,
		Disabled          = 0x0F, ///< A disabled event. Should be ignored.
		MixBins           = 0x10, ///< Set a separate volume for each channel.
		EnvironmentReverb = 0x11, ///< Environmental reverb.
		MixBinsPan        = 0x12  ///< Set channel volumes according to a listener orientation.
	};

	/** How to select a cue or wave variation. */
	enum class SelectMethod {
		RandomNoRepeats   = 0x00, ///< Random, but no immediate repeats.
		Ordered           = 0x01, ///< One after the other, in order.
		Shuffle           = 0x02, ///< Random, no repeats at all.
		Parameter         = 0x03, ///< Game-controlled.
		Random            = 0x04, ///< Completely random.
		OrderedFromRandom = 0x05, ///< Start with a random entry, then in order.
	};

	enum class Mode3D {
		Normal       = 0,
		HeadRelative = 1,
		Disabled     = 2
	};

	enum class CrossfadeType {
		Disabled    = 0,
		Linear      = 1,
		Logarithmic = 2
	};

	enum class TransitionSource {
		Immediate    = 0x00, ///< Transition immediately.
		Marker       = 0x01, ///< Transition on a marker within a range.
		RandomMarker = 0x02, ///< Transition on a random marker within a range.
		EndOfLoop    = 0x04, ///< Transition on a loop end.
		EndOfSound   = 0x08  ///< Transition at the end of the sound.
	};

	enum class TransitionDestination {
		Beginning     = 0x00, ///< Transition to the beginning.
		AlignedTime   = 0x01, ///< Transition to the same time.
		AlignedMarker = 0x02, ///< Transition to the same marker.
		Marker        = 0x04, ///< Transition to a marker within a range.
		RandomMarker  = 0x08  ///< Transition to a random marker within a range.
	};

	enum class TransitionEffect {
		None                       = 0x00, ///< No effect.
		Crossfade                  = 0x01, ///< Crossfade.
		Sound                      = 0x02, ///< Use a transitional sound.
		SoundFadeTo                = 0x03, ///< Fade into a transitional sound.
		SoundFadeFrom              = 0x06, ///< Fade from a transitional sound.
		SoundFadeToFrom            = 0x07  ///< Fade into and from a transitional sound.
	};

	struct Parameters3D {
		Mode3D mode { Mode3D::Normal };

		uint16 coneInsideAngle  { 360 }; ///< Angle of the inside cone.
		uint16 coneOutsideAngle { 360 }; ///< Angle of the outside cone.

		/** Attenuation in dB (-64.0f to 0.0f) for sounds outside the ouside angle. */
		float coneOutsideVolume { 0.0f };

		float distanceMin {      1.0f }; ///< Minimum distance.
		float distanceMax { 100000.0f }; ///< Maximum distance.

		float distanceFactor { 1.0f }; ///< Factor that scales the distance of the sound.
		float rollOffFactor  { 1.0f }; ///< Speed of the roll-off attenuation.
		float dopplerFactor  { 1.0f }; ///< Increase or decrease the doppler effect.

		std::vector<float> rollOffCurve; ///< Custom roll-off curve.

		float volumeLFE   { 0.0f }; ///< Low-frequency effect volume in dB (-64.0f to 0.0f).
		float volumeI3DL2 { 0.0f }; ///< I3DL2 reverb volume in dB (-64.0f to 0.0f).
	};

	/** A reference to an XACT wavebank used by the soundbank. */
	struct WaveBank {
		Common::UString name; ///< File name, without extension.

		XACTWaveBank *bank { nullptr };

		WaveBank(const Common::UString &n = "") : name(n) { }
		~WaveBank();
	};

	/** An event within a sound track. */
	struct Event {
		EventType type;
		uint32 timestamp { 0 }; ///< Timestamp of the event, in milliseconds.

		union {
			struct {
				bool isRelative;      ///< Relative to the current pitch or absolute?
				bool enableFade;      ///< Fade from pitchStart to pitchEnd.
				bool enableVariation; ///< Randomly vary between pitchStart and pitchEnd.

				uint16 fadeStepCount; ///< Number of steps during a fade.
				uint32 fadeDuration;  ///< Number of milliseconds a fade should last.

				float pitchStart; ///< Lower bound or starting value in semitones (-24.0f to 24.0f).
				float pitchEnd;   ///< Upper bound or ending value in semitones (-24.0f to 24.0f).
			} pitch;

			struct {
				bool isRelative;      ///< Relative to the current volume or absolute?
				bool enableFade;      ///< Fade from volumeStart to volumeEnd.
				bool enableVariation; ///< Randomly vary between volumeStart and volumeEnd.

				uint16 fadeStepCount; ///< Number of steps during a fade.
				uint32 fadeDuration;  ///< Number of milliseconds a fade should last.

				float volumeStart; ///< Lower bound or starting value in dB (-64.0f to 64.0f).
				float volumeEnd;   ///< Upper bound or ending value in dB (-64.0f to 64.0f).
			} volume;

			struct {
				bool isRelative;  ///< Relative to the current volume or absolute?
				bool random;      ///< Randomize values.
				bool sweepCutOff; ///< Sweep cut-off frequency.

				uint16 sweepStepCount; ///< Number of steps during a sweep.
				uint32 sweepDuration;  ///< Number of milliseconds a sweep should last.

				float resonanceStart; ///< Lower bound of the resonance in dB (0.0f to 32.0f).
				float resonanceEnd;   ///< Upper bound of the resonance in dB (0.0f to 32.0f).

				uint16 cutOffStart; ///< Lower bound or starting value of cut-off frequency in Hz (0 to 8192).
				uint16 cutOffEnd;   ///< Upper bound or ending value of cut-off frequency in Hz (0 to 8192).
			} lowpass;

			struct {
				float delta;     ///< Frequency delta in Hz (0.0f to 23.4f).
				float pitch;     ///< Pitch modulation in semitones (-12.0f to 12.0f).
			} lfopitch;

			struct {
				float delta;     ///< Frequency delta in Hz (0.0f to 23.4f).
				float pitch;     ///< Pitch modulation in semitones (-12.0f to 12.0f).
				float filter;    ///< Filter modulation in semitones (-96.0f to 96.0f).
				float amplitude; ///< Amplitude modulation in dB (-16.0f to 16.0f).
			} lfomulti;

			struct {
				uint32 delay;   ///< Delay length in milliseconds.
				uint32 attack;  ///< Attack length in milliseconds.
				uint32 hold;    ///< Hold length in milliseconds.
				uint32 decay;   ///< Deay length in milliseconds.
				uint32 release; ///< Release length in milliseconds.

				float sustain; ///< Sustain power in fractions (0.0f to 1.0f).
			} aeg;

			struct {
				uint32 delay;   ///< Delay length in milliseconds.
				uint32 attack;  ///< Attack length in milliseconds.
				uint32 hold;    ///< Hold length in milliseconds.
				uint32 decay;   ///< Deay length in milliseconds.
				uint32 release; ///< Release length in milliseconds.

				float sustain; ///< Sustain power in fractions (0.0f to 1.0f).

				float pitch;  ///< Pitch scale in semitones (-12.0 to 12.0f).
				float filter; ///< Filter cut-off in semitones (-96.0 to 96.0f).
			} pfeg;

			struct {
				uint16 count;
			} loop;

			struct {
				bool   repeat;         ///< Repeat the marker?
				uint16 repeatCount;    ///< Number of times the marker should occur.
				uint32 repeatDuration; ///< Total duration of the marker.

				uint32 value; ///< Custom, user-set value of the marker.
			} marker;

			struct {
				struct {
					uint8 channel; ///< Index of the channel to set the volume of.
					float volume;  ///< Attenuation in dB (-64.0f to 0.0f).
				} bins[8];
			} mixbins;

			struct {
				float room;              ///< Room effect attenuation in dB (-1.0f to 0.0).
				float roomHF;            ///< High-frequency room effect attenuation in dB (-1.0f to 0.0).
				float roomRollOffFactor; ///< Roll-off factor for the room effect.

				uint32 decay;        ///< Decay time in milliseconds.
				float  decayHFRatio; ///< High-frequency to low-frequency decay time ratio.

				float reflection;       ///< (Early) Reflection attenuation in dB (-1.0f to 0.0).
				uint32 reflectionDelay; ///< (Early) Reflection delay in milliseconds.

				float reverb;       ///< (Late) Reverb attenuation in dB (-1.0f to 0.0).
				uint32 reverbDelay; ///< (Late) Reverb delay in milliseconds.

				float diffusion; ///< Echo density in (late) reverb decay in fractions (0.0f to 1.0f).
				float density;   ///< Modal density in (late) reverb decay in fractions (0.0f to 1.0f).

				uint16 referenceHF; ///< Reference high frequency in Hz (20 to 20000).
			} envreverb;

			struct {
				bool use3D; // Use 3D mixbins?

				uint16 angleStart;
				uint16 angleEnd;

				uint8 speakerChannelCount; // Number of channels in the speaker configuration.

				struct {
					uint8 channel; ///< Index of the channel to set the volume of.
					float volume;  ///< Attenuation in dB (-64.0f to 0.0f).
				} bins[4];

			} mixbinspan;

		} params;

		Event(EventType t);
	};

	/** A wave within a sound track. */
	struct WaveVariation {
		Common::UString name; ///< Name of the wave. Can be empty.

		Common::UString bank;           ///< Name of the wavebank the wave is in.
		size_t index { kSoundSilence }; ///< Index into the wavebank to the wave.

		size_t weightMin { kWeightMinimum }; ///< Lower bound of this variation's weight.
		size_t weightMax { kWeightMaximum }; ///< Upper bound of this variation's weight.
	};

	typedef std::vector<Event> Events;
	typedef std::vector<WaveVariation> WaveVariations;

	/** A track within a sound. */
	struct Track {
		/** How a wave variation to be played is selected. */
		SelectMethod variationSelectMethod { SelectMethod::Ordered };

		Events events;        ///< All the events in the track.
		WaveVariations waves; ///< All the waves in the track.
	};

	typedef std::vector<Track> Tracks;

	struct Sound {
		Common::UString name; ///< Name of the sound. Can be empty.

		Common::UString categoryName;          ///< Name of the category. Can be empty.
		uint8 categoryIndex { kCategoryNone }; ///< Index of the category.

		bool gainBoost { false }; ///< Gain boost of 6dB enabled?

		bool loopNewVariation { false }; ///< Select a new wave variation on each loop?
		uint16 loopCount { 0 };          ///< Number of times to loop the sound.

		float volume { 0.0f }; ///< Volume attenuation in dB. 0.0f means full volume, -64.0f maximum attenuation.
		float pitch  { 0.0f }; ///< Pitch change in semitones (-24.0f to 24.0f).

		float volumeVariationMin { 0.0f }; ///< Volume variation lower bound in dB (-64.0f to 64.0f).
		float volumeVariationMax { 0.0f }; ///< Volume variation upper bound in dB (-64.0f to 64.0f).

		float pitchVariationMin { 0.0f }; ///< Pitch variation lower bound in semitones (-24.0f to 24.0f).
		float pitchVariationMax { 0.0f }; ///< Pitch variation upper bound in semitones (-24.0f to 24.0f).

		bool linger { false }; ///< Should the sound linger, if it's in an interactive cue?

		uint16 delay { 0 }; ///< Maximum variable delay, in milliseconds.

		uint8 layer { kLayerNone };
		uint8 priority { 255 };

		bool is3D { false };
		Parameters3D params3D;

		bool parametricEQ { false};      ///< Is the parametric EQ enabled?
		float parametricEQGain { 0.0f }; ///< Parametric EQ gain (-1.0f to 4.0f).
		float parametricEQQ { 1.0f };    ///< Parametric EQ Q coefficient (1.0f/2^0 to 1.0f/2^7 in 8 exponential steps).
		uint16 parametricEQFreq { 30 };  ///< Parametric EQ frequency (30 to 8000).

		Tracks tracks; ///< All the tracks in the sound.
	};

	/** A cue variation within a cue. */
	struct CueVariation {
		Common::UString soundName;           ///< Name of the referenced sound. Can be empty.
		size_t soundIndex { kSoundSilence }; ///< Index of the referenced sound.

		size_t weightMin { kWeightMinimum }; ///< Lower bound of this variation's weight.
		size_t weightMax { kWeightMaximum }; ///< Upper bound of this variation's weight.
	};

	typedef std::vector<CueVariation> CueVariations;

	struct Transition {
		size_t from { kSoundSilence }; ///< Sound index to transition from.
		size_t to   { kSoundSilence }; ///< Sound index to transition to.

		TransitionSource sourceWhen { TransitionSource::Immediate }; ///< When to begin transitioning.
		TransitionDestination destinationWhen { TransitionDestination::Beginning }; ///< When to transition to.

		TransitionEffect effect { TransitionEffect::None }; ///< The effect to use during transitioning.

		size_t transitionSound { kSoundSilence }; ///< Sound index to use as a transition.

		uint16 sourceFadeDuration      { 0 }; ///< Source fade-out duration in milliseconds.
		uint16 destinationFadeDuration { 0 }; ///< Destination fade-in duration in milliseconds.

		uint32 sourceMarkerLow  { 0 }; ///< Lower bound of a marker in the source.
		uint32 sourceMarkerHigh { 0 }; ///< Upper bound of a marker in the source.

		uint32 destinationMarkerLow  { 0 }; ///< Lower bound of a marker in the destination.
		uint32 destinationMarkerHigh { 0 }; ///< Upper bound of a marker in the destination.
	};

	typedef std::vector<Transition> Transitions;

	struct ParametersCrossfade {
		CrossfadeType type { CrossfadeType::Disabled };

		uint32 duration  { 0 }; ///< Fade duration in milliseconds.
		uint8  stepCount { 0 }; ///< Number of steps during the fade.

		float volume { 0.0f }; ///< Initial (fade-in) or final (fade-out) attenuation in dB (-64.0f to 0.0f).
	};

	struct Cue {
		Common::UString name; ///< Name of the cue. Can be empty.

		bool sequential   { false }; ///< Queue for sequential playback?
		bool crossfade    { false }; ///< Crossfade this cue in/out?
		bool stopOnStarve { false }; ///< Stop playback on starvation?
		bool interactive  { false }; ///< Is this an interactive cue?

		ParametersCrossfade fadeIn;  ///< Parameters for a crossfade-in.
		ParametersCrossfade fadeOut; ///< Parameters for a crossfade-out.

		/** How a cue variation to be played is selected. */
		SelectMethod variationSelectMethod { SelectMethod::Ordered };

		CueVariations variations; ///< All the cue variations in the cue.

		uint8 transitionTrigger { 0 };
		Transitions transitions;
	};

	typedef std::vector<WaveBank> WaveBanks;
	typedef std::vector<Sound> Sounds;
	typedef std::vector<Cue> Cues;

	typedef std::map<Common::UString, WaveBank *> WaveBankMap;
	typedef std::map<Common::UString, Sound *> SoundMap;
	typedef std::map<Common::UString, Cue *> CueMap;


	Common::UString _name;

	WaveBanks _waveBanks;
	Sounds _sounds;
	Cues _cues;

	WaveBankMap _waveBankMap;
	SoundMap _soundMap;
	CueMap _cueMap;


	const XACTWaveBank *getWaveBank(const Common::UString &name);

	ChannelHandle playCue(Cue &cue, size_t variation, SoundType soundType);
	ChannelHandle playSound(Sound &sound, SoundType soundType);
	ChannelHandle playTrack(Track &track, const Sound &sound, SoundType soundType);
};

} // End of namespace Sound

#endif // SOUND_XACTSOUNDBANK_H

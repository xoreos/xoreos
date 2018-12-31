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
	static const uint8 kCategoryNone = 0xFF; ///< No category assigned.
	static const uint8 kLayerNone    = 0xFF; ///< No layer assigned.

	/** Special value to mean "loop forever". */
	static const uint16 kLoopCountInfinite = 0xFFFF;

	static const size_t kWeightMinimum = 0;     ///< Absolute lower bound on variation weights.
	static const size_t kWeightMaximum = 10000; ///< Absolute upper bound on variation weights

	/** The type of an event. */
	enum EventType {
		kEventTypePlay              = 0x00, ///< Just play a wave, from start to finish.
		kEventTypePlayComplex       = 0x01, ///< Play a wave, with parameters.
		kEventTypeStop              = 0x03, ///< Stop playing.
		kEventTypePitch             = 0x04, ///< Set the pitch.
		kEventTypeVolume            = 0x05, ///< Set the volume.
		kEventTypeLowPass           = 0x07, ///< Low-pass filter.
		kEventTypeLFOPitch          = 0x08, ///< Low-frequency oscillator on the pitch.
		kEventTypeLFOMulti          = 0x09, ///< Low-frequency oscillator on the pitch and amplitude.
		kEventTypeEnvelopeAmplitude = 0x0A, ///< DAHDSR envelope on the amplitude.
		kEventTypeEnvelopePitch     = 0x0B, ///< DAHDSR envelope on the pitch.
		kEventTypeLoop              = 0x0C, ///< Set loop parameters.
		kEventTypeMarker            = 0x0E,
		kEventTypeDisabled          = 0x0F, ///< A disabled event. Should be ignored.
		kEventTypeMixBins           = 0x10, ///< Set a separate volume for each channel.
		kEventTypeEnvironmentReverb = 0x11, ///< Environmental reverb.
		kEventTypeMixBinSpan        = 0x12  ///< Set channel volumes according to a listener orientation.
	};

	/** How to select a cue or wave variation. */
	enum SelectMethod {
		kSelectMethodRandomNoRepeats   = 0x00, ///< Random, but no immediate repeats.
		kSelectMethodOrdered           = 0x01, ///< One after the other, in order.
		kSelectMethodShuffle           = 0x02, ///< Random, no repeats at all.
		kSelectMethodParameter         = 0x03, ///< Game-controlled.
		kSelectMethodRandom            = 0x04, ///< Completely random.
		kSelectMethodOrderedFromRandom = 0x05, ///< Start with a random entry, then in order.
	};

	enum Mode3D {
		kMode3DNormal       = 0,
		kMode3DHeadRelative = 1,
		kMode3DDisabled     = 2
	};

	struct Parameters3D {
		Mode3D mode;

		uint16 coneInsideAngle;   ///< Angle of the inside cone.
		uint16 coneOutsideAngle;  ///< Angle of the outside cone.
		float  coneOutsideVolume; ///< Attenuation in dB (-64.0f to 0.0f) for sounds outside the ouside angle.

		float distanceMin; ///< Minimum distance.
		float distanceMax; ///< Maximum distance.

		float distanceFactor; ///< Factor that scales the distance of the sound.
		float rollOffFactor;  ///< Speed of the roll-off attenuation.
		float dopplerFactor;  ///< Increase or decrease the doppler effect.

		std::vector<float> rollOffCurve; ///< Custom roll-off curve.

		float volumeLFE;   ///< Low-frequency effect volume in dB (-64.0f to 0.0f).
		float volumeI3DL2; ///< I3DL2 reverb volume in dB (-64.0f to 0.0f).

		Parameters3D() :
				mode(kMode3DNormal),
				coneInsideAngle(360), coneOutsideAngle(360), coneOutsideVolume(0.0f),
				distanceMin(1.0f), distanceMax(100000.0f),
				distanceFactor(1.0f), rollOffFactor(1.0f), dopplerFactor(1.0f),
				volumeLFE(0.0f), volumeI3DL2(0.0f) {
		}
	};

	/** A reference to an XACT wavebank used by the soundbank. */
	struct WaveBank {
		Common::UString name; ///< File name, without extension.

		XACTWaveBank *bank;

		WaveBank(const Common::UString &n = "") : name(n), bank(0) { }
		~WaveBank();
	};

	/** An event within a sound track. */
	struct Event {
		EventType type;

		union {
			struct {
				uint16 count;
			} loop;

		} params;

		Event(EventType t) : type(t) { }
	};

	/** A wave within a sound track. */
	struct WaveVariation {
		Common::UString name; ///< Name of the wave. Can be empty.

		Common::UString bank; ///< Name of the wavebank the wave is in.
		size_t index;         ///< Index into the wavebank to the wave.

		size_t weightMin; ///< Lower bound of this variation's weight.
		size_t weightMax; ///< Upper bound of this variation's weight.
	};

	typedef std::vector<Event> Events;
	typedef std::vector<WaveVariation> WaveVariations;

	/** A track within a sound. */
	struct Track {
		/** How a wave variation to be played is selected. */
		SelectMethod variationSelectMethod;

		Events events;        ///< All the events in the track.
		WaveVariations waves; ///< All the waves in the track.
	};

	typedef std::vector<Track> Tracks;

	struct Sound {
		Common::UString name; ///< Name of the sound. Can be empty.

		Common::UString categoryName; ///< Name of the category. Can be empty.
		uint8 categoryIndex;          ///< Index of the category.

		bool gainBoost; ///< Gain boost of 6dB enabled?

		float volume; ///< Volume attenuation in dB. 0.0f means full volume, -64.0f is maximum attenuation.
		float pitch;  ///< Pitch change in semitones.

		float volumeVariationMin; ///< Volume variation lower bound in dB (-64.0f to 64.0f);
		float volumeVariationMax; ///< Volume variation upper bound in dB (-64.0f to 64.0f);

		float pitchVariationMin; ///< Pitch variation lower bound in semitones.
		float pitchVariationMax; ///< Pitch variation upper bound in semitones.

		uint16 delay; ///< Maximum variable delay, in milliseconds.

		uint8 layer;
		uint8 priority;

		bool is3D;
		Parameters3D params3D;

		Tracks tracks; ///< All the tracks in the sound.

		Sound() : categoryIndex(kCategoryNone), gainBoost(false), volume(0.0f), pitch(0.0f),
				volumeVariationMin(0.0f), volumeVariationMax(0.0f),
				pitchVariationMin(0.0f), pitchVariationMax(0.0f),
				delay(0), layer(kLayerNone), priority(255), is3D(false) {
		}
	};

	/** A cue variation within a cue. */
	struct CueVariation {
		Common::UString soundName; ///< Name of the referenced sound. Can be empty.
		size_t soundIndex;         ///< Index of the referenced sound.

		size_t weightMin; ///< Lower bound of this variation's weight.
		size_t weightMax; ///< Upper bound of this variation's weight.
	};

	typedef std::vector<CueVariation> CueVariations;

	struct Transition {
		Common::UString from;
		Common::UString to;

		Common::UString style;

		Common::UString parameters[5];
	};

	typedef std::vector<Transition> Transitions;

	struct Cue {
		Common::UString name; ///< Name of the cue. Can be empty.

		/** How a cue variation to be played is selected. */
		SelectMethod variationSelectMethod;

		CueVariations variations; ///< All the cue variations in the cue.
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

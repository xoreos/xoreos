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
 *  See also xactsoundbank_binary.h for the original Xbox version of
 *  the XSB file and xactsoundbank_ascii.h for the textual representation.
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
	static const uint16 kLoopCountInfinite = 0xFFFF;

	enum EventType {
		kEventTypePlay              = 0x00,
		kEventTypePlayComplex       = 0x01,
		kEventTypeStop              = 0x03,
		kEventTypePitch             = 0x04,
		kEventTypeVolume            = 0x05,
		kEventTypeLowPass           = 0x07,
		kEventTypeLFOPitch          = 0x08,
		kEventTypeLFOMulti          = 0x09,
		kEventTypeEnvelopeAmplitude = 0x0A,
		kEventTypeEnvelopePitch     = 0x0B,
		kEventTypeLoop              = 0x0C,
		kEventTypeMarker            = 0x0E,
		kEventTypeDisabled          = 0x0F,
		kEventTypeMixBins           = 0x10,
		kEventTypeEnvironmentReverb = 0x11,
		kEventTypeMixBinSpan        = 0x12
	};

	struct WaveBank {
		Common::UString name;

		XACTWaveBank *bank;

		WaveBank(const Common::UString &n = "") : name(n), bank(0) { }
		~WaveBank();
	};

	struct Category {
		Common::UString name;
	};

	struct Event {
		EventType type;

		union {
			struct {
				uint16 count;
			} loop;

		} params;

		Event(EventType t) : type(t) { }
	};

	struct WaveVariation {
		Common::UString name;
		Common::UString bank;

		size_t index;
	};

	typedef std::vector<Event> Events;
	typedef std::vector<WaveVariation> WaveVariations;

	struct Track {
		uint8 variationSelectMethod;

		Events events;
		WaveVariations waves;
	};

	typedef std::vector<Track> Tracks;

	struct Sound {
		Common::UString name;

		Category *category;

		Tracks tracks;
	};

	struct CueVariation {
		Common::UString soundName;
		size_t soundIndex;

		uint32 weightMin;
		uint32 weightMax;
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
		Common::UString name;

		uint8 variationSelectMethod;

		CueVariations variations;
		Transitions transitions;
	};

	typedef std::vector<WaveBank> WaveBanks;
	typedef std::vector<Category> Categories;
	typedef std::vector<Sound> Sounds;
	typedef std::vector<Cue> Cues;

	typedef std::map<Common::UString, WaveBank *> WaveBankMap;
	typedef std::map<Common::UString, Sound *> SoundMap;
	typedef std::map<Common::UString, Cue *> CueMap;


	Common::UString _name;

	WaveBanks _waveBanks;
	Categories _categories;
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

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

#ifndef SOUND_XACTSOUNDBANK_BINARY_H
#define SOUND_XACTSOUNDBANK_BINARY_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/sound/xactsoundbank.h"

namespace Common {
	class SeekableReadStream;
}

namespace Sound {

/** Class to hold audio playback information of an XSB soundbank file.
 *
 *  An XSB file is a soundbank, a collection of sound definitions and
 *  cues. They're commonly used together with XSB files, which contain
 *  the actual audio data.
 *
 *  XSB files are found in the Xbox version of Jade Empire.
 *
 *  Only version 11 of the XSB format is supported, because that's the
 *  version used by Jade Empire.
 *
 *  Interestingly enough, the non-Xbox versions of Jade Empire do not
 *  use XSB files, instead opting for an ASCII representation of the
 *  same information. See xactwavebank_ascii.h for this variant.
 *
 *  See also xactsoundbank.h for the abstract XACT SoundBank interface,
 *  and xactwavebank.h for the abstract XACT WaveBank interface.
 */
class XACTSoundBank_Binary : public XACTSoundBank {
public:
	XACTSoundBank_Binary(Common::SeekableReadStream &xsb);
	virtual ~XACTSoundBank_Binary() = default;

private:
	void load(Common::SeekableReadStream &xsb);

	void readCueVarations(Common::SeekableReadStream &xsb, Cue &cue, uint32_t offset);

	void addWaveVariation(Track &track, uint32_t indices, uint32_t weightMin, uint32_t weightMax);
	void readWaveVariations(Common::SeekableReadStream &xsb, Track &track, uint32_t offset);

	void readComplexTrack(Common::SeekableReadStream &xsb, Track &track, Sound &sound);
	void readTracks(Common::SeekableReadStream &xsb, Sound &sound, uint32_t indicesOrOffset,
	                uint32_t count, uint8_t flags);

	void readWaveBanks(Common::SeekableReadStream &xsb, uint32_t offset, uint32_t count);
	void readCues(Common::SeekableReadStream &xsb, uint32_t xsbFlags, uint32_t offset, uint32_t count,
	              uint32_t offsetFadeParams);
	void readSounds(Common::SeekableReadStream &xsb, uint32_t offset, uint32_t count, uint32_t offset3DParams);
};

} // End of namespace Sound

#endif // SOUND_XACTSOUNDBANK_BINARY_H

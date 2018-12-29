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
 *  An abstract XACT WaveBank, containing sound files.
 */

#ifndef SOUND_XACTWAVEBANK_H
#define SOUND_XACTWAVEBANK_H

#include "src/common/types.h"

namespace Common {
	class UString;
}

namespace Sound {

class RewindableAudioStream;

/** An abstract XACT WaveBank interface.
 *
 *  This interface allows us to treat both binary and ASCII variants
 *  of the XWB file the same.
 *
 *  See also xactwavebank_binary.h for the original Xbox version of
 *  the XWB file and xactwavebank_ascii.h for the textual representation.
 *
 *  For the abstract SoundBank interface, see xactsoundbank.h.
 */
class XACTWaveBank {
public:
	virtual ~XACTWaveBank() { }

	/** Return the internal name of the WaveBank. */
	virtual const Common::UString &getName() const = 0;
	/** Is this WaveBank rating for streaming, or in-memory play? */
	virtual bool isStreaming() const = 0;

	/** Return the number of wave files. */
	virtual size_t getWaveCount() const = 0;

	/** Return the audio stream of a wave. */
	virtual RewindableAudioStream *getWave(size_t index) const = 0;

	/** Load an XACT WaveBank, of either ASCII or Binary format. */
	static XACTWaveBank *load(const Common::UString &name);
};

} // End of namespace Sound

#endif // SOUND_XACTWAVEBANK_H

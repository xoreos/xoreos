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

#include "src/common/types.h"

namespace Common {
	class UString;
}

namespace Sound {

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

	/** Return the internal name of the SoundBank. */
	virtual const Common::UString &getName() const = 0;

	/** Load an XACT SoundBank, of either ASCII or Binary format. */
	static XACTSoundBank *load(const Common::UString &name);
};

} // End of namespace Sound

#endif // SOUND_XACTSOUNDBANK_H

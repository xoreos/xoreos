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

class XACTSoundBank_Binary : public XACTSoundBank {
public:
	XACTSoundBank_Binary(Common::SeekableReadStream &xsb);

private:
	void load(Common::SeekableReadStream &xsb);
};

} // End of namespace Sound

#endif // SOUND_XACTSOUNDBANK_BINARY_H

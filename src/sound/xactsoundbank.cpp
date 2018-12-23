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
#include "src/common/ustring.h"

#include "src/aurora/resman.h"

#include "src/sound/xactsoundbank.h"
#include "src/sound/xactsoundbank_ascii.h"
#include "src/sound/xactsoundbank_binary.h"

namespace Sound {

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

} // End of namespace Sound

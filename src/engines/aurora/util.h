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
 *  Generic Aurora engines utility functions.
 */

#ifndef ENGINES_AURORA_UTIL_H
#define ENGINES_AURORA_UTIL_H

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/sound/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {
	class GFF3File;
}

namespace Engines {

/** Play this video resource. */
void playVideo(const Common::UString &video);

/** Play this sound resource. */
Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType,
		bool loop = false, float volume = 1.0f, bool pitchVariance = false);

/** Make sure that an int config value is in the right range. */
void checkConfigInt   (const Common::UString &key, int    min, int    max);
/** Make sure that a double config value is in the right range. */
void checkConfigDouble(const Common::UString &key, double min, double max);

/** Wait for a "long" amount of time, returning prematurely with true in the case of a requested quit. */
bool longDelay(uint32_t ms);

/** Load a GFF3, but return 0 instead of throwing on error. */
Aurora::GFF3File *loadOptionalGFF3(const Common::UString &gff3, Aurora::FileType type,
                                   uint32_t id = 0xFFFFFFFF, bool repairNWNPremium = false);

/** Load a GFF4, but return 0 instead of throwing on error. */
Aurora::GFF4File *loadOptionalGFF4(const Common::UString &gff4, Aurora::FileType fileType,
                                   uint32_t type = 0xFFFFFFFF);

/** Debug method to quickly dump the current list of resource to disk. */
bool dumpResList(const Common::UString &name);

/** Debug method to quickly dump a stream to disk. */
bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);
/** Debug method to quickly dump a resource to disk. */
bool dumpResource(const Common::UString &name, Aurora::FileType type, const Common::UString &file = "");
/** Debug method to quickly dump a resource to disk. */
bool dumpResource(const Common::UString &name, const Common::UString &file = "");

/** Debug method to quickly dump an image resource to disk. */
bool dumpTGA(const Common::UString &name, const Common::UString &file = "");

/** Debug method to quickly dump a 2DA to disk. */
bool dump2DA(const Common::UString &name, const Common::UString &file = "");

} // End of namespace Engines

#endif // ENGINES_AURORA_UTIL_H

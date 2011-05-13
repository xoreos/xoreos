/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/aurora/util.h
 *  Generic Aurora engines utility functions.
 */

#ifndef ENGINES_AURORA_UTIL_H
#define ENGINES_AURORA_UTIL_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "sound/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {
	class GFFFile;
}

namespace Engines {

/** Play this video resource. */
void playVideo(const Common::UString &video);

/** Play this sound resource. */
Sound::ChannelHandle playSound(const Common::UString &sound, Sound::SoundType soundType,
		bool loop = false, float volume = 1.0);

/** Make sure that an int config value is in the right range. */
void checkConfigInt   (const Common::UString &key, int    min, int    max, int    def);
/** Make sure that a double config value is in the right range. */
void checkConfigDouble(const Common::UString &key, double min, double max, double def);

/** Wait for a "long" amount of time, returning prematurely with true in the case of a requested quit. */
bool longDelay(uint32 ms);

/** Load a GFF file. */
void loadGFF(Aurora::GFFFile &gff, Common::SeekableReadStream &stream, uint32 id);
/** Load a GFF file. */
void loadGFF(Aurora::GFFFile &gff, const Common::UString &name, Aurora::FileType type, uint32 id);
/** Load a GFF file. */
Aurora::GFFFile *loadGFF(const Common::UString &name, Aurora::FileType type, uint32 id);

/** Debug method to quickly dump the current list of resource to disk. */
bool dumpResList(const Common::UString &name);

/** Debug method to quickly dump a stream to disk. */
bool dumpStream(Common::SeekableReadStream &stream, const Common::UString &fileName);
/** Debug method to quickly dump a resource to disk. */
bool dumpResource(const Common::UString &name, Aurora::FileType type, Common::UString file = "");
/** Debug method to quickly dump a resource to disk. */
bool dumpResource(const Common::UString &name, const Common::UString &file = "");

/** Debug method to quickly dump an image resource to disk. */
bool dumpTGA(const Common::UString &name);

/** Debug method to quickly dump a 2DA to disk. */
bool dump2DA(const Common::UString &name);

} // End of namespace Engines

#endif // ENGINES_AURORA_UTIL_H

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
 *  Loader for the savenfo.res file, storing savegame metadata for kotor 1 and 2.
 */

#ifndef AURORA_NFOFILE_H
#define AURORA_NFOFILE_H

#include "src/aurora/gff3file.h"

namespace Aurora {

class NFOFile {
public:
	NFOFile(Common::SeekableReadStream *stream);
	~NFOFile();

	/** Get the player portrait. */
	const Common::UString &getPortrait0() const;
	/** Get the first companions portrait. */
	const Common::UString &getPortrait1() const;
	/** Get the second companions portrait. */
	const Common::UString &getPortrait2() const;

	/** Get the time played. */
	uint32_t getTimePlayed() const;

	/** Get the area name. */
	const Common::UString &getSaveName() const;
	/** Get the area name. */
	const Common::UString &getAreaName() const;

	/** Get the pc name (kotor2 only). */
	const Common::UString &getPCName() const;

private:
	void load(const GFF3Struct &top);

	uint32_t _timePlayed; ///< The time this save was actually played.

	Common::UString _saveName; ///< The save name.
	Common::UString _areaName; ///< The current area to load.
	Common::UString _pcName; ///< The name of the pc (kotor2 only).

	Common::UString _portrait0; ///< The portrait of the pc or solo companion.
	Common::UString _portrait1; ///< The portrait of the first companion.
	Common::UString _portrait2; ///< The portrait of the second companion.
};

} // End of namespace Aurora

#endif // AURORA_NFOFILE_H

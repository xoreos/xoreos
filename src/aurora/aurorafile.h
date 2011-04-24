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

/** @file aurora/aurorafile.h
 *  Utility class for handling special data structures found in BioWare's Aurora files.
 */

#ifndef AURORA_AURORAFILE_H
#define AURORA_AURORAFILE_H

#include <string>

namespace Common {
	class SeekableReadStream;
	class UString;
}

namespace Aurora {

/** Utility class for handling data found in Aurora files. */
class AuroraFile {
public:
	/** Clean up a path string for portable use. */
	static void cleanupPath(Common::UString &path);
};

/** Base class for most files found in games using BioWare's Aurora engine. */
class AuroraBase {
public:
	AuroraBase();

	void clear();

	/** Return the file's ID. */
	uint32 getID() const;

	/** Return the file's version. */
	uint32 getVersion() const;

	/** Were the ID and version encoded in little-endian UTF-16 in the file? */
	bool isUTF16LE() const;

protected:
	uint32 _id;      ///< The file's ID.
	uint32 _version; ///< The file's version.
	bool   _utf16le; ///< The file's ID and version are in little-endian UTF-16.

	void readHeader(Common::SeekableReadStream &stream);

	uint32 convertUTF16LE(uint32 x1, uint32 x2);
};

} // End of namespace Aurora

#endif // AURORA_AURORAFILE_H

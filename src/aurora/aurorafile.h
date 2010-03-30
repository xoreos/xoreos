/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_AURORAFILE_H
#define AURORA_AURORAFILE_H

#include <string>

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Utility class for handling data found in Aurora files. */
class AuroraFile {
public:
	/** Read a raw, not \\0-terminated string from the stream.
	 *
	 *  @param  stream The stream from where to read.
	 *  @param  length The length of the string in bytes.
	 *  @return The string.
	 */
	static std::string readRawString(Common::SeekableReadStream &stream, uint32 length);

	/** Read a raw, not \\0-terminated string from the stream at a given position.
	 *
	 *  Seeks the stream to the position, reads the string and seeks back to the
	 *  the original position.
	 *
	 *  @param  stream The stream from where to read.
	 *  @param  length The length of the string in bytes.
	 *  @param  offset The offset from where to read.
	 *  @return The string.
	 */
	static std::string readRawString(Common::SeekableReadStream &stream, uint32 length, uint32 offset);

	/** Read a float (IEEE 754-1985)
	 */
	static float readFloat(Common::SeekableReadStream &stream);

	/** Read a double (IEEE 754-1985)
	 */
	static double readDouble(Common::SeekableReadStream &stream);

	/** Clean up a path string for portable use. */
	static void cleanupPath(std::string &path);
};

} // End of namespace Aurora

#endif // AURORA_AURORAFILE_H

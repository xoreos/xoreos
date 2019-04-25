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
 *  Writing BioWare's ERFs (encapsulated resource file).
 */

#ifndef AURORA_ERFWRITER_H
#define AURORA_ERFWRITER_H

#include "src/common/writestream.h"
#include "src/common/readstream.h"

#include "src/aurora/locstring.h"

namespace Aurora {

class ERFWriter {
public:
	enum Version {
		kERFVersion10,
		kERFVersion20
	};

	/** Create an ERF writer by writing the header to the stream and reserve fileCount
	 *  places in the key and resource table.
	 *
	 *  @param id The FourCC for the archive.
	 *  @param fileCount The number of files which should be contained in the archive.
	 *  @param stream The write stream in which the archive should be written.
	 *  @param version The ERF version to write
	 *  @param description The LocString, that should be used for the description.
	 */
	ERFWriter(uint32 id, uint32 fileCount, Common::SeekableWriteStream &stream,
	          Version version = kERFVersion10, LocString description = LocString());
	~ERFWriter();

	/** Add a new stream to this archive to be packed. */
	void add(const Common::UString &resRef, FileType resType, Common::ReadStream &stream);

private:
	Common::SeekableWriteStream &_stream;

	const Version _version;

	uint32 _currentFileCount;
	uint32 _fileCount;
	uint32 _offsetToResourceData;
	uint32 _keyTableOffset;
	uint32 _resourceTableOffset;
};

} // End of namespace Aurora

#endif // AURORA_ERFWRITER_H

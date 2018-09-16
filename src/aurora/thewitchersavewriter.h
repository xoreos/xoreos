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
 *  Writer for writing TheWitcherSave files.
 */

#ifndef AURORA_THEWITCHERSAVEWRITER_H
#define AURORA_THEWITCHERSAVEWRITER_H

#include "src/common/writestream.h"

namespace Aurora {

class TheWitcherSaveWriter {
public:
	/** Create a new TheWitcherSave writer.
	 *
	 *  @param areaName Set the area name which should be written
	 *  into the header.
	 *  @param stream The stream to write the file to.
	 */
	TheWitcherSaveWriter(const Common::UString &areaName, Common::SeekableWriteStream &stream);

	/** Add a file to this TheWitcherSave archive.
	 *
	 *  @param fileName The filename of the stream to write
	 *  @param stream The stream of the
	 */
	void add(const Common::UString &fileName, Common::ReadStream &stream);

	/** Finish the stream and write the file table at the
	 *  end of the stream, and set the finished flag to prevent
	 *  further adds.
	 */
	void finish();

private:
	struct Resource {
		Common::UString name;
		size_t size, offset;
	};

	Common::SeekableWriteStream &_stream;

	bool _finished;
	std::vector<Resource> _resources;
};

} // End of namespace Aurora

#endif // AURORA_THEWITCHERSAVEWRITER_H

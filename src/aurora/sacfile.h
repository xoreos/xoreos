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
 *  Handling GFF3 files with SAC header.
 */

#ifndef AURORA_SACFILE_H
#define AURORA_SACFILE_H

#include "src/common/readstream.h"

#include "src/aurora/gff3file.h"

namespace Aurora {

/**
 * A SAC file is basically a GFF3 file with some bytes extra header. This
 * class inherits from the GFF3File class to offer all GFF access methods
 * while providing also information about the SAC header.
 */
class SACFile : public GFF3File {
public:
	SACFile(Common::SeekableReadStream *stream);

	Common::UString getLevelFile() const;

private:
	Common::UString _levelFile;
	Common::ScopedPtr<Common::SeekableReadStream> _stream;

	Common::SeekableReadStream *load(Common::SeekableReadStream *stream);
};

} // End of namespace Aurora

#endif // AURORA_SACFILE_H

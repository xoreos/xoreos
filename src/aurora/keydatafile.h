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
 *  An abstract KEY data file (BIF or BZF).
 */

#ifndef AURORA_KEYDATAFILE_H
#define AURORA_KEYDATAFILE_H

#include "src/common/types.h"

#include "src/aurora/archive.h"

namespace Aurora {

class KEYFile;

class KEYDataFile : public Archive {
public:
	virtual ~KEYDataFile() { }

	/** Merge information from the KEY into the data file.
	 *
	 *  Without this step, this data file archive does not contain any
	 *  resource names at all.
	 *
	 *  @param key A KEYFile with information about this data file.
	 *  @param dataFileIndex The index this data file has within the KEY file.
	 */
	virtual void mergeKEY(const KEYFile &key, uint32_t dataFileIndex) = 0;
};

} // End of namespace Aurora

#endif // AURORA_KEYDATAFILE_H

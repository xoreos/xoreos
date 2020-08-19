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
 *  Handling BioWare's KEYs (resource index files).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"

#include "src/aurora/keyfile.h"

static const uint32_t kKEYID     = MKTAG('K', 'E', 'Y', ' ');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32_t kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

KEYFile::KEYFile(Common::SeekableReadStream &key) {
	load(key);
}

KEYFile::~KEYFile() {
}

void KEYFile::load(Common::SeekableReadStream &key) {
	readHeader(key);

	if (_id != kKEYID)
		throw Common::Exception("Not a KEY file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported KEY file version %s", Common::debugTag(_version).c_str());

	uint32_t bifCount = key.readUint32LE();
	uint32_t resCount = key.readUint32LE();

	_bifs.reserve(bifCount);
	_resources.reserve(resCount);

	// Version 1.1 has some NULL bytes here
	if (_version == kVersion11)
		key.skip(4);

	uint32_t offFileTable     = key.readUint32LE();
	uint32_t offResTable      = key.readUint32LE();

	key.skip( 8); // Build year and day
	key.skip(32); // Reserved

	try {

		_bifs.resize(bifCount);
		readBIFList(key, offFileTable);

		_resources.resize(resCount);
		readResList(key, offResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading KEY file");
		throw;
	}

}

void KEYFile::readBIFList(Common::SeekableReadStream &key, uint32_t offset) {
	key.seek(offset);

	for (BIFList::iterator bif = _bifs.begin(); bif != _bifs.end(); ++bif) {
		key.skip(4); // File size of the bif

		uint32_t nameOffset = key.readUint32LE();
		uint32_t nameSize   = 0;

		// nameSize is expanded to 4 bytes in 1.1 and the location is dropped
		if (_version == kVersion11) {
			nameSize = key.readUint32LE();
		} else {
			nameSize = key.readUint16LE();
			key.skip(2); // Location of the bif (HD, CD, ...)
		}

		const size_t curPos = key.seek(nameOffset);

		*bif = Common::readStringFixed(key, Common::kEncodingASCII, nameSize);

		key.seek(curPos);

		bif->replaceAll('\\', '/');
		if (bif->beginsWith("/"))
			bif->erase(bif->begin());
	}
}

void KEYFile::readResList(Common::SeekableReadStream &key, uint32_t offset) {
	key.seek(offset);

	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++res) {
		res->name = Common::readStringFixed(key, Common::kEncodingASCII, 16);
		res->type = (FileType) key.readUint16LE();

		uint32_t id = key.readUint32LE();

		// The new flags field holds the bifIndex now. The rest contains fixed
		// resource info.
		if (_version == kVersion11) {
			uint32_t flags = key.readUint32LE();
			res->bifIndex = (flags & 0xFFF00000) >> 20;
		} else
			res->bifIndex = id >> 20;

		// TODO: Fixed resources?
		res->resIndex = id & 0xFFFFF;
	}
}

const KEYFile::BIFList &KEYFile::getBIFs() const {
	return _bifs;
}

const KEYFile::ResourceList &KEYFile::getResources() const {
	return _resources;
}

} // End of namespace Aurora

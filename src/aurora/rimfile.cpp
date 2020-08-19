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
 *  Handling BioWare's RIMs (resource archives).
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/memreadstream.h"
#include "src/common/error.h"
#include "src/common/encoding.h"

#include "src/aurora/rimfile.h"

static const uint32_t kRIMID     = MKTAG('R', 'I', 'M', ' ');
static const uint32_t kVersion1  = MKTAG('V', '1', '.', '0');

namespace Aurora {

RIMFile::RIMFile(Common::SeekableReadStream *rim) : _rim(rim) {
	assert(_rim);

	load(*_rim);
}

RIMFile::~RIMFile() {
}

void RIMFile::load(Common::SeekableReadStream &rim) {
	readHeader(rim);

	if (_id != kRIMID)
		throw Common::Exception("Not a RIM file (%s)", Common::debugTag(_id).c_str());

	if (_version != kVersion1)
		throw Common::Exception("Unsupported RIM file version %s", Common::debugTag(_version).c_str());

	rim.skip(4);                            // Reserved
	uint32_t resCount   = rim.readUint32LE(); // Number of resources in the RIM
	uint32_t offResList = rim.readUint32LE(); // Offset to the resource list

	_resources.resize(resCount);
	_iResources.resize(resCount);

	try {

		// Read the resource list
		readResList(rim, offResList);

	} catch (Common::Exception &e) {
		e.add("Failed reading RIM file");
		throw;
	}

}

void RIMFile::readResList(Common::SeekableReadStream &rim, uint32_t offset) {
	rim.seek(offset);

	uint32_t index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		res->name    = Common::readStringFixed(rim, Common::kEncodingASCII, 16);
		res->type    = (FileType) rim.readUint16LE();
		res->index   = index;
		rim.skip(4 + 2); // Resource ID + Reserved
		iRes->offset = rim.readUint32LE();
		iRes->size   = rim.readUint32LE();
	}
}

const Archive::ResourceList &RIMFile::getResources() const {
	return _resources;
}

const RIMFile::IResource &RIMFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t RIMFile::getResourceSize(uint32_t index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *RIMFile::getResource(uint32_t index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	if (tryNoCopy)
		return new Common::SeekableSubReadStream(_rim.get(), res.offset, res.offset + res.size);

	_rim->seek(res.offset);

	return _rim->readStream(res.size);
}

} // End of namespace Aurora

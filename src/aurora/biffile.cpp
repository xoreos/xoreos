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
 *  Handling BioWare's BIFs (resource data files).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"

#include "src/aurora/biffile.h"
#include "src/aurora/keyfile.h"

static const uint32_t kBIFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');
static const uint32_t kVersion11 = MKTAG('V', '1', '.', '1');

namespace Aurora {

BIFFile::BIFFile(Common::SeekableReadStream *bif) : _bif(bif) {
	assert(_bif);

	load(*_bif);
}

BIFFile::~BIFFile() {
}

void BIFFile::load(Common::SeekableReadStream &bif) {
	readHeader(bif);

	if (_id != kBIFID)
		throw Common::Exception("Not a BIF file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion1) && (_version != kVersion11))
		throw Common::Exception("Unsupported BIF file version %s", Common::debugTag(_version).c_str());

	uint32_t varResCount = bif.readUint32LE();
	uint32_t fixResCount = bif.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BIF resources");

	_iResources.resize(varResCount);

	uint32_t offVarResTable = bif.readUint32LE();

	try {

		readVarResTable(bif, offVarResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading BIF file");
		throw;
	}

}

void BIFFile::readVarResTable(Common::SeekableReadStream &bif, uint32_t offset) {
	bif.seek(offset);

	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		bif.skip(4); // ID

		if (_version == kVersion11)
			bif.skip(4); // Flags

		res->offset = bif.readUint32LE();
		res->size   = bif.readUint32LE();
		res->type   = (FileType) bif.readUint32LE();
	}
}

void BIFFile::mergeKEY(const KEYFile &key, uint32_t dataFileIndex) {
	const KEYFile::ResourceList &keyResList = key.getResources();

	for (KEYFile::ResourceList::const_iterator keyRes = keyResList.begin(); keyRes != keyResList.end(); ++keyRes) {
		if (keyRes->bifIndex != dataFileIndex)
			continue;

		if (keyRes->resIndex >= _iResources.size()) {
			warning("Resource index out of range (%d/%d)", keyRes->resIndex, (int) _iResources.size());
			continue;
		}

		if (keyRes->type != _iResources[keyRes->resIndex].type)
			warning("KEY and BIF disagree on the type of the resource \"%s\" (%d, %d). Trusting the BIF",
			        keyRes->name.c_str(), keyRes->type, _iResources[keyRes->resIndex].type);

		Resource res;

		res.name  = keyRes->name;
		res.type  = _iResources[keyRes->resIndex].type;
		res.index = keyRes->resIndex;

		_resources.push_back(res);
	}

}

const Archive::ResourceList &BIFFile::getResources() const {
	return _resources;
}

const BIFFile::IResource &BIFFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t BIFFile::getResourceSize(uint32_t index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *BIFFile::getResource(uint32_t index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	if (tryNoCopy)
		return new Common::SeekableSubReadStream(_bif.get(), res.offset, res.offset + res.size);

	_bif->seek(res.offset);

	return _bif->readStream(res.size);
}

} // End of namespace Aurora

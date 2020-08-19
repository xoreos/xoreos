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
 *  Handling BioWare's BZFs (resource data files), used in the iOS version of
 *  Knights of the Old Republic.
 *
 *  Essentially, they are BIF files with LZMA-compressed data.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#ifdef ENABLE_LZMA
#include "src/common/lzma.h"
#endif

#include "src/aurora/bzffile.h"
#include "src/aurora/keyfile.h"

static const uint32_t kBZFID     = MKTAG('B', 'I', 'F', 'F');
static const uint32_t kVersion1  = MKTAG('V', '1', ' ', ' ');

namespace Aurora {

BZFFile::BZFFile(Common::SeekableReadStream *bzf) : _bzf(bzf) {
	assert(_bzf);

	load(*_bzf);
}

BZFFile::~BZFFile() {
}

void BZFFile::load(Common::SeekableReadStream &bzf) {
	readHeader(bzf);

	if (_id != kBZFID)
		throw Common::Exception("Not a BZF file (%s)", Common::debugTag(_id).c_str());

	if (_version != kVersion1)
		throw Common::Exception("Unsupported BZF file version %s", Common::debugTag(_version).c_str());

	uint32_t varResCount = bzf.readUint32LE();
	uint32_t fixResCount = bzf.readUint32LE();

	if (fixResCount != 0)
		throw Common::Exception("TODO: Fixed BZF resources");

	_iResources.resize(varResCount);

	uint32_t offVarResTable = bzf.readUint32LE();

	try {

		readVarResTable(bzf, offVarResTable);

	} catch (Common::Exception &e) {
		e.add("Failed reading BZF file");
		throw;
	}

}

void BZFFile::readVarResTable(Common::SeekableReadStream &bzf, uint32_t offset) {
	bzf.seek(offset);

	for (uint32_t i = 0; i < _iResources.size(); i++) {
		bzf.skip(4); // ID

		_iResources[i].offset = bzf.readUint32LE();
		_iResources[i].size   = bzf.readUint32LE();
		_iResources[i].type   = (FileType) bzf.readUint32LE();

		if (i > 0)
			_iResources[i - 1].packedSize = _iResources[i].offset - _iResources[i - 1].offset;
	}

	if (!_iResources.empty())
		_iResources.back().packedSize = bzf.size() - _iResources.back().offset;
}

void BZFFile::mergeKEY(const KEYFile &key, uint32_t dataFileIndex) {
	const KEYFile::ResourceList &keyResList = key.getResources();

	for (KEYFile::ResourceList::const_iterator keyRes = keyResList.begin(); keyRes != keyResList.end(); ++keyRes) {
		if (keyRes->bifIndex != dataFileIndex)
			continue;

		if (keyRes->resIndex >= _iResources.size()) {
			warning("Resource index out of range (%d/%d)", keyRes->resIndex, (int) _iResources.size());
			continue;
		}

		if (keyRes->type != _iResources[keyRes->resIndex].type)
			warning("KEY and BZF disagree on the type of the resource \"%s\" (%d, %d). Trusting the BZF",
			        keyRes->name.c_str(), keyRes->type, _iResources[keyRes->resIndex].type);

		Resource res;

		res.name  = keyRes->name;
		res.type  = _iResources[keyRes->resIndex].type;
		res.index = keyRes->resIndex;

		_resources.push_back(res);
	}

}

const Archive::ResourceList &BZFFile::getResources() const {
	return _resources;
}

const BZFFile::IResource &BZFFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t BZFFile::getResourceSize(uint32_t index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *BZFFile::getResource(uint32_t index, bool UNUSED(tryNoCopy)) const {
	const IResource &res = getIResource(index);

	_bzf->seek(res.offset);

#ifdef ENABLE_LZMA
	return Common::decompressLZMA1(*_bzf, res.packedSize, res.size, true);
#else
	throw Common::Exception("LZMA decompression disabled when building without liblzma");
#endif
}

} // End of namespace Aurora

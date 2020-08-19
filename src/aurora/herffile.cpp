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
 *  BioWare's HERF (hashed ERF) file parsing.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filepath.h"
#include "src/common/memreadstream.h"
#include "src/common/encoding.h"
#include "src/common/hash.h"

#include "src/aurora/herffile.h"
#include "src/aurora/util.h"

namespace Aurora {

HERFFile::HERFFile(Common::SeekableReadStream *herf) : _herf(herf), _dictOffset(0xFFFFFFFF), _dictSize(0) {
	assert(_herf);

	load(*_herf);
}

HERFFile::~HERFFile() {
}

void HERFFile::load(Common::SeekableReadStream &herf) {
	uint32_t magic = herf.readUint32LE();
	if (magic != 0x00F1A5C0)
		throw Common::Exception("Invalid HERF file (0x%08X)", magic);

	uint32_t resCount = herf.readUint32LE();

	_resources.resize(resCount);
	_iResources.resize(resCount);

	try {

		searchDictionary(herf, resCount);
		readResList(herf);

	} catch (Common::Exception &e) {
		e.add("Failed reading HERF file");
		throw;
	}
}

void HERFFile::searchDictionary(Common::SeekableReadStream &herf, uint32_t resCount) {
	const uint32_t dictHash = Common::hashStringDJB2("erf.dict");

	const size_t pos = herf.pos();

	for (uint32_t i = 0; i < resCount; i++) {
		uint32_t hash = herf.readUint32LE();
		if (hash == dictHash) {
			_dictSize   = herf.readUint32LE();
			_dictOffset = herf.readUint32LE();
			break;
		}

		herf.skip(8);
	}

	herf.seek(pos);
}

void HERFFile::readDictionary(Common::SeekableReadStream &herf, std::map<uint32_t, Common::UString> &dict) {
	if (_dictOffset == 0xFFFFFFFF)
		return;

	size_t pos = herf.pos();

	herf.seek(_dictOffset);

	uint32_t magic = herf.readUint32LE();
	if (magic != 0x00F1A5C0)
		throw Common::Exception("Invalid HERF dictionary (0x%08X)", magic);

	uint32_t hashCount = herf.readUint32LE();

	for (uint32_t i = 0; i < hashCount; i++) {
		if ((size_t)herf.pos() >= (_dictOffset + _dictSize))
			break;

		uint32_t hash = herf.readUint32LE();
		dict[hash] = Common::readStringFixed(herf, Common::kEncodingASCII, 128).toLower();
	}

	herf.seek(pos);
}

void HERFFile::readResList(Common::SeekableReadStream &herf) {
	std::map<uint32_t, Common::UString> dict;
	readDictionary(herf, dict);

	uint32_t index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		res->index = index;

		res->hash = herf.readUint32LE();

		iRes->size   = herf.readUint32LE();
		iRes->offset = herf.readUint32LE();

		if (iRes->offset >= (uint32_t)herf.size())
			throw Common::Exception("HERFFile::readResList(): Resource goes beyond end of file");

		std::map<uint32_t, Common::UString>::const_iterator name = dict.find(res->hash);
		if (name != dict.end()) {
			res->name = Common::FilePath::getStem(name->second);
			res->type = TypeMan.getFileType(name->second);
		}

		if ((iRes->offset == _dictOffset) && (iRes->size == _dictSize)) {
			res->name = "erf";
			res->type = kFileTypeDICT;
		}
	}
}

const Archive::ResourceList &HERFFile::getResources() const {
	return _resources;
}

const HERFFile::IResource &HERFFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t HERFFile::getResourceSize(uint32_t index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *HERFFile::getResource(uint32_t index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	if (tryNoCopy)
		return new Common::SeekableSubReadStream(_herf.get(), res.offset, res.offset + res.size);

	_herf->seek(res.offset);

	return _herf->readStream(res.size);
}

Common::HashAlgo HERFFile::getNameHashAlgo() const {
	return Common::kHashDJB2;
}

} // End of namespace Aurora

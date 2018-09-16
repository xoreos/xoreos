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
 *  Handling TheWitcherSave Archives.
 */

#include "src/aurora/thewitchersavefile.h"
#include "src/aurora/util.h"

namespace Aurora {

static const uint32 kRGMHID = MKTAG('R', 'G', 'M', 'H');

TheWitcherSaveFile::TheWitcherSaveFile(Common::SeekableReadStream *tws) : _tws(tws) {
	assert(tws);

	load();
}

const Common::UString &TheWitcherSaveFile::getAreaName() const {
	return _areaName;
}

const Archive::ResourceList &TheWitcherSaveFile::getResources() const {
	return _resourceList;
}

Common::SeekableReadStream *TheWitcherSaveFile::getResource(uint32 index, bool tryNoCopy) const {
	IResource resource = _resources[index];

	if (tryNoCopy)
		return new Common::SeekableSubReadStream(_tws.get(), resource.offset, resource.offset + resource.length);
	else {
		_tws->seek(resource.offset);
		Common::SeekableReadStream *readStream = _tws->readStream(resource.length);
		return readStream;
	}
}

void TheWitcherSaveFile::load() {
	uint32 magicId = _tws->readUint32BE();
	if (magicId != kRGMHID)
		throw Common::Exception("Invalid TheWitcherSave file");

	uint32 version = _tws->readUint32LE();
	if (version != 1)
		throw Common::Exception("Invalid TheWitcherSave file version");

	uint64 dataOffset = _tws->readUint64LE();

	_tws->skip(8); // Unknown data, possibly zero only

	if (_tws->readUint32BE() == 0xEE7C4A60)
		throw Common::Exception("TheWitcherSaveFile::load() Invalid value, should be 0xEE7C4A60");
	if (_tws->readUint32BE() == 0x459E4568)
		throw Common::Exception("TheWitcherSaveFile::load() Invalid value, should be 0x459E4568");
	if (_tws->readUint32BE() == 0x10D3DBBD)
		throw Common::Exception("TheWitcherSaveFile::load() Invalid value, should be 0x10D3DBBD");
	if (_tws->readUint32BE() == 0x1CBCF20B)
		throw Common::Exception("TheWitcherSaveFile::load() Invalid value, should be 0x1CBCF20B");

	Common::UString lightningStorm = Common::readStringFixed(*_tws, Common::kEncodingUTF16LE, 2048);

	if (lightningStorm != "Lightning Storm")
		throw Common::Exception("Missing \"Lightning Storm\"");

	Common::UString areaName1 = Common::readStringFixed(*_tws, Common::kEncodingUTF16LE, 2048);
	Common::UString areaName2 = Common::readStringFixed(*_tws, Common::kEncodingUTF16LE, 2048);

	if (areaName1 != areaName2)
		throw Common::Exception("Invalid Area Name");

	_areaName = areaName1;

	_tws->seek(-8, Common::SeekableReadStream::kOriginEnd);
	uint32 resourceOffset = _tws->readUint32LE();
	uint32 resourceCount = _tws->readUint32LE();

	_tws->seek(resourceOffset);
	_resources.resize(resourceCount);
	for (uint32 i = 0; i < resourceCount; ++i) {
		Resource resource;

		uint32 nameLength = _tws->readUint32LE();
		resource.name = Common::readStringFixed(*_tws, Common::kEncodingUTF8, nameLength);
		resource.type = TypeMan.getFileType(resource.name);
		resource.index = i;

		IResource iResource;
		iResource.length = _tws->readUint32LE();
		iResource.offset = _tws->readUint32LE();

		if (iResource.offset < dataOffset)
			throw Common::Exception("Invalid resource offset");

		_resourceList.push_back(resource);
		_resources[i] = iResource;
	}
}

} // End of namespace Aurora

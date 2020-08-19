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
 *  Nintendo DS ROM parsing.
 */

// Based on http://dsibrew.org/wiki/NDS_Format

#include <cassert>

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/readfile.h"
#include "src/common/encoding.h"

#include "src/aurora/ndsrom.h"
#include "src/aurora/util.h"

namespace Aurora {

NDSFile::NDSFile(const Common::UString &fileName) {
	_nds = std::make_unique<Common::ReadFile>(fileName);

	load(*_nds);
}

NDSFile::NDSFile(Common::SeekableReadStream *nds) : _nds(nds) {
	assert(_nds);

	load(*_nds);
}

NDSFile::~NDSFile() {
}

void NDSFile::load(Common::SeekableReadStream &nds) {
	if (!isNDS(nds, _title, _code, _maker))
		throw Common::Exception("Not a supported NDS ROM file");

	nds.seek(0x40);

	uint32_t fileNameTableOffset = nds.readUint32LE();
	uint32_t fileNameTableLength = nds.readUint32LE();
	uint32_t fatOffset           = nds.readUint32LE();
	//uint32_t fatLength = nds.readUint32LE();

	try {

		readNames(nds, fileNameTableOffset, fileNameTableLength);
		readFAT(nds, fatOffset);

	} catch (Common::Exception &e) {
		e.add("Failed reading NDS file");
		throw;
	}

}

void NDSFile::readNames(Common::SeekableReadStream &nds, uint32_t offset, uint32_t length) {
	nds.seek(offset + 8);

	uint32_t index = 0;
	while (((size_t)nds.pos()) < (size_t)(offset + length)) {
		Resource res;

		byte nameLength = nds.readByte();
		if ((nameLength == 0) || ((size_t)nds.pos() >= (size_t)(offset + length)))
			break;

		Common::UString name = Common::readStringFixed(nds, Common::kEncodingASCII, nameLength).toLower();

		res.name  = TypeMan.setFileType(name, kFileTypeNone);
		res.type  = TypeMan.getFileType(name);
		res.index = index++;

		_resources.push_back(res);
	}
}

void NDSFile::readFAT(Common::SeekableReadStream &nds, uint32_t offset) {
	nds.seek(offset);

	_iResources.resize(_resources.size());
	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		res->offset = nds.readUint32LE();
		res->size   = nds.readUint32LE() - res->offset; // Value is the end offset
	}
}

const Common::UString &NDSFile::getTitle() const {
	return _title;
}

const Common::UString &NDSFile::getCode() const {
	return _code;
}

const Common::UString &NDSFile::getMaker() const {
	return _maker;
}

bool NDSFile::isNDS(Common::SeekableReadStream &stream,
                    Common::UString &title, Common::UString &code, Common::UString &maker) {

	if (stream.size() < 0x40)
		return false;

	try {
		stream.seek(0);

		title = Common::readStringFixed(stream, Common::kEncodingASCII, 12);
		code  = Common::readStringFixed(stream, Common::kEncodingASCII,  4);
		maker = Common::readStringFixed(stream, Common::kEncodingASCII,  2);
	} catch (...) {
		return false;
	}

	return true;
}

bool NDSFile::hasResource(Common::UString name) const {
	name.makeLower();

	for (ResourceList::const_iterator r = _resources.begin(); r != _resources.end(); ++r)
		if (TypeMan.setFileType(r->name, r->type) == name)
			return true;

	return false;
}

const Archive::ResourceList &NDSFile::getResources() const {
	return _resources;
}

const NDSFile::IResource &NDSFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t NDSFile::getResourceSize(uint32_t index) const {
	return getIResource(index).size;
}

Common::SeekableReadStream *NDSFile::getResource(uint32_t index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	_nds->seek(res.offset);

	if (tryNoCopy)
		return new Common::SeekableSubReadStream(_nds.get(), res.offset, res.offset + res.size);

	_nds->seek(res.offset);

	return _nds->readStream(res.size);
}

} // End of namespace Aurora

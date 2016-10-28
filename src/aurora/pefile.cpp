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
 *  A portable executable archive.
 */

#include "src/common/ptrvector.h"
#include "src/common/error.h"
#include "src/common/pe_exe.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/pefile.h"

namespace Aurora {

PEFile::PEFile(Common::SeekableReadStream *exe, const std::vector<Common::UString> &remap) {
	assert(exe);

	_peFile.reset(new Common::PEResources(exe));

	load(remap);
}

PEFile::~PEFile() {
}

const Archive::ResourceList &PEFile::getResources() const {
	return _resources;
}

Common::SeekableReadStream *PEFile::getResource(uint32 index, bool UNUSED(tryNoCopy)) const {
	// Convert from the PE cursor group/cursor format to the standalone
	// cursor format.

	Common::ScopedPtr<Common::SeekableReadStream>
		cursorGroup(_peFile->getResource(Common::kPEGroupCursor, index));

	if (!cursorGroup)
		throw Common::Exception("No such PE resource %u", index);


	Common::MemoryWriteStreamDynamic out(true);

	// Cursor Group Header
	out.writeUint16LE(cursorGroup->readUint16LE());
	out.writeUint16LE(cursorGroup->readUint16LE());

	const size_t cursorCount = cursorGroup->readUint16LE();
	out.writeUint16LE(cursorCount);


	Common::PtrVector<Common::SeekableReadStream> cursorStreams;
	cursorStreams.resize(cursorCount);

	uint32 startOffset = 6 + cursorCount * 16;

	for (size_t i = 0; i < cursorCount; i++) {
		out.writeByte(cursorGroup->readUint16LE());     // width
		out.writeByte(cursorGroup->readUint16LE() / 2); // height
		cursorGroup->readUint16LE();                    // planes
		out.writeByte(cursorGroup->readUint16LE());     // bits per pixel
		out.writeByte(0);                               // reserved

		cursorGroup->readUint32LE();                    // data size
		uint16 id = cursorGroup->readUint16LE();

		cursorStreams[i] = _peFile->getResource(Common::kPECursor, id);
		if (!cursorStreams[i])
			throw Common::Exception("Could not get cursor resource %d", id);

		out.writeUint16LE(cursorStreams[i]->readUint16LE()); // hotspot X
		out.writeUint16LE(cursorStreams[i]->readUint16LE()); // hotspot Y
		out.writeUint32LE(cursorStreams[i]->size() - 4);     // size
		out.writeUint32LE(startOffset);                      // offset
		startOffset += cursorStreams[i]->size() - 4;
	}

	for (size_t i = 0; i < cursorCount; i++)
		out.writeStream(*cursorStreams[i], cursorStreams[i]->size() - 4);

	out.setDisposable(false);
	return new Common::MemoryReadStream(out.getData(), out.size(), true);
}

void PEFile::load(const std::vector<Common::UString> &remap) {
	std::vector<Common::PEResourceID> cursorList = _peFile->getNameList(Common::kPEGroupCursor);

	for (std::vector<Common::PEResourceID>::const_iterator it = cursorList.begin(); it != cursorList.end(); ++it) {
		Resource res;

		if (it->getID() == 0xFFFFFFFF)
			throw Common::Exception("Found non-integer cursor group");

		uint32 id = it->getID() - 1;
		if (id >= remap.size())
			res.name = Common::UString::format("cursor%d", id);
		else
			res.name = remap[id];

		res.type  = kFileTypeCUR;
		res.index = id + 1;

		_resources.push_back(res);
	}
}

} // End of namespace Aurora

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
 *  Writer for writing TheWitcherSave files.
 */

#include "src/common/encoding.h"

#include "src/aurora/util.h"
#include "src/aurora/thewitchersavewriter.h"

namespace Aurora {

static const uint32_t kRGMHID = MKTAG('R', 'G', 'M', 'H');

TheWitcherSaveWriter::TheWitcherSaveWriter(const Common::UString &areaName, Common::SeekableWriteStream &stream) :
		_stream(stream), _finished(false) {
	// Write the magic id
	stream.writeUint32BE(kRGMHID);

	// Write the version 1
	stream.writeUint32LE(1);

	// Write the constant header length
	stream.writeUint64LE(8232);

	// Write some zeros
	stream.writeZeros(8);

	// Write some values, which are always the same
	stream.writeUint32LE(0xEE7C4A60);
	stream.writeUint32LE(0x459E4568);
	stream.writeUint32LE(0x10D3DBBD);
	stream.writeUint32LE(0x1CBCF20B);

	// Write Lightning Storm
	Common::writeStringFixed(stream, "Lightning Storm", Common::kEncodingUTF16LE, 2048);

	// Write the current area name two times
	Common::writeStringFixed(stream, areaName, Common::kEncodingUTF16LE, 2048);
	Common::writeStringFixed(stream, areaName, Common::kEncodingUTF16LE, 2048);

	// Write 2048 zeros
	stream.writeZeros(2048);
}

void TheWitcherSaveWriter::add(const Common::UString &resRef, const Aurora::FileType fileType, Common::ReadStream &stream) {
	if (_finished)
		throw Common::Exception("TheWitcherSave::add() Archive is already finished");

	Resource resource;
	resource.name = TypeMan.setFileType(resRef, fileType);
	resource.offset = _stream.pos();
	resource.size = _stream.writeStream(stream);

	_resources.push_back(resource);
}

void TheWitcherSaveWriter::finish() {
	if (_finished)
		throw Common::Exception("TheWitcherSave::finish() Archive is already finished");

	const size_t resourceTableOffset = _stream.pos();

	for (size_t i = 0; i < _resources.size(); ++i) {
		const Resource &r = _resources[i];
		_stream.writeUint32LE(r.name.size());
		Common::writeString(_stream, r.name, Common::kEncodingASCII, false);
		_stream.writeUint32LE(r.size);
		_stream.writeUint32LE(r.offset);
	}

	_stream.writeUint32LE(resourceTableOffset);
	_stream.writeUint32LE(_resources.size());

	_finished = true;
}

} // End of namespace Aurora

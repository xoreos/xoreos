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
 *  Handling BioWare's ERFs (encapsulated resource file).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>

#include "src/common/memreadstream.h"
#include "src/common/readfile.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/md5.h"
#include "src/common/blowfish.h"

#include "src/aurora/erffile.h"
#include "src/aurora/util.h"

#include <zlib.h>

static const uint32 kERFID     = MKTAG('E', 'R', 'F', ' ');
static const uint32 kMODID     = MKTAG('M', 'O', 'D', ' ');
static const uint32 kHAKID     = MKTAG('H', 'A', 'K', ' ');
static const uint32 kSAVID     = MKTAG('S', 'A', 'V', ' ');
static const uint32 kVersion1  = MKTAG('V', '1', '.', '0');
static const uint32 kVersion11 = MKTAG('V', '1', '.', '1');
static const uint32 kVersion2  = MKTAG('V', '2', '.', '0');
static const uint32 kVersion22 = MKTAG('V', '2', '.', '2');
static const uint32 kVersion3  = MKTAG('V', '3', '.', '0');

namespace Aurora {

ERFFile::ERFFile(Common::SeekableReadStream *erf, const std::vector<byte> &password) :
	_erf(erf), _password(password) {

	try {
		load(*_erf);
	} catch (...) {
		delete _erf;
		throw;
	}
}

ERFFile::~ERFFile() {
	delete _erf;
}

void ERFFile::verifyVersion(uint32 id, uint32 version, bool utf16le) {
	if ((id != kERFID) && (id != kMODID) && (id != kHAKID) && (id != kSAVID))
		throw Common::Exception("Not an ERF file (%s)", Common::debugTag(id).c_str());

	if ((version != kVersion1) && (version != kVersion11) &&
	    (version != kVersion2) && (version != kVersion22) &&
	    (version != kVersion3))
		throw Common::Exception("Unsupported ERF file version %s", Common::debugTag(version).c_str());

	if ((version != kVersion1) && (version != kVersion11) && !utf16le)
		throw Common::Exception("ERF file version 2.0+, but not UTF-16LE");
}

void ERFFile::verifyPasswordDigest() {
	if (_header.encryption == kEncryptionNone)
		return;

	if (_password.empty())
		throw Common::Exception("Encrypted; password required");

		if (_header.encryption == kEncryptionXOR)
			throw Common::Exception("Unsupported XOR encryption");

	if (_header.encryption == kEncryptionBlowfishDAO) {
		// The digest is the simple MD5 sum of the password
		if (!Common::compareMD5Digest(_password, _header.passwordDigest))
			throw Common::Exception("Password digest does not match");

		// Parse the password into a number, and create an 8-byte little endian array out of it

		Common::UString passwordString((const char *) &_password[0], _password.size());

		uint64 passwordNumber = 0;
		Common::parseString(passwordString, passwordNumber);

		_password.resize(8);
		for (size_t i = 0; i < 8; i++) {
			_password[i] = passwordNumber & 0xFF;

			passwordNumber >>= 8;
		}

		return;
	}

	if (_header.encryption == kEncryptionBlowfishDA2) {
		// The digest is the MD5 sum of an [0-255] array encrypted by the password
		byte buffer[256];
		for (size_t i = 0; i < sizeof(buffer); i++)
			buffer[i] = i;

		Common::MemoryReadStream bufferStream(buffer, sizeof(buffer));
		Common::SeekableReadStream *bufferEncrypted = Common::encryptBlowfishEBC(bufferStream, _password);

		try {

			if (!Common::compareMD5Digest(*bufferEncrypted,  _header.passwordDigest))
				throw Common::Exception("Password digest does not match");

		} catch (...) {
			delete bufferEncrypted;
			throw;
		}

		delete bufferEncrypted;
		return;
	}

	throw Common::Exception("Invalid encryption type %u", (uint)_header.encryption);
}

void ERFFile::load(Common::SeekableReadStream &erf) {
	readHeader(erf);

	verifyVersion(_id, _version, _utf16le);

	try {

		readERFHeader(erf, _header, _version);

		try {

			verifyPasswordDigest();

			readDescription(_description, erf, _header, _version);
			readResources(erf, _header);

		} catch (Common::Exception &UNUSED(e)) {
			delete[] _header.stringTable;
			throw;
		}

		delete[] _header.stringTable;
		_header.stringTable = 0;

	} catch (Common::Exception &e) {
		e.add("Failed reading ERF file");
		throw;
	}

}

void ERFFile::readERFHeader(Common::SeekableReadStream &erf, ERFHeader &header, uint32 version) {
	header.buildYear       = 0;
	header.buildDay        = 0;
	header.stringTableSize = 0;
	header.stringTable     = 0;

	uint32 flags = 0;

	if        ((version == kVersion1) || (version == kVersion11)) {

		header.langCount = erf.readUint32LE(); // Number of languages for the description
		erf.skip(4);                           // Number of bytes in the description
		header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

		header.offDescription = erf.readUint32LE();
		header.offKeyList     = erf.readUint32LE();
		header.offResList     = erf.readUint32LE();

		header.buildYear = erf.readUint32LE() + 1900;
		header.buildDay  = erf.readUint32LE();

		header.descriptionID = erf.readUint32LE();

		erf.skip(116); // Reserved

		header.moduleID = 0; // No module ID in ERF V1.0 / V1.1

	} else if (version == kVersion2) {

		header.langCount = 0;                  // No description in ERF V2.0
		header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

		header.buildYear = erf.readUint32LE() + 1900;
		header.buildDay  = erf.readUint32LE();

		erf.skip(4);     // Unknown, always 0xFFFFFFFF?

		header.descriptionID   = 0;    // No description in ERF V2.0
		header.offDescription  = 0;    // No description in ERF V2.0
		header.offKeyList      = 0;    // No separate key list in ERF V2.0
		header.offResList      = 0x20; // Resource list always starts at 0x20 in ERF V2.0

		header.moduleID = 0; // No module ID in ERF V2.0

	} else if (version == kVersion22) {

		header.langCount = 0;                  // No description in ERF V2.2
		header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

		header.buildYear = erf.readUint32LE() + 1900;
		header.buildDay  = erf.readUint32LE();

		erf.skip(4);     // Unknown, always 0xFFFFFFFF?

		flags = erf.readUint32LE();

		header.moduleID = erf.readUint32LE();

		header.passwordDigest.resize(16);
		if (erf.read(&header.passwordDigest[0], 16) != 16)
			throw Common::Exception(Common::kReadError);

		header.descriptionID   = 0;    // No description in ERF V2.2
		header.offDescription  = 0;    // No description in ERF V2.2
		header.offKeyList      = 0;    // No separate key list in ERF V2.2
		header.offResList      = 0x38; // Resource list always starts at 0x38 in ERF V2.2

	} else if (version == kVersion3) {

		header.langCount = 0;                        // No description in ERF V3.0
		header.stringTableSize = erf.readUint32LE();
		header.resCount        = erf.readUint32LE(); // Number of resources in the ERF

		flags = erf.readUint32LE();

		header.moduleID = erf.readUint32LE();

		header.passwordDigest.resize(16);
		if (erf.read(&header.passwordDigest[0], 16) != 16)
			throw Common::Exception(Common::kReadError);

		header.stringTable = new char[header.stringTableSize];
		if (erf.read(header.stringTable, header.stringTableSize) != header.stringTableSize) {
			delete[] header.stringTable;
			throw Common::Exception("Failed to read ERF string table");
		}

		header.descriptionID  = 0;                             // No description in ERF V3.0
		header.offDescription = 0;                             // No description in ERF V3.0
		header.offKeyList     = 0;                             // No separate key list in ERF V3.0
		header.offResList     = 0x30 + header.stringTableSize; // Resource list always starts after the string table in ERF V3.2

	}

	header.encryption  = (Encryption)  ((flags >>  4) & 0x0000000F);
	header.compression = (Compression) ((flags >> 29) & 0x00000007);
}

void ERFFile::readDescription(LocString &description, Common::SeekableReadStream &erf,
                              const ERFHeader &header, uint32 version) {
	if (version != kVersion1)
		return;

	erf.seek(header.offDescription);
	description.readLocString(erf, header.descriptionID, header.langCount);
}

void ERFFile::readResources(Common::SeekableReadStream &erf, const ERFHeader &header) {
	_resources.resize(header.resCount);
	_iResources.resize(header.resCount);

	if        (_version == kVersion1) {

		readV1KeyList(erf, header); // Read name and type part of the resource list
		readV1ResList(erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion11) {

		readV11KeyList(erf, header); // Read name and type part of the resource list
		readV1ResList (erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion2) {

		// Read the resource list
		readV2ResList(erf, header);

	} else if (_version == kVersion22) {

		// Read the resource list
		readV22ResList(erf, header);

	} else if (_version == kVersion3) {

		// Read the resource list
		readV3ResList(erf, header);

	}

}

void ERFFile::readV1KeyList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offKeyList);

	uint32 index = 0;
	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++index, ++res) {
		res->name = Common::readStringFixed(erf, Common::kEncodingASCII, 16);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
		res->index = index;
	}
}

void ERFFile::readV11KeyList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offKeyList);

	uint32 index = 0;
	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++index, ++res) {
		res->name = Common::readStringFixed(erf, Common::kEncodingASCII, 32);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
		res->index = index;
	}
}

void ERFFile::readV1ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		res->offset                         = erf.readUint32LE();
		res->packedSize = res->unpackedSize = erf.readUint32LE();
	}
}

void ERFFile::readV2ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		Common::UString name = Common::readStringFixed(erf, Common::kEncodingUTF16LE, 64);

		res->name  = TypeMan.setFileType(name, kFileTypeNone);
		res->type  = TypeMan.getFileType(name);
		res->index = index;

		iRes->offset                          = erf.readUint32LE();
		iRes->packedSize = iRes->unpackedSize = erf.readUint32LE();
	}

}

void ERFFile::readV22ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		Common::UString name = Common::readStringFixed(erf, Common::kEncodingUTF16LE, 64);

		res->name  = TypeMan.setFileType(name, kFileTypeNone);
		res->type  = TypeMan.getFileType(name);
		res->index = index;

		iRes->offset       = erf.readUint32LE();
		iRes->packedSize   = erf.readUint32LE();
		iRes->unpackedSize = erf.readUint32LE();
	}

}

void ERFFile::readV3ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32 index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		int32 nameOffset = erf.readSint32LE();

		if (nameOffset >= 0) {
			if ((uint32)nameOffset >= header.stringTableSize)
				throw Common::Exception("Invalid ERF string table offset");

			Common::UString name = header.stringTable + nameOffset;
			res->name = TypeMan.setFileType(name, kFileTypeNone);
			res->type = TypeMan.getFileType(name);
		}

		res->index = index;
		res->hash  = erf.readUint64LE();

		uint32 typeHash = erf.readUint32LE();

		// Look up the file type by its hash
		FileType type = TypeMan.getFileType(Common::kHashFNV32, typeHash);
		if (type != kFileTypeNone)
			res->type = type;

		iRes->offset       = erf.readUint32LE();
		iRes->packedSize   = erf.readUint32LE();
		iRes->unpackedSize = erf.readUint32LE();
	}

}

uint32 ERFFile::getBuildYear() const {
	return _header.buildYear;
}

uint32 ERFFile::getBuildDay() const {
	return _header.buildDay;
}

const LocString &ERFFile::getDescription() const {
	return _description;
}

const Archive::ResourceList &ERFFile::getResources() const {
	return _resources;
}

const ERFFile::IResource &ERFFile::getIResource(uint32 index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32 ERFFile::getResourceSize(uint32 index) const {
	return getIResource(index).unpackedSize;
}

Common::SeekableReadStream *ERFFile::getResource(uint32 index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	if (tryNoCopy && (_header.encryption == kEncryptionNone) && (_header.compression == kCompressionNone))
		return new Common::SeekableSubReadStream(_erf, res.offset, res.offset + res.packedSize);

	_erf->seek(res.offset);

	// Read, decrypt and decompress
	return decompress(decrypt(_erf->readStream(res.packedSize)), res.unpackedSize);
}

Common::MemoryReadStream *ERFFile::decrypt(Common::MemoryReadStream *cryptStream) const {
	try {
		Common::MemoryReadStream *decryptStream = 0;

		switch (_header.encryption) {
			case kEncryptionNone:
				return cryptStream;

			case kEncryptionBlowfishDAO:
			case kEncryptionBlowfishDA2:
				decryptStream = Common::decryptBlowfishEBC(*cryptStream, _password);
				delete cryptStream;
				return decryptStream;

			default:
				throw Common::Exception("Invalid ERF encryption %u", (uint) _header.encryption);
		}
	} catch (...) {
		delete cryptStream;
		throw;
	}
}

Common::SeekableReadStream *ERFFile::decompress(Common::MemoryReadStream *packedStream,
                                                uint32 unpackedSize) const {
	switch (_header.compression) {
		case kCompressionNone:
			if (packedStream->size() == unpackedSize)
				return packedStream;

			return new Common::SeekableSubReadStream(packedStream, 0, unpackedSize, true);

		case kCompressionBioWareZlib:
			return decompressBiowareZlib(packedStream, unpackedSize);

		case kCompressionHeaderlessZlib:
			return decompressHeaderlessZlib(packedStream, unpackedSize);

		default:
			delete packedStream;
			throw Common::Exception("Invalid ERF compression %u", (uint) _header.compression);
	}
}

Common::SeekableReadStream *ERFFile::decompressBiowareZlib(Common::MemoryReadStream *packedStream,
                                                           uint32 unpackedSize) const {

	// This ugly const cast is necessary because the zlib API wants a non-const
	// next_in pointer by default. Unless we define ZLIB_CONST, but that only
	// appeared in zlib 1.2.5.3. Not really worth bumping our required zlib
	// version for, IMHO.
	byte *compressedData = const_cast<byte *>(packedStream->getData());
	uint32 packedSize = packedStream->size();

	Common::SeekableReadStream *stream = 0;
	try {
		stream = decompressZlib(compressedData + 1, packedSize - 1, unpackedSize, *compressedData >> 4);
	} catch (...) {
		delete packedStream;
		throw;
	}

	delete packedStream;
	return stream;
}

Common::SeekableReadStream *ERFFile::decompressHeaderlessZlib(Common::MemoryReadStream *packedStream,
                                                              uint32 unpackedSize) const {

	// This ugly const cast is necessary because the zlib API wants a non-const
	// next_in pointer by default. Unless we define ZLIB_CONST, but that only
	// appeared in zlib 1.2.5.3. Not really worth bumping our required zlib
	// version for, IMHO.
	byte *compressedData = const_cast<byte *>(packedStream->getData());
	uint32 packedSize = packedStream->size();

	Common::SeekableReadStream *stream = 0;
	try {
		stream = decompressZlib(compressedData, packedSize, unpackedSize, MAX_WBITS);
	} catch (...) {
		delete packedStream;
		throw;
	}

	delete packedStream;
	return stream;
}

Common::SeekableReadStream *ERFFile::decompressZlib(byte *compressedData, uint32 packedSize,
                                                    uint32 unpackedSize, int windowBits) const {
	// Allocate the decompressed data
	byte *decompressedData = new byte[unpackedSize];

	z_stream strm;
	strm.zalloc   = Z_NULL;
	strm.zfree    = Z_NULL;
	strm.opaque   = Z_NULL;
	strm.avail_in = packedSize;
	strm.next_in  = compressedData;

	// Negative windows bits means there is no zlib header present in the data.
	int zResult = inflateInit2(&strm, -windowBits);
	if (zResult != Z_OK) {
		inflateEnd(&strm);

		delete[] decompressedData;
		throw Common::Exception("Could not initialize zlib inflate");
	}

	strm.avail_out = unpackedSize;
	strm.next_out  = decompressedData;

	zResult = inflate(&strm, Z_SYNC_FLUSH);
	if (zResult != Z_OK && zResult != Z_STREAM_END) {
		inflateEnd(&strm);

		delete[] decompressedData;
		throw Common::Exception("Failed to inflate: %d", zResult);
	}

	inflateEnd(&strm);
	return new Common::MemoryReadStream(decompressedData, unpackedSize, true);
}

Common::HashAlgo ERFFile::getNameHashAlgo() const {
	// Only V3 uses hashing
	return (_version == kVersion3) ? Common::kHashFNV64 : Common::kHashNone;
}

LocString ERFFile::getDescription(Common::SeekableReadStream &erf) {
	uint32 id, version;
	bool ut16le;

	readHeader(erf, id, version, ut16le);
	verifyVersion(id, version, ut16le);

	ERFHeader header;
	LocString description;

	try {
		readERFHeader(erf, header, version);
		readDescription(description, erf, header, version);
	} catch (...) {
		delete[] header.stringTable;
		throw;
	}

	delete[] header.stringTable;

	return description;
}

LocString ERFFile::getDescription(const Common::UString &fileName) {
	Common::ReadFile erf(fileName);

	return getDescription(erf);
}

} // End of namespace Aurora

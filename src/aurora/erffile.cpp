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
#include "src/common/deflate.h"

#include "src/aurora/erffile.h"
#include "src/aurora/util.h"

static const uint32_t kERFID     = MKTAG('E', 'R', 'F', ' ');
static const uint32_t kMODID     = MKTAG('M', 'O', 'D', ' ');
static const uint32_t kHAKID     = MKTAG('H', 'A', 'K', ' ');
static const uint32_t kSAVID     = MKTAG('S', 'A', 'V', ' ');
static const uint32_t kVersion10 = MKTAG('V', '1', '.', '0');
static const uint32_t kVersion11 = MKTAG('V', '1', '.', '1');
static const uint32_t kVersion20 = MKTAG('V', '2', '.', '0');
static const uint32_t kVersion21 = MKTAG('V', '2', '.', '1');
static const uint32_t kVersion22 = MKTAG('V', '2', '.', '2');
static const uint32_t kVersion30 = MKTAG('V', '3', '.', '0');

namespace Aurora {

static const size_t kNWNPremiumKeyLength = 56;

static const byte kNWNPremiumKeys[][kNWNPremiumKeyLength] = {
	{
		0x8A, 0x83, 0x5A, 0x2D, 0x01, 0x10, 0x5C, 0xBE, 0xCE, 0x2C, 0xD0, 0x69, 0xB8, 0x48, 0xC9, 0xBE,
		0xAA, 0x7E, 0x57, 0xBD, 0xAB, 0x94, 0xCE, 0x0D, 0x09, 0x10, 0xBD, 0x57, 0x8D, 0x1A, 0x0D, 0x35,
		0xC9, 0x84, 0x90, 0xFA, 0x7B, 0x65, 0x32, 0x97, 0xCF, 0x39, 0x66, 0xA7, 0x60, 0xEC, 0x99, 0xEF,
		0x76, 0x5A, 0x3B, 0x2C, 0x2F, 0x0A, 0x24, 0xC3
	}, {
		0x5C, 0xA8, 0x83, 0xCE, 0x6D, 0x9C, 0x84, 0x3B, 0xDE, 0xA1, 0xD7, 0x85, 0x78, 0x87, 0xFD, 0x0D,
		0xD7, 0x97, 0x42, 0x38, 0x93, 0x53, 0xCD, 0x3D, 0xEC, 0x03, 0xCE, 0x15, 0x09, 0x48, 0xD7, 0x9B,
		0x86, 0x20, 0x19, 0x21, 0xC2, 0x6E, 0x42, 0xEE, 0xAF, 0xBE, 0xCA, 0x41, 0xBF, 0x0A, 0x66, 0x9A,
		0x2A, 0x8F, 0xC2, 0x04, 0x56, 0x14, 0x77, 0x82
	}, {
		0x94, 0x37, 0xDD, 0x56, 0xED, 0xD9, 0xF0, 0x41, 0x91, 0x28, 0xC2, 0xD5, 0x4F, 0x97, 0xE8, 0x5D,
		0x95, 0x42, 0x41, 0xB5, 0xA3, 0xA2, 0x03, 0xB6, 0x08, 0x8B, 0xC1, 0xC7, 0xCB, 0xEE, 0xD8, 0xD9,
		0x83, 0x4E, 0x9F, 0xAB, 0x69, 0x41, 0x6C, 0xFC, 0x87, 0x13, 0xB5, 0xFD, 0xCA, 0x0A, 0x1D, 0x83,
		0x89, 0x51, 0x4E, 0xF3, 0x5B, 0xED, 0x9A, 0x5C
	}, {
		0xEC, 0xBF, 0x9A, 0x75, 0x39, 0xE4, 0x65, 0x10, 0xCA, 0xA1, 0xB6, 0x3A, 0x76, 0xA2, 0x50, 0xDD,
		0xE2, 0x06, 0xEA, 0x39, 0x20, 0x0F, 0xBC, 0xC7, 0x5D, 0x03, 0xD6, 0xB2, 0x75, 0x83, 0xEF, 0x08,
		0xA7, 0xFD, 0x68, 0xDB, 0x33, 0xAA, 0x69, 0xA1, 0x39, 0x80, 0x62, 0x29, 0x8C, 0xE9, 0x2B, 0xA9,
		0x60, 0xC6, 0x93, 0x27, 0x2C, 0x0D, 0x42, 0x26
	}, {
		0x72, 0xA3, 0x8B, 0x0C, 0x49, 0x63, 0x53, 0x32, 0x7D, 0x86, 0x9C, 0x3D, 0x48, 0x84, 0x77, 0xBD,
		0xBB, 0x7C, 0x48, 0xDC, 0x33, 0x45, 0x40, 0x78, 0xAF, 0x26, 0x64, 0x96, 0x7F, 0x80, 0x44, 0x67,
		0xF7, 0xDA, 0x08, 0x74, 0xF4, 0xD8, 0x7C, 0x2D, 0x2D, 0x1F, 0xF2, 0xD1, 0x5D, 0x18, 0xF7, 0xD2,
		0x71, 0x28, 0xD0, 0x31, 0x5C, 0xCC, 0x6E, 0xCD
	}, {
		0x1D, 0xEA, 0x5E, 0x04, 0x0B, 0x43, 0xF2, 0x2F, 0x23, 0x90, 0x04, 0x38, 0x5C, 0xA1, 0x18, 0x9C,
		0xFF, 0xB1, 0xB1, 0x86, 0xE6, 0x69, 0x67, 0x8E, 0x29, 0x08, 0x94, 0x4C, 0x08, 0x22, 0x0A, 0x8C,
		0x9D, 0xC5, 0xFE, 0xE2, 0x27, 0x84, 0xE7, 0xD3, 0xD1, 0x6B, 0xF2, 0x38, 0xF4, 0xFF, 0xBF, 0x80,
		0x7E, 0x1E, 0x98, 0x75, 0x32, 0xA3, 0x2C, 0xF6
	}
};


ERFFile::ERFHeader::ERFHeader() {
	clear();
}

ERFFile::ERFHeader::~ERFHeader() {
}

void ERFFile::ERFHeader::clear() {
	resCount  = 0;
	langCount = 0;

	descriptionID = 0;

	offDescription = 0xFFFFFFFF;
	offKeyList     = 0xFFFFFFFF;
	offResList     = 0xFFFFFFFF;

	descriptionSize = 0;

	buildYear = 0;
	buildDay  = 0;

	isNWNPremium = false;

	moduleID = 0;

	encryption  = kEncryptionNone;
	compression = kCompressionNone;

	clearStringTable();
}

void ERFFile::ERFHeader::clearStringTable() {
	stringTable.reset();

	stringTableSize = 0;
}

bool ERFFile::ERFHeader::isSensible(size_t fileSize) {
	if ((buildYear > 2200) || (buildYear < 2000) || (buildDay > 366))
		return false;

	if ((resCount >= 131072) || (langCount > 32))
		return false;

	if ((offDescription != 0xFFFFFFFF) && (offDescription > fileSize))
		return false;
	if ((offKeyList     != 0xFFFFFFFF) && (offKeyList     > fileSize))
		return false;
	if ((offResList     != 0xFFFFFFFF) && (offResList     > fileSize))
		return false;

	return true;
}


ERFFile::ERFFile(Common::SeekableReadStream *erf, const std::vector<byte> &password) :
	_erf(erf), _password(password) {

	assert(_erf);

	load();
}

ERFFile::~ERFFile() {
}

void ERFFile::verifyVersion(uint32_t id, uint32_t version, bool utf16le) {
	if ((id != kERFID) && (id != kMODID) && (id != kHAKID) && (id != kSAVID))
		throw Common::Exception("Not an ERF file (%s)", Common::debugTag(id).c_str());

	if ((version != kVersion10) && (version != kVersion11) &&
	    (version != kVersion20) && (version != kVersion21) && (version != kVersion22) &&
	    (version != kVersion30))
		throw Common::Exception("Unsupported ERF file version %s", Common::debugTag(version).c_str());

	if ((version != kVersion10) && (version != kVersion11) && (version != kVersion21) && !utf16le)
		throw Common::Exception("ERF file version 2.0 or 2.2+, but not UTF-16LE");
}

void ERFFile::verifyPasswordDigest() {
	if ((_header.encryption == kEncryptionNone) || (_header.encryption == kEncryptionBlowfishNWN))
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

		Common::UString passwordString(reinterpret_cast<const char *>(&_password[0]), _password.size());

		uint64_t passwordNumber = 0;
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

		Common::MemoryReadStream bufferStream(buffer);
		std::unique_ptr<Common::SeekableReadStream>
			bufferEncrypted(Common::encryptBlowfishEBC(bufferStream, _password));

		if (!Common::compareMD5Digest(*bufferEncrypted, _header.passwordDigest))
			throw Common::Exception("Password digest does not match");

		return;
	}

	throw Common::Exception("Invalid encryption type %u", (uint)_header.encryption);
}

bool ERFFile::decryptNWNPremiumHeader(Common::SeekableReadStream &erf, ERFHeader &header,
                                      const std::vector<byte> &password) {

	std::unique_ptr<Common::SeekableReadStream>
		decryptERF(decrypt(erf, erf.pos(), 152, kEncryptionBlowfishNWN, password));

	readV11Header(*decryptERF, header);

	return header.isSensible(erf.size());
}

bool ERFFile::findNWNPremiumKey(Common::SeekableReadStream &erf, ERFHeader &header,
                                const std::vector<byte> &md5, std::vector<byte> &password) {

	assert(md5.empty() || (md5.size() == Common::kMD5Length));

	password.resize(kNWNPremiumKeyLength);
	const size_t headerPos = erf.pos();

	for (size_t i = 0; i < ARRAYSIZE(kNWNPremiumKeys); i++) {
		std::memcpy(&password[0], kNWNPremiumKeys[i], kNWNPremiumKeyLength);
		if (!md5.empty())
			std::memcpy(&password[0] + kNWNPremiumKeyLength - Common::kMD5Length, &md5[0], Common::kMD5Length);

		erf.seek(headerPos);
		if (decryptNWNPremiumHeader(erf, header, password))
			return true;
	}

	return false;
}

void ERFFile::readNWNPremiumHeader(Common::SeekableReadStream &erf, ERFHeader &header,
                                   std::vector<byte> &password) {

	assert(header.encryption == kEncryptionBlowfishNWN);

	if (!password.empty() && (password.size() != Common::kMD5Length))
		throw Common::Exception("Invalid Neverwinter Nights premium module MD5 length (%u)",
		                        (uint)password.size());

	const std::vector<byte> md5 = password;

	if (!findNWNPremiumKey(erf, header, md5, password))
		throw Common::Exception("Wrong Neverwinter Nights premium module key");

	header.isNWNPremium = true;
	header.encryption   = kEncryptionBlowfishNWN;
}

void ERFFile::load() {
	readHeader(*_erf);

	verifyVersion(_id, _version, _utf16le);

	try {

		readERFHeader(*_erf, _header, _version, _password);

		verifyPasswordDigest();

		readDescription(_description, *_erf, _header);

		if (_header.encryption == kEncryptionBlowfishNWN)
			decryptNWNPremium();

		readResources(*_erf, _header);

		_header.clearStringTable();

	} catch (Common::Exception &e) {
		e.add("Failed reading ERF file");
		throw;
	}

}

void ERFFile::decryptNWNPremium() {
	assert(_header.encryption == kEncryptionBlowfishNWN);

	_erf->seek(0);

	_erf.reset(decrypt(*_erf, kEncryptionBlowfishNWN, _password));

	_header.encryption = kEncryptionNone;
}

void ERFFile::readV10Header(Common::SeekableReadStream &erf, ERFHeader &header) {
	header.langCount        = erf.readUint32LE(); // Number of languages for the description
	header.descriptionSize  = erf.readUint32LE(); // Number of bytes in the description
	header.resCount         = erf.readUint32LE(); // Number of resources in the ERF

	header.offDescription = erf.readUint32LE();
	header.offKeyList     = erf.readUint32LE();
	header.offResList     = erf.readUint32LE();

	header.buildYear = erf.readUint32LE() + 1900;
	header.buildDay  = erf.readUint32LE();

	header.descriptionID = erf.readUint32LE();

	erf.skip(116); // Reserved
}

void ERFFile::readV11Header(Common::SeekableReadStream &erf, ERFHeader &header) {
	header.langCount        = erf.readUint32LE(); // Number of languages for the description
	header.descriptionSize  = erf.readUint32LE(); // Number of bytes in the description
	header.resCount         = erf.readUint32LE(); // Number of resources in the ERF

	header.offDescription = erf.readUint32LE();
	header.offKeyList     = erf.readUint32LE();
	header.offResList     = erf.readUint32LE();

	header.buildYear = erf.readUint32LE() + 1900;
	header.buildDay  = erf.readUint32LE();

	header.descriptionID = erf.readUint32LE();

	erf.skip(116); // Reserved

	const uint32_t keyListSize = header.offResList - header.offKeyList;

	/* Check whether the "true" V1.1 extended filenames would fit into the key
	 * list. If so, this is probably a true V1.1 ERF, as found in Neverwinter
	 * Nights 2. If not, this is a Neverwinter Nights premium module with shorter
	 * filenames. */
	header.isNWNPremium = (header.resCount > 0) && ((keyListSize / header.resCount) < 40);

	if (!header.isSensible()) {
		header.isNWNPremium = true;
		header.encryption   = kEncryptionBlowfishNWN;
	}
}

void ERFFile::readV20Header(Common::SeekableReadStream &erf, ERFHeader &header) {
	header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

	header.buildYear = erf.readUint32LE() + 1900;
	header.buildDay  = erf.readUint32LE();

	erf.skip(4); // Unknown, always 0xFFFFFFFF?

	header.offResList = 0x00000020; // Resource list always starts at 0x20 in ERF V2.0
}

void ERFFile::readV21Header(Common::SeekableReadStream &erf, ERFHeader &header) {
	erf.skip(8); // Unknown, always 0x00000000 0x00000000?

	header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

	erf.skip(4); // Unknown, always 0x00000000?

	header.buildYear = erf.readUint16LE() + 1900;
	header.buildDay  = erf.readUint16LE();

	erf.skip(4); // Unknown, always 0xFFFF0000?

	header.offResList = 0x00000020; // Resource list always starts at 0x20 in ERF V2.1

	header.compression = kCompressionStandardZlib;
}

void ERFFile::readV22Header(Common::SeekableReadStream &erf, ERFHeader &header, uint32_t &flags) {
	header.resCount  = erf.readUint32LE(); // Number of resources in the ERF

	header.buildYear = erf.readUint32LE() + 1900;
	header.buildDay  = erf.readUint32LE();

	erf.skip(4); // Unknown, always 0xFFFFFFFF?

	flags = erf.readUint32LE();

	header.moduleID = erf.readUint32LE();

	header.passwordDigest.resize(Common::kMD5Length);
	if (erf.read(&header.passwordDigest[0], Common::kMD5Length) != Common::kMD5Length)
		throw Common::Exception(Common::kReadError);

	header.offResList = 0x00000038; // Resource list always starts at 0x38 in ERF V2.2

	header.encryption  = (Encryption)  ((flags >>  4) & 0x0000000F);
	header.compression = (Compression) ((flags >> 29) & 0x00000007);
}

void ERFFile::readV30Header(Common::SeekableReadStream &erf, ERFHeader &header, uint32_t &flags) {
	header.stringTableSize = erf.readUint32LE(); // Number of bytes in the string table
	header.resCount        = erf.readUint32LE(); // Number of resources in the ERF

	flags = erf.readUint32LE();

	header.moduleID = erf.readUint32LE();

	header.passwordDigest.resize(Common::kMD5Length);
	if (erf.read(&header.passwordDigest[0], Common::kMD5Length) != Common::kMD5Length)
		throw Common::Exception(Common::kReadError);

	header.stringTable = std::make_unique<char[]>(header.stringTableSize);
	if (erf.read(header.stringTable.get(), header.stringTableSize) != header.stringTableSize) {
		header.clearStringTable();
		throw Common::Exception("Failed to read ERF string table");
	}

	// Resource list always starts after the string table in ERF V3.0
	header.offResList     = 0x00000030 + header.stringTableSize;

	header.encryption  = (Encryption)  ((flags >>  4) & 0x0000000F);
	header.compression = (Compression) ((flags >> 29) & 0x00000007);
}

void ERFFile::readERFHeader(Common::SeekableReadStream &erf, ERFHeader &header, uint32_t version,
                            std::vector<byte> &password) {

	header.clear();

	if        (version == kVersion10) {

		/* Version 1.0:
		 * Neverwinter Nights, Knights of the Old Republic I and II,
		 * Jade Empire, The Witcher. */

		readV10Header(erf, header);

	} else if (version == kVersion11) {

		/* Version 1.1:
		 * There's two slightly different formats of the 1.1 ERF version:
		 * - The one is present in Neverwinter Nights 2. It extends the filename
		 *   field in the key list to 32 characters (from 16 in V1.0)
		 * - Premium modules in Neverwinter Nights. They are identical to V1.0
		 *   ERFs, except that they may be encrypted.
		 */

		const size_t headerPos = erf.pos();

		readV11Header(erf, header);

		if (header.encryption == kEncryptionBlowfishNWN) {
			erf.seek(headerPos);

			readNWNPremiumHeader(erf, header, password);
		}

	} else if (version == kVersion20) {

		/* Version 2.0:
		 * Unencrypted data in Dragon Age: Origins (PC). */

		readV20Header(erf, header);

	} else if (version == kVersion21) {

		/* Version 2.1:
		 * Unencrypted data in Dragon Age: Origins (Xbox). */

		readV21Header(erf, header);

	} else if (version == kVersion22) {

		/* Version 2.2:
		 * Encrypted data in Dragon Age: Origins. */

		uint32_t flags = 0;
		readV22Header(erf, header, flags);

	} else if (version == kVersion30) {

		/* Version 3.0:
		 * Dragon Age II. */

		uint32_t flags = 0;
		readV30Header(erf, header, flags);
	}
}

void ERFFile::readDescription(LocString &description, Common::SeekableReadStream &erf,
                              const ERFHeader &header) {

	description.clear();
	if ((header.offDescription == 0) || (header.offDescription == 0xFFFFFFFF) || (header.langCount == 0))
		return;

	if (header.encryption == kEncryptionBlowfishNWN)
		return;

	erf.seek(header.offDescription);
	description.readLocString(erf, header.descriptionID, header.langCount);
}

void ERFFile::readResources(Common::SeekableReadStream &erf, const ERFHeader &header) {
	_resources.resize(header.resCount);
	_iResources.resize(header.resCount);

	if        (_version == kVersion10) {

		readV10KeyList(erf, header); // Read name and type part of the resource list
		readV10ResList(erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion11) {

		// Read name and type part of the resource list
		if (header.isNWNPremium)
			readV10KeyList(erf, header);
		else
			readV11KeyList(erf, header);

		readV10ResList (erf, header); // Read offset and size part of the resource list

	} else if (_version == kVersion20) {

		// Read the resource list
		readV20ResList(erf, header);

	} else if (_version == kVersion21) {

		// Read the resource list
		readV21ResList(erf, header);

	} else if (_version == kVersion22) {

		// Read the resource list
		readV22ResList(erf, header);

	} else if (_version == kVersion30) {

		// Read the resource list
		readV30ResList(erf, header);

	}

}

void ERFFile::readV10KeyList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offKeyList);

	uint32_t index = 0;
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

	uint32_t index = 0;
	for (ResourceList::iterator res = _resources.begin(); res != _resources.end(); ++index, ++res) {
		res->name = Common::readStringFixed(erf, Common::kEncodingASCII, 32);
		erf.skip(4); // Resource ID
		res->type = (FileType) erf.readUint16LE();
		erf.skip(2); // Reserved
		res->index = index;
	}
}

void ERFFile::readV10ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	for (IResourceList::iterator res = _iResources.begin(); res != _iResources.end(); ++res) {
		res->offset                         = erf.readUint32LE();
		res->packedSize = res->unpackedSize = erf.readUint32LE();
	}
}

void ERFFile::readV20ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32_t index = 0;
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

void ERFFile::readV21ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32_t index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		Common::UString name = Common::readStringFixed(erf, Common::kEncodingASCII, 32);

		res->name  = TypeMan.setFileType(name, kFileTypeNone);
		res->type  = TypeMan.getFileType(name);
		res->index = index;

		iRes->offset       = erf.readUint32LE();
		iRes->packedSize   = erf.readUint32LE();
		iRes->unpackedSize = erf.readUint32LE();
	}

}

void ERFFile::readV22ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32_t index = 0;
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

void ERFFile::readV30ResList(Common::SeekableReadStream &erf, const ERFHeader &header) {
	erf.seek(header.offResList);

	uint32_t index = 0;
	ResourceList::iterator   res = _resources.begin();
	IResourceList::iterator iRes = _iResources.begin();
	for (; (res != _resources.end()) && (iRes != _iResources.end()); ++index, ++res, ++iRes) {
		int32_t nameOffset = erf.readSint32LE();

		if (nameOffset >= 0) {
			if ((uint32_t)nameOffset >= header.stringTableSize)
				throw Common::Exception("Invalid ERF string table offset");

			Common::UString name = header.stringTable.get() + nameOffset;
			res->name = TypeMan.setFileType(name, kFileTypeNone);
			res->type = TypeMan.getFileType(name);
		}

		res->index = index;
		res->hash  = erf.readUint64LE();

		uint32_t typeHash = erf.readUint32LE();

		// Look up the file type by its hash
		FileType type = TypeMan.getFileType(Common::kHashFNV32, typeHash);
		if (type != kFileTypeNone)
			res->type = type;

		iRes->offset       = erf.readUint32LE();
		iRes->packedSize   = erf.readUint32LE();
		iRes->unpackedSize = erf.readUint32LE();
	}

}

uint32_t ERFFile::getBuildYear() const {
	return _header.buildYear;
}

uint32_t ERFFile::getBuildDay() const {
	return _header.buildDay;
}

const LocString &ERFFile::getDescription() const {
	return _description;
}

const Archive::ResourceList &ERFFile::getResources() const {
	return _resources;
}

const ERFFile::IResource &ERFFile::getIResource(uint32_t index) const {
	if (index >= _iResources.size())
		throw Common::Exception("Resource index out of range (%u/%u)", index, (uint)_iResources.size());

	return _iResources[index];
}

uint32_t ERFFile::getResourceSize(uint32_t index) const {
	return getIResource(index).unpackedSize;
}

Common::SeekableReadStream *ERFFile::getResource(uint32_t index, bool tryNoCopy) const {
	const IResource &res = getIResource(index);

	if (tryNoCopy && (_header.encryption == kEncryptionNone) && (_header.compression == kCompressionNone))
		return new Common::SeekableSubReadStream(_erf.get(), res.offset, res.offset + res.packedSize);

	_erf->seek(res.offset);

	// Read
	Common::MemoryReadStream *stream = _erf->readStream(res.packedSize);

	// Decrypt
	if (_header.encryption != kEncryptionNone)
		stream = decrypt(stream, _header.encryption, _password);

	// Decompress
	return decompress(stream, res.unpackedSize);
}

Common::MemoryReadStream *ERFFile::decrypt(Common::SeekableReadStream &cryptStream,
                                           Encryption encryption, const std::vector<byte> &password) {
	switch (encryption) {
		case kEncryptionBlowfishDAO:
		case kEncryptionBlowfishDA2:
		case kEncryptionBlowfishNWN:
			return Common::decryptBlowfishEBC(cryptStream, password);

		default:
			throw Common::Exception("Invalid ERF encryption %u", (uint) encryption);
	}
}

Common::MemoryReadStream *ERFFile::decrypt(Common::SeekableReadStream *cryptStream,
                                           Encryption encryption, const std::vector<byte> &password) {

	assert(cryptStream);

	std::unique_ptr<Common::SeekableReadStream> stream(cryptStream);

	return decrypt(*stream, encryption, password);
}

Common::SeekableReadStream *ERFFile::decrypt(Common::SeekableReadStream &erf, size_t pos, size_t size,
                                             Encryption encryption, const std::vector<byte> &password) {

	return decrypt(new Common::SeekableSubReadStream(&erf, pos, pos + size), encryption, password);
}

Common::SeekableReadStream *ERFFile::decrypt(Common::SeekableReadStream &erf, size_t size,
                                             Encryption encryption, const std::vector<byte> &password) {

	return decrypt(erf, erf.pos(), size, encryption, password);
}

Common::SeekableReadStream *ERFFile::decompress(Common::MemoryReadStream *packedStream,
                                                uint32_t unpackedSize) const {

	std::unique_ptr<Common::MemoryReadStream> stream(packedStream);

	switch (_header.compression) {
		case kCompressionNone:
			if (stream->size() == unpackedSize)
				return stream.release();

			return new Common::SeekableSubReadStream(stream.release(), 0, unpackedSize, true);

		case kCompressionBioWareZlib:
			return decompressBiowareZlib(stream.release(), unpackedSize);

		case kCompressionHeaderlessZlib:
			return decompressHeaderlessZlib(stream.release(), unpackedSize);

		case kCompressionStandardZlib:
			return decompressStandardZlib(stream.release(), unpackedSize);

		default:
			break;
	}

	throw Common::Exception("Invalid ERF compression %u", (uint) _header.compression);
}

Common::SeekableReadStream *ERFFile::decompressBiowareZlib(Common::MemoryReadStream *packedStream,
                                                           uint32_t unpackedSize) const {

	/* Decompress using raw inflate. An extra one byte header specifies the window size. */

	assert(packedStream);

	std::unique_ptr<Common::MemoryReadStream> stream(packedStream);

	const byte * const compressedData = stream->getData();
	const uint32_t packedSize = stream->size();

	return decompressZlib(compressedData + 1, packedSize - 1, unpackedSize, *compressedData >> 4);
}

Common::SeekableReadStream *ERFFile::decompressHeaderlessZlib(Common::MemoryReadStream *packedStream,
                                                              uint32_t unpackedSize) const {

	/* Decompress using raw inflate. Use the default maximum window size (15). */

	assert(packedStream);

	std::unique_ptr<Common::MemoryReadStream> stream(packedStream);

	const byte * const compressedData = stream->getData();
	const uint32_t packedSize = stream->size();

	return decompressZlib(compressedData, packedSize, unpackedSize, Common::kWindowBitsMax);
}

Common::SeekableReadStream *ERFFile::decompressStandardZlib(Common::MemoryReadStream *packedStream,
                                                            uint32_t unpackedSize) const {

	/* Decompress using raw inflate. Use the default maximum window size (15), and with zlib header. */

	assert(packedStream);

	std::unique_ptr<Common::MemoryReadStream> stream(packedStream);

	const byte * const compressedData = stream->getData();
	const uint32_t packedSize = stream->size();

	return decompressZlib(compressedData, packedSize, unpackedSize, -Common::kWindowBitsMax);
}

Common::SeekableReadStream *ERFFile::decompressZlib(const byte *compressedData, uint32_t packedSize,
                                                    uint32_t unpackedSize, int windowBits) const {

	// Decompress. Negative window size to signal not to look for a gzip header.
	const byte *data = Common::decompressDeflate(compressedData, packedSize, unpackedSize, -windowBits);

	return new Common::MemoryReadStream(data, unpackedSize, true);
}

Common::HashAlgo ERFFile::getNameHashAlgo() const {
	// Only V3 uses hashing
	return (_version == kVersion30) ? Common::kHashFNV64 : Common::kHashNone;
}

LocString ERFFile::getDescription(Common::SeekableReadStream &erf) {
	uint32_t id, version;
	bool ut16le;

	readHeader(erf, id, version, ut16le);
	verifyVersion(id, version, ut16le);

	ERFHeader header;
	LocString description;

	try {
		std::vector<byte> password;
		readERFHeader(erf, header, version, password);
	} catch (...) {
		return description;
	}

	readDescription(description, erf, header);

	return description;
}

LocString ERFFile::getDescription(const Common::UString &fileName) {
	Common::ReadFile erf(fileName);

	return getDescription(erf);
}

} // End of namespace Aurora

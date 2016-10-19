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

#ifndef AURORA_ERFFILE_H
#define AURORA_ERFFILE_H

#include <vector>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of an ERF archive file.
 *
 *  The ERF file is, together with KEY and BIF files, the most basic
 *  and widespread archive archive format used in Aurora engine games.
 *  There are several versions with different features.
 *
 *  Supported versions:
 *  - 1.0:
 *    - Used in Neverwinter Nights, Knights of the Old Republic I and II,
 *      Jade Empire and The Witcher
 *    - 16 ASCII characters per resource name
 *    - Extension saved as a Type ID
 *    - Includes a localized description text field
 *  - 1.1 (variant a):
 *    - Used in BioWare premium modules for Neverwinter Nights
 *    - Blowfish encrypted
 *    - Otherwise, identical to the 1.0 format
 *  - 1.1 (variant b):
 *    - Used in Neverwinter Nights 2
 *    - 32 ASCII characters per resource name
 *    - Extension saved as a Type ID
 *    - Includes a localized description text field
 *  - 2.0:
 *    - Used in Dragon Age: Origins
 *    - 32 UTF-16 characters per resource name
 *    - Resource name includes extension and path
 *  - 2.2:
 *    - Used in Dragon Age: Origins
 *    - Optionally Blowfish encrypted
 *    - Optionally DEFLATE compressed
 *    - Otherwise, identical to the 2.0 format
 *  - 3.0:
 *    - Dragon Age II
 *    - Optionally Blowfish encrypted
 *    - Optionally DEFLATE compressed
 *    - Variable amount of ASCII characters per resource name
 *    - Resource name includes extension and path
 *    - Optionally, resource names are stripped in favour of a FNV64
 *      hash of the resource name (including path and extension) and
 *      a FNV32 hash of only the extension
 *
 *  Known unsupported features:
 *  - XOR encryption as used in versions 2.2 and 3.0
 *
 *  ERF files can have several different file extensions:
 *  - .erf: Generic resource archive
 *  - .nwm: BioWare original campaign modules
 *  - .mod: Community adventure modules
 *  - .hak: Additional, shared module resources
 *  - .sav: Save games
 *  - .crf: Creature resources
 *  - .rim: Room resources in the Dragon Age games (Note: in earlier games,
 *          .rim are RIM files. See also class RIMFile in rimfile.h.)
 *  - .rimp: Patch resources
 */
class ERFFile : public Archive, public AuroraFile {
public:
	/** Take over this stream and read an ERF file out of it.
	 *
	 *  When the ERF is encrypted, use this password to decrypt it.
	 *
	 *  In Dragon Age: Origins and Dragon Age II, each DLC can have a unique
	 *  password with which many of the DLC's ERF archive are encrypted. This
	 *  password is used to directly decrypt the archive.
	 *
	 *  In Neverwinter Nights, each premium module consists of an unencrypted
	 *  .nwm file and an encrypted .hak file, both of which are ERF archives.
	 *  In this case, the password is the MD5 of the .nwm file. It is then used
	 *  to calculate the key to decrypt the .hak file.
	 */
	ERFFile(Common::SeekableReadStream *erf, const std::vector<byte> &password = std::vector<byte>());
	~ERFFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index, bool tryNoCopy = false) const;

	/** Return the year the ERF was built. */
	uint32 getBuildYear() const;
	/** Return the day of year the ERF was built. */
	uint32 getBuildDay() const;

	/** Return the description. */
	const LocString &getDescription() const;

	/** Return with which algorithm the name is hashed. */
	Common::HashAlgo getNameHashAlgo() const;

	static LocString getDescription(Common::SeekableReadStream &erf);
	static LocString getDescription(const Common::UString &fileName);

private:
	enum Encryption {
		kEncryptionNone        =  0, ///< No encryption at all.
		kEncryptionXOR         =  1, ///< XOR encryption as used by V2.2 and V3.0 (UNSUPPORTED!)
		kEncryptionBlowfishDAO =  2, ///< Blowfish encryption as used by Dragon Age: Origins (V2.2).
		kEncryptionBlowfishDA2 =  3, ///< Blowfish encryption as used by Dragon Age II (V3.0).
		kEncryptionBlowfishNWN = 16  ///< Blowfish encryption as used by Neverwinter Nights (V1.1).
	};

	enum Compression {
		kCompressionNone           = 0, ///< No compression as all.
		kCompressionBioWareZlib    = 1, ///< Compression using DEFLATE with an extra header byte.
		kCompressionHeaderlessZlib = 7  ///< Compression using DEFLATE with default parameters.
	};

	/** The header of an ERF file. */
	struct ERFHeader {
		uint32 resCount;         ///< Number of resources in this ERF.

		uint32 langCount;        ///< Number of language strings in the description.
		uint32 descriptionID;    ///< ID of the description.

		uint32 offDescription;   ///< Offset to the description.
		uint32 offKeyList;       ///< Offset to the key list.
		uint32 offResList;       ///< Offset to the resource list.

		uint32 descriptionSize;  ///< Number of bytes in the description structure.

		uint32 buildYear;        ///< The year the ERF was built.
		uint32 buildDay;         ///< The day of year the ERF was built.

		bool   isNWNPremium;     ///< Is this a Neverwinter Nights premium module?

		Common::ScopedArray<char> stringTable; ///< String table used for hashed ERFs.
		uint32 stringTableSize;                ///< Size of the string table.

		uint32 moduleID;         ///< ID of the module this ERF belongs to.

		Encryption  encryption;  ///< The encryption algorithm in use.
		Compression compression; ///< The compression algorithm in use.

		/** Digest of the encryption password, if any. */
		std::vector<byte> passwordDigest;

		ERFHeader();
		~ERFHeader();

		void clear();
		void clearStringTable();

		bool isSensible(size_t fileSize = SIZE_MAX);
	};

	/** Internal resource information. */
	struct IResource {
		uint32 offset;       ///< The offset of the resource within the ERF.
		uint32 packedSize;   ///< The resource's packed size.
		uint32 unpackedSize; ///< The resource's unpacked size.
	};

	typedef std::vector<IResource> IResourceList;

	Common::ScopedPtr<Common::SeekableReadStream> _erf;

	ERFHeader _header;

	/** The ERF's description. */
	LocString _description;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	/** The password we were given, if any. */
	std::vector<byte> _password;

	void load();

	// .--- Header
	static void verifyVersion(uint32 id, uint32 version, bool utf16le);

	static void readERFHeader(Common::SeekableReadStream &erf, ERFHeader &header, uint32 version,
	                          std::vector<byte> &password);
	static void readDescription(LocString &description, Common::SeekableReadStream &erf,
                              const ERFHeader &header);

	void readResources(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- V1.0
	static void readV10Header(Common::SeekableReadStream &erf, ERFHeader &header);
	void readV10ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readV10KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- V1.1
	static void readV11Header(Common::SeekableReadStream &erf, ERFHeader &header);
	void readV11KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- V2.0
	static void readV20Header(Common::SeekableReadStream &erf, ERFHeader &header);
	void readV20ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- V2.2
	static void readV22Header(Common::SeekableReadStream &erf, ERFHeader &header, uint32 &flags);
	void readV22ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- V3.0
	static void readV30Header(Common::SeekableReadStream &erf, ERFHeader &header, uint32 &flags);
	void readV30ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	// '---

	// .--- Encryption
	void verifyPasswordDigest();

	static Common::MemoryReadStream *decrypt(Common::SeekableReadStream &cryptStream,
	                                         Encryption encryption, const std::vector<byte> &password);
	static Common::MemoryReadStream *decrypt(Common::SeekableReadStream *cryptStream,
	                                         Encryption encryption, const std::vector<byte> &password);

	static Common::SeekableReadStream *decrypt(Common::SeekableReadStream &erf, size_t pos, size_t size,
	                                           Encryption encryption, const std::vector<byte> &password);
	static Common::SeekableReadStream *decrypt(Common::SeekableReadStream &erf, size_t size,
	                                           Encryption encryption, const std::vector<byte> &password);

	static bool decryptNWNPremiumHeader(Common::SeekableReadStream &erf, ERFHeader &header,
	                                    const std::vector<byte> &password);
	static bool findNWNPremiumKey      (Common::SeekableReadStream &erf, ERFHeader &header,
	                                    const std::vector<byte> &md5, std::vector<byte> &password);
	static void readNWNPremiumHeader   (Common::SeekableReadStream &erf, ERFHeader &header,
	                                    std::vector<byte> &password);

	void decryptNWNPremium();
	// '---

	// .--- Compression
	Common::SeekableReadStream *decompress(Common::MemoryReadStream *packedStream,
	                                       uint32 unpackedSize) const;

	Common::SeekableReadStream *decompressBiowareZlib   (Common::MemoryReadStream *packedStream,
	                                                     uint32 unpackedSize) const;
	Common::SeekableReadStream *decompressHeaderlessZlib(Common::MemoryReadStream *packedStream,
	                                                     uint32 unpackedSize) const;

	Common::SeekableReadStream *decompressZlib(const byte *compressedData, uint32 packedSize,
	                                           uint32 unpackedSize, int windowBits) const;
	// '---

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H

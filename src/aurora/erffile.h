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
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/archive.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold resource data of an ERF file. */
class ERFFile : public Archive, public AuroraBase {
public:
	ERFFile(Common::SeekableReadStream *erf);
	~ERFFile();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return the size of a resource. */
	uint32 getResourceSize(uint32 index) const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index, bool tryNoCopy = false) const;

	/** Return the description. */
	const LocString &getDescription() const;

	/** Return with which algorithm the name is hashed. */
	Common::HashAlgo getNameHashAlgo() const;

	static LocString getDescription(Common::SeekableReadStream &erf);
	static LocString getDescription(const Common::UString &fileName);

private:
	/** The header of an ERF file. */
	struct ERFHeader {
		uint32 resCount;        ///< Number of resources in this ERF.

		uint32 langCount;       ///< Number of language strings in the description.
		uint32 descriptionID;   ///< ID of the description.

		uint32 offDescription;  ///< Offset to the description.
		uint32 offKeyList;      ///< Offset to the key list.
		uint32 offResList;      ///< Offset to the resource list.

		char  *stringTable;     ///< String table used for hashed ERFs.
		uint32 stringTableSize; ///< Size of the string table.

		uint32 flags;           ///< Only used for the compression type ATM.
		uint32 moduleID;        ///< ID of the module this ERF belongs to.

		/** Digest of the encryption password, if any. */
		Common::UString passwordDigest;
	};

	/** Internal resource information. */
	struct IResource {
		uint32 offset;       ///< The offset of the resource within the ERF.
		uint32 packedSize;   ///< The resource's packed size.
		uint32 unpackedSize; ///< The resource's unpacked size.
	};

	typedef std::vector<IResource> IResourceList;

	Common::SeekableReadStream *_erf;

	ERFHeader _header;

	/** The ERF's description. */
	LocString _description;

	/** External list of resource names and types. */
	ResourceList _resources;

	/** Internal list of resource offsets and sizes. */
	IResourceList _iResources;

	void load(Common::SeekableReadStream &erf);

	static void verifyVersion(uint32 id, uint32 version, bool utf16le);

	static void readERFHeader(Common::SeekableReadStream &erf, ERFHeader &header, uint32 version);
	static void readDescription(LocString &description, Common::SeekableReadStream &erf,
                              const ERFHeader &header, uint32 version);

	void readResources(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V1.0
	void readV1ResList(Common::SeekableReadStream &erf, const ERFHeader &header);
	void readV1KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V1.1
	void readV11KeyList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V2.0
	void readV2ResList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V2.2
	void readV22ResList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// V3.0
	void readV3ResList(Common::SeekableReadStream &erf, const ERFHeader &header);

	// Compression
	uint32 getCompressionType() const;
	Common::SeekableReadStream *decompress(Common::MemoryReadStream *packedStream, uint32 unpackedSize) const;
	Common::SeekableReadStream *decompressBiowareZlib(Common::MemoryReadStream *packedStream, uint32 unpackedSize) const;
	Common::SeekableReadStream *decompressHeaderlessZlib(Common::MemoryReadStream *packedStream, uint32 unpackedSize) const;
	Common::SeekableReadStream *decompressZlib(byte *compressedData, uint32 packedSize, uint32 unpackedSize, int windowBits) const;

	const IResource &getIResource(uint32 index) const;
};

} // End of namespace Aurora

#endif // AURORA_ERFFILE_H

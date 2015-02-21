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
 *  Utility functions to handle files used in BioWare's Aurora engine.
 */

#ifndef AURORA_UTIL_H
#define AURORA_UTIL_H

#include <map>

#include "src/common/singleton.h"
#include "src/common/hash.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

namespace Aurora {

/** Return the human readable string of a Platform. */
Common::UString getPlatformDescription(Platform platform);


class FileTypeManager : public Common::Singleton<FileTypeManager> {
public:
	FileTypeManager();
	~FileTypeManager();

	/** Return the file type of a file name, detected by its extension. */
	FileType getFileType(const Common::UString &path);

	/** Return the file type of a file name, detected by its hashed extension. */
	FileType getFileType(Common::HashAlgo algo, uint64 hashedExtension);

	/** Return the file name with an added extensions according to the specified file type. */
	Common::UString addFileType(const Common::UString &path, FileType type);
	/** Return the file name with a swapped extensions according to the specified file type. */
	Common::UString setFileType(const Common::UString &path, FileType type);


private:
	/** File type <-> extension mapping. */
	struct Type {
		FileType type;
		const char *extension;
	};

	static const Type types[];

	typedef std::map<Common::UString, const Type *> ExtensionLookup;
	typedef std::map<FileType       , const Type *> TypeLookup;
	typedef std::map<uint64         , const Type *> HashLookup;

	ExtensionLookup _extensionLookup;
	TypeLookup      _typeLookup;
	HashLookup      _hashLookup[Common::kHashMAX];


	void buildExtensionLookup();
	void buildTypeLookup();
	void buildHashLookup(Common::HashAlgo algo);
};

} // End of namespace Aurora

/** Shortcut for accessing the file type manager. */
#define TypeMan ::Aurora::FileTypeManager::instance()

#endif // AURORA_UTIL_H

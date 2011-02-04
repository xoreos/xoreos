/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/zipfile.h
 *  A ZIP archive.
 */

#ifndef AURORA_ZIPFILE_H
#define AURORA_ZIPFILE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"
#include "aurora/archive.h"

namespace Common {
	class UString;
	class SeekableReadStream;
	class File;
	class ZipFile;
}

namespace Aurora {

/** A class encapsulating ZIP files for resource archive access. */
class ZIPFile : public Archive {
public:
	ZIPFile(const Common::UString &fileName);
	~ZIPFile();

	/** Clear the resource list. */
	void clear();

	/** Return the list of resources. */
	const ResourceList &getResources() const;

	/** Return a stream of the resource's contents. */
	Common::SeekableReadStream *getResource(uint32 index) const;

private:
	/** The actual zip file. */
	Common::ZipFile *_zipFile;

	/** External list of resource names and types. */
	ResourceList _resources;

	void load();
};

} // End of namespace Aurora

#endif // AURORA_ZIPFILE_H

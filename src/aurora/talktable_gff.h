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
 *  Handling BioWare's GFF'd talk tables.
 */

#ifndef AURORA_TALKTABLE_GFF_H
#define AURORA_TALKTABLE_GFF_H

#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/talktable.h"

namespace Aurora {

/** Loading BioWare's GFF'd talk tables. */
class TalkTable_GFF : public TalkTable {
public:
	TalkTable_GFF(Common::SeekableReadStream *tlk, Common::Encoding encoding);
	~TalkTable_GFF();

	bool hasEntry(uint32 strRef) const;

	const Common::UString &getString     (uint32 strRef) const;
	const Common::UString &getSoundResRef(uint32 strRef) const;


private:
	struct Entry {
		Common::UString text;

		const GFF4Struct *strct;

		Entry(const GFF4Struct *s = 0) : strct(s) { }
	};

	typedef std::map<uint32, Entry *> Entries;


	GFF4File *_gff;

	mutable Entries _entries;

	void load(Common::SeekableReadStream *tlk);
	void load02(const GFF4Struct &top);
	void load05(const GFF4Struct &top);

	void readString(Entry &entry) const;
	void readString02(Entry &entry) const;
	void readString05(Entry &entry) const;

	void readString05(Common::SeekableReadStream *huffTree,
	                  Common::SeekableReadStream *bitStream, Entry &entry) const;

	void clean();
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_GFF_H

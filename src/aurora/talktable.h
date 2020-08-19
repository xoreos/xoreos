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
 *  Base class for BioWare's talk tables.
 */

#ifndef AURORA_TALKTABLE_H
#define AURORA_TALKTABLE_H

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/encoding.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Aurora {

/** Base class for BioWare's talk tables.
 *
 *  A talk table contains localized string data, and optional voice-
 *  over resource names, indexed by a string reference ("StrRef").
 *
 *  A single talktable always contains strings in a single language
 *  (and for a single gender of the PC), and commonly all strings for
 *  a given context (module, campaign, ...).
 *
 *  See classes TalkTable_TLK and TalkTable_GFF for the two main
 *  formats a talk table can be found in.
 */
class TalkTable : boost::noncopyable {
public:
	virtual ~TalkTable();

	virtual bool hasEntry(uint32_t strRef) const = 0;

	virtual const Common::UString &getString     (uint32_t strRef) const = 0;
	virtual const Common::UString &getSoundResRef(uint32_t strRef) const = 0;

	virtual uint32_t getSoundID(uint32_t strRef) const = 0;

	/** Take over this stream and read a talk table (of either format) out of it. */
	static TalkTable *load(Common::SeekableReadStream *tlk, Common::Encoding encoding);


protected:
	TalkTable(Common::Encoding encoding);

	Common::Encoding _encoding;
};

} // End of namespace Aurora

#endif // AURORA_TALKTABLE_H

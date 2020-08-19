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

#include <memory>

#include "src/common/util.h"
#include "src/common/readstream.h"

#include "src/aurora/aurorafile.h"
#include "src/aurora/talktable.h"
#include "src/aurora/talktable_tlk.h"
#include "src/aurora/talktable_gff.h"

static const uint32_t kTLKID = MKTAG('T', 'L', 'K', ' ');
static const uint32_t kGFFID = MKTAG('G', 'F', 'F', ' ');

namespace Aurora {

TalkTable::TalkTable(Common::Encoding encoding) : _encoding(encoding) {
}

TalkTable::~TalkTable() {
}

TalkTable *TalkTable::load(Common::SeekableReadStream *tlk, Common::Encoding encoding) {
	std::unique_ptr<Common::SeekableReadStream> tlkStream(tlk);
	if (!tlkStream)
		return 0;

	size_t pos = tlkStream->pos();

	uint32_t id, version;
	bool utf16le;

	AuroraFile::readHeader(*tlkStream, id, version, utf16le);

	tlkStream->seek(pos);

	if (id == kTLKID)
		return new TalkTable_TLK(tlkStream.release(), encoding);

	if (id == kGFFID)
		return new TalkTable_GFF(tlkStream.release(), encoding);

	return 0;
}

} // End of namespace Aurora

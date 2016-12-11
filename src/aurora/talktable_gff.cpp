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

/* See the TLK description on the Dragon Age toolset wiki
 * (<http://social.bioware.com/wiki/datoolset/index.php/TLK>).
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/talktable_gff.h"
#include "src/aurora/gff4file.h"

static const uint32 kTLKID     = MKTAG('T', 'L', 'K', ' ');
static const uint32 kVersion02 = MKTAG('V', '0', '.', '2');
static const uint32 kVersion05 = MKTAG('V', '0', '.', '5');

namespace Aurora {

TalkTable_GFF::TalkTable_GFF(Common::SeekableReadStream *tlk, Common::Encoding encoding) :
	TalkTable(encoding) {

	load(tlk);
}

TalkTable_GFF::~TalkTable_GFF() {
}

bool TalkTable_GFF::hasEntry(uint32 strRef) const {
	return _entries.find(strRef) != _entries.end();
}

static const Common::UString kEmptyString = "";
const Common::UString &TalkTable_GFF::getString(uint32 strRef) const {
	Entries::iterator e = _entries.find(strRef);
	if (e == _entries.end())
		return kEmptyString;

	readString(*e->second);

	return e->second->text;
}

const Common::UString &TalkTable_GFF::getSoundResRef(uint32 UNUSED(strRef)) const {
	return kEmptyString;
}

uint32 TalkTable_GFF::getSoundID(uint32 UNUSED(strRef)) const {
	return kFieldIDInvalid;
}

void TalkTable_GFF::load(Common::SeekableReadStream *tlk) {
	assert(tlk);

	try {
		_gff.reset(new GFF4File(tlk, kTLKID));

		const GFF4Struct &top = _gff->getTopLevel();

		if      (_gff->getTypeVersion() == kVersion02)
			load02(top);
		else if (_gff->getTypeVersion() == kVersion05)
			load05(top);
		else
			throw Common::Exception("Unsupported GFF TLK file version %08X", _gff->getTypeVersion());

	} catch (Common::Exception &e) {
		e.add("Unable to load GFF TLK");
		throw;
	}
}

void TalkTable_GFF::load02(const GFF4Struct &top) {
	if (!top.hasField(kGFF4TalkStringList))
		return;

	const GFF4List &strings = top.getList(kGFF4TalkStringList);

	for (GFF4List::const_iterator s = strings.begin(); s != strings.end(); ++s) {
		if (!*s)
			continue;

		uint32 strRef = (*s)->getUint(kGFF4TalkStringID, 0xFFFFFFFF);
		if (strRef == 0xFFFFFFFF)
			continue;

		Common::ScopedPtr<Entry> entry(new Entry(*s));

		std::pair<Entries::iterator, bool> result = _entries.insert(std::make_pair(strRef, entry.get()));
		if (result.second)
			entry.release();
	}
}

void TalkTable_GFF::load05(const GFF4Struct &top) {
	if (!top.hasField(kGFF4HuffTalkStringList) ||
      !top.hasField(kGFF4HuffTalkStringHuffTree) ||
	    !top.hasField(kGFF4HuffTalkStringBitStream))
		return;

	const GFF4List &strings = top.getList(kGFF4HuffTalkStringList);

	for (GFF4List::const_iterator s = strings.begin(); s != strings.end(); ++s) {
		if (!*s)
			continue;

		uint32 strRef = (*s)->getUint(kGFF4HuffTalkStringID, 0xFFFFFFFF);
		if (strRef == 0xFFFFFFFF)
			continue;

		Common::ScopedPtr<Entry> entry(new Entry(*s));

		std::pair<Entries::iterator, bool> result = _entries.insert(std::make_pair(strRef, entry.get()));
		if (result.second)
			entry.release();
	}
}

void TalkTable_GFF::readString(Entry &entry) const {
	if (!entry.strct)
		return;

	if      (_gff->getTypeVersion() == kVersion02)
		readString02(entry);
	else if (_gff->getTypeVersion() == kVersion05)
		readString05(entry);

	entry.strct = 0;
}

void TalkTable_GFF::readString02(Entry &entry) const {
	if (_encoding != Common::kEncodingInvalid)
		entry.text = entry.strct->getString(kGFF4TalkString, _encoding);
	else
		entry.text = "[???]";
}

void TalkTable_GFF::readString05(Entry &entry) const {
	Common::ScopedPtr<Common::SeekableReadStream>
		huffTree (_gff->getTopLevel().getData(kGFF4HuffTalkStringHuffTree)),
		bitStream(_gff->getTopLevel().getData(kGFF4HuffTalkStringBitStream));

	readString05(huffTree.get(), bitStream.get(), entry);
}

void TalkTable_GFF::readString05(Common::SeekableReadStream *huffTree,
                                 Common::SeekableReadStream *bitStream, Entry &entry) const {

	if (!huffTree || !bitStream)
		return;

	/* Read a string encoded in a Huffman'd bitstream.
	 *
	 * The Huffman tree itself is made up of signed 32bit nodes:
	 *  - Positive values are internal nodes, encoding a child index
	 *  - Negative values are leaf nodes, encoding an UTF-16 character value
	 *
	 * Kudos to Rick (gibbed) (<http://gib.me/>).
	 */

	std::vector<uint16> utf16Str;

	const uint32 startOffset = entry.strct->getUint(kGFF4HuffTalkStringBitOffset);

	uint32 index = startOffset >> 5;
	uint32 shift = startOffset & 0x1F;

	do {
		ptrdiff_t e = (huffTree->size() / 8) - 1;

		while (e >= 0) {
			bitStream->seek(index * 4);
			const ptrdiff_t offset = (bitStream->readUint32LE() >> shift) & 1;

			huffTree->seek(((e * 2) + offset) * 4);
			e = huffTree->readSint32LE();

			shift++;
			index += (shift >> 5);

			shift %= 32;
		}

		utf16Str.push_back(TO_LE_16(0xFFFF - e));

	} while (utf16Str.back() != 0);

	const byte  *data = reinterpret_cast<const byte *>(&utf16Str[0]);
	const size_t size = utf16Str.size() * 2;

	entry.text = Common::readString(data, size, Common::kEncodingUTF16LE);
}

} // End of namespace Aurora

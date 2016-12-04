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
 *  Handling BioWare's localized strings.
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include "src/common/util.h"
#include "src/common/scopedptr.h"
#include "src/common/memreadstream.h"
#include "src/common/encoding.h"

#include "src/aurora/locstring.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/talkman.h"

namespace Aurora {

LocString::LocString() : _id(kStrRefInvalid) {
}

LocString::~LocString() {
}

void LocString::clear() {
	_id = kStrRefInvalid;

	_strings.clear();
}

bool LocString::empty() const {
	return (_id == kStrRefInvalid) && (_strings.empty() || getString().empty());
}

void LocString::swap(LocString &str) {
	SWAP(_id, str._id);

	_strings.swap(str._strings);
}

uint32 LocString::getID() const {
	return _id;
}

void LocString::setID(uint32 id) {
	_id = id;
}

bool LocString::hasString(Language language, LanguageGender gender) const {
	return hasString(LangMan.getLanguageID(language, gender));
}

bool LocString::hasString(uint32 languageID) const {
	return _strings.find(languageID) != _strings.end();
}

const Common::UString &LocString::getString(Language language, LanguageGender gender) const {
	return getString(LangMan.getLanguageID(language, gender));
}

static const Common::UString kEmpty;
const Common::UString &LocString::getString(uint32 languageID) const {
	StringMap::const_iterator s = _strings.find(languageID);
	if (s == _strings.end())
		return kEmpty;

	return s->second;
}

void LocString::setString(Language language, LanguageGender gender, const Common::UString &str) {
	return setString(LangMan.getLanguageID(language, gender), str);
}

void LocString::setString(Language language, const Common::UString &str) {
	setString(language, kLanguageGenderMale  , str);
	setString(language, kLanguageGenderFemale, str);
}

void LocString::setString(uint32 languageID, const Common::UString &str) {
	_strings[languageID] = str;
}

const Common::UString &LocString::getStrRefString() const {
	if (_id == kStrRefInvalid)
		return kEmpty;

	return TalkMan.getString(_id);
}

const Common::UString &LocString::getFirstString() const {
	if (_strings.empty())
		return getStrRefString();

	return _strings.begin()->second;
}

const Common::UString &LocString::getString() const {
	uint32 languageID = LangMan.getLanguageID(LangMan.getCurrentLanguageText(), LangMan.getCurrentGender());

	// Look whether we have an internal localized string
	if (hasString(languageID))
		return getString(languageID);

	// Try the differently gendered internal string
	if (hasString(LangMan.swapLanguageGender(languageID)))
		return getString(LangMan.swapLanguageGender(languageID));

	// Next, try the external localized one
	const Common::UString &refString = getStrRefString();
	if (!refString.empty())
		return refString;

	// If all else fails, just get the first one available
	return getFirstString();
}

void LocString::readString(uint32 languageID, Common::SeekableReadStream &stream) {
	uint32 length = stream.readUint32LE();

	std::pair<StringMap::iterator, bool> s = _strings.insert(std::make_pair(languageID, ""));
	if (length == 0)
		return;

	s.first->second = "[???]";

	Common::ScopedPtr<Common::MemoryReadStream> data(stream.readStream(length));
	Common::ScopedPtr<Common::MemoryReadStream> parsed(LangMan.preParseColorCodes(*data));

	Common::Encoding encoding = LangMan.getEncodingLocString(LangMan.getLanguageGendered(languageID));
	if (encoding != Common::kEncodingInvalid)
		s.first->second = Common::readString(*parsed, encoding);
}

void LocString::readLocSubString(Common::SeekableReadStream &stream) {
	uint32 languageID = stream.readUint32LE();

	readString(languageID, stream);
}

void LocString::readLocString(Common::SeekableReadStream &stream, uint32 id, uint32 count) {
	_id = id;

	while (count-- > 0)
		readLocSubString(stream);
}

void LocString::readLocString(Common::SeekableReadStream &stream) {
	uint32 id    = stream.readUint32LE();
	uint32 count = stream.readUint32LE();

	readLocString(stream, id, count);
}

} // End of namespace Aurora

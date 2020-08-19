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
 *  Types and functions related to language.
 */

#include <cassert>

#include "src/common/ustring.h"
#include "src/common/memreadstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/language.h"
#include "src/aurora/language_strings.h"

DECLARE_SINGLETON(Aurora::LanguageManager)

namespace Aurora {

LanguageManager::LanguageManager() :
	_currentLanguageText(kLanguageInvalid), _currentLanguageVoice(kLanguageInvalid),
	_currentGender(kLanguageGenderMale) {

}

LanguageManager::~LanguageManager() {
}

void LanguageManager::clear() {
	_langByID.clear();
	_langByLang.clear();

	_currentLanguageText  = kLanguageInvalid;
	_currentLanguageVoice = kLanguageInvalid;

	_currentGender = kLanguageGenderMale;
}

void LanguageManager::addLanguage(Language language, uint32_t id, Common::Encoding encoding) {
	Declaration declaration;

	declaration.language = language;
	declaration.id       = id;
	declaration.encoding = encoding;

	declaration.encodingLocString = encoding;

	addLanguage(declaration);
}

void LanguageManager::addLanguage(Language language, uint32_t id, Common::Encoding encoding,
                                  Common::Encoding encodingLocString) {
	Declaration declaration;

	declaration.language = language;
	declaration.id       = id;
	declaration.encoding = encoding;

	declaration.encodingLocString = encodingLocString;

	addLanguage(declaration);
}

void LanguageManager::addLanguage(const Declaration &languageDeclaration) {
	if (languageDeclaration.id != kLanguageInvalid)
		_langByID[languageDeclaration.id] = languageDeclaration;

	if (languageDeclaration.language != kLanguageInvalid)
		_langByLang[languageDeclaration.language] = languageDeclaration;
}

void LanguageManager::addLanguages(const Declaration *languageDeclarations, size_t count) {
	while (count-- > 0)
		addLanguage(*languageDeclarations++);
}

std::vector<Language> LanguageManager::getLanguages() const {
	std::vector<Language> languages;

	for (LanguageByLanguage::const_iterator l = _langByLang.begin(); l != _langByLang.end(); ++l)
		languages.push_back(l->first);

	return languages;
}

const LanguageManager::Declaration *LanguageManager::find(Language language) const {
	LanguageByLanguage::const_iterator l = _langByLang.find(language);
	if (l != _langByLang.end())
		return &l->second;

	return 0;
}

const LanguageManager::Declaration *LanguageManager::find(uint32_t id) const {
	LanguageByID::const_iterator l = _langByID.find(id);
	if (l != _langByID.end())
		return &l->second;

	return 0;
}

uint32_t LanguageManager::getLanguageID(Language language) const {
	const Declaration *l = find(language);
	if (!l)
		return kLanguageInvalid;

	return l->id;
}

uint32_t LanguageManager::getLanguageID(Language language, LanguageGender gender) const {
	const Declaration *l = find(language);
	if (!l)
		return kLanguageInvalid;

	if (gender == kLanguageGenderCurrent)
		gender = getCurrentGender();

	return convertLanguageIDToGendered(l->id, gender);
}

Language LanguageManager::getLanguage(uint32_t languageID) const {
	const Declaration *l = find(languageID);
	if (!l)
		return kLanguageInvalid;

	return l->language;
}

Language LanguageManager::getLanguage(uint32_t languageID, LanguageGender &gender) const {
	gender = getLanguageGender(languageID);

	const Declaration *l = find(convertLanguageIDToUngendered(languageID));
	if (!l)
		return kLanguageInvalid;

	return l->language;
}

Language LanguageManager::getLanguageGendered(uint32_t languageID) const {
	LanguageGender gender;
	return getLanguage(languageID, gender);
}

Common::Encoding LanguageManager::getEncoding(Language language) const {
	const Declaration *l = find(language);
	if (!l)
		return Common::kEncodingInvalid;

	return l->encoding;
}

Common::Encoding LanguageManager::getEncodingLocString(Language language) const {
	const Declaration *l = find(language);
	if (!l)
		return Common::kEncodingInvalid;

	return l->encodingLocString;
}

void LanguageManager::setCurrentLanguage(Language language) {
	setCurrentLanguageText(language);
	setCurrentLanguageVoice(language);
}

void LanguageManager::setCurrentLanguage(Language languageText, Language languageVoice) {
	setCurrentLanguageText(languageText);
	setCurrentLanguageVoice(languageVoice);
}

void LanguageManager::setCurrentGender(LanguageGender gender) {
	_currentGender = gender;
}

Language LanguageManager::getCurrentLanguageText() const {
	return _currentLanguageText;
}

Language LanguageManager::getCurrentLanguageVoice() const {
	return _currentLanguageVoice;
}

void LanguageManager::setCurrentLanguageText(Language language) {
	_currentLanguageText = language;
}

void LanguageManager::setCurrentLanguageVoice(Language language) {
	_currentLanguageVoice = language;
}

LanguageGender LanguageManager::getCurrentGender() const {
	return _currentGender;
}

Common::Encoding LanguageManager::getCurrentEncoding() const {
	return getEncoding(getCurrentLanguageText());
}

Common::Encoding LanguageManager::getCurrentEncodingLocString() const {
	return getEncodingLocString(getCurrentLanguageText());
}

uint32_t LanguageManager::convertLanguageIDToGendered(uint32_t languageID, LanguageGender gender) {
	assert(((size_t) gender) < kLanguageGenderMAX);

	if (languageID == kLanguageInvalid)
		return kLanguageInvalid;

	// In gendered language use, the ID is:
	// - ID * 2 + 0  for male
	// - ID * 2 + 1  for female

	return languageID * 2 + ((size_t) gender);
}

uint32_t LanguageManager::convertLanguageIDToUngendered(uint32_t languageID) {
	if (languageID == kLanguageInvalid)
		return kLanguageInvalid;

	return languageID / 2;
}

LanguageGender LanguageManager::getLanguageGender(uint32_t languageID) {
	if (languageID == kLanguageInvalid)
		return kLanguageGenderMale;

	return (LanguageGender) (languageID % 2);
}

uint32_t LanguageManager::swapLanguageGender(uint32_t languageID) {
	if (languageID == kLanguageInvalid)
		return kLanguageInvalid;

	return languageID ^ 1;
}

Common::UString LanguageManager::getLanguageName(Language language) {
	if (language == kLanguageChinese)
		return "Chinese";

	if (((uint32_t) language >= kLanguageMAX))
		return "Invalid";

	return kLanguageNames[language];
}

Language LanguageManager::parseLanguage(const Common::UString &str) {
	if (str.empty())
		return kLanguageInvalid;

	Common::UString lowerStr = str.toLower();

	for (size_t i = 0; i < ARRAYSIZE(kLanguageStrings); i++) {
		for (size_t j = 0; j < ARRAYSIZE(kLanguageStrings[i].strings); j++) {
			if (!kLanguageStrings[i].strings[j])
				break;

			if (lowerStr == kLanguageStrings[i].strings[j])
				return kLanguageStrings[i].language;
		}
	}

	return kLanguageInvalid;
}

Common::MemoryReadStream *LanguageManager::preParseColorCodes(Common::SeekableReadStream &stream) {
	Common::MemoryWriteStreamDynamic output;

	output.reserve(stream.size());

	int state = 0;

	std::vector<byte> collect;
	collect.reserve(6);

	byte color[3];

	byte b;
	while (stream.read(&b, 1) == 1) {
		if (state == 0) {
			if (b == '<') {
				collect.push_back(b);
				state = 1;
			} else
				output.writeByte(b);

			continue;
		}

		if (state == 1) {
			if (b == 'c') {
				collect.push_back(b);
				state = 2;
			} else {
				output.write(&collect[0], collect.size());
				output.writeByte(b);
				collect.clear();
				state = 0;
			}

			continue;
		}

		if ((state == 2) || (state == 3) || (state == 4)) {
			collect.push_back(b);
			color[state - 2] = b;
			state++;

			continue;
		}

		if (state == 5) {
			if (b == '>') {
				Common::UString c = Common::UString::format("<c%02X%02X%02X%02X>",
				                    (uint8_t) color[0], (uint8_t) color[1], (uint8_t) color[2], (uint8_t) 0xFF);

				output.writeString(c);
				collect.clear();
				state = 0;

			} else {
				output.write(&collect[0], collect.size());
				output.writeByte(b);
				collect.clear();
				state = 0;
			}

			continue;
		}
	}

	return new Common::MemoryReadStream(output.getData(), output.size(), true);
}

} // End of namespace Aurora

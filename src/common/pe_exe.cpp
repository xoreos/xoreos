/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file common/pe_exe.cpp
 *  Portable executable parsing.
 */

#include "common/pe_exe.h"
#include "common/stream.h"

namespace Common {

PEResourceID::PEResourceID() : _idType(kIDTypeNull) {
}

PEResourceID::PEResourceID(UString x) : _idType(kIDTypeString), _name(x) {
}

PEResourceID::PEResourceID(uint32 x) : _idType(kIDTypeNumerical), _id(x) {
}

PEResourceID &PEResourceID::operator=(UString string) {
	_name = string;
	_idType = kIDTypeString;
	return *this;
}

PEResourceID &PEResourceID::operator=(uint32 x) {
	_id = x;
	_idType = kIDTypeNumerical;
	return *this;
}

bool PEResourceID::operator==(const UString &x) const {
	return _idType == kIDTypeString && _name.equalsIgnoreCase(x);
}

bool PEResourceID::operator==(const uint32 &x) const {
	return _idType == kIDTypeNumerical && _id == x;
}

bool PEResourceID::operator==(const PEResourceID &x) const {
	if (_idType != x._idType)
		return false;
	if (_idType == kIDTypeString)
		return _name.equalsIgnoreCase(x._name);
	if (_idType == kIDTypeNumerical)
		return _id == x._id;
	return true;
}

bool PEResourceID::operator<(const PEResourceID &x) const {
	return toString() < x.toString();
}

UString PEResourceID::getString() const {
	if (_idType != kIDTypeString)
		return "";

	return _name;
}

uint32 PEResourceID::getID() const {
	if (_idType != kIDTypeNumerical)
		return 0xffffffff;

	return _id;
}

UString PEResourceID::toString() const {
	if (_idType == kIDTypeString)
		return _name;
	else if (_idType == kIDTypeNumerical) {
		static char name[9];
		sprintf(name, "%08x", _id);
		name[8] = 0;
		return name;
	}

	return "";
}

PEResources::PEResources() {
	_exe = 0;
}

PEResources::~PEResources() {
	clear();
}

void PEResources::clear() {
	_sections.clear();
	_resources.clear();
	delete _exe; _exe = 0;
}

bool PEResources::loadFromEXE(SeekableReadStream *stream) {
	clear();

	if (!stream)
		return false;

	if (stream->readUint16BE() != 'MZ')
		return false;

	stream->skip(58);

	uint32 peOffset = stream->readUint32LE();

	if (!peOffset || peOffset >= (uint32)stream->size())
		return false;

	stream->seek(peOffset);

	if (stream->readUint32BE() != MKTAG('P', 'E', '\0', '\0'))
		return false;

	stream->skip(2);
	uint16 sectionCount = stream->readUint16LE();
	stream->skip(12);
	uint16 optionalHeaderSize = stream->readUint16LE();
	stream->skip(optionalHeaderSize + 2);

	// Read in all the sections
	for (uint16 i = 0; i < sectionCount; i++) {
		char sectionName[9];
		stream->read(sectionName, 8);
		sectionName[8] = 0;

		Section section;
		stream->skip(4);
		section.virtualAddress = stream->readUint32LE();
		section.size = stream->readUint32LE();
		section.offset = stream->readUint32LE();
		stream->skip(16);

		_sections[sectionName] = section;
	}

	// Currently, we require loading a resource section
	if (_sections.find(".rsrc") == _sections.end()) {
		clear();
		return false;
	}

	_exe = stream;

	Section &resSection = _sections[".rsrc"];
	parseResourceLevel(resSection, resSection.offset, 0);

	return true;
}

void PEResources::parseResourceLevel(Section &section, uint32 offset, int level) {
	_exe->seek(offset + 12);

	uint16 entryCount = _exe->readUint16LE(); // named entry count
	entryCount += _exe->readUint16LE();       // id entry count

	for (uint32 i = 0; i < entryCount; i++) {
		uint32 value = _exe->readUint32LE();

		PEResourceID id;

		if (value & 0x80000000) {
			value &= 0x7fffffff;

			uint32 startPos = _exe->pos();
			_exe->seek(section.offset + (value & 0x7fffffff));

			// Read in the name, UTF-16LE
			uint16 nameLength = _exe->readUint16LE();
			UString name;
			name.readFixedUTF16LE(*_exe, nameLength);

			_exe->seek(startPos);

			id = name;
		} else {
			id = value;
		}

		uint32 nextOffset = _exe->readUint32LE();
		uint32 lastOffset = _exe->pos();

		if (level == 0)
			_curType = id;
		else if (level == 1)
			_curName = id;
		else if (level == 2)
			_curLang = id;

		if (level < 2) {
			// Time to dive down further
			parseResourceLevel(section, section.offset + (nextOffset & 0x7fffffff), level + 1);
		} else {
			_exe->seek(section.offset + nextOffset);

			Resource resource;
			resource.offset = _exe->readUint32LE() + section.offset - section.virtualAddress;
			resource.size = _exe->readUint32LE();

			//status("Found resource '%s' '%s' '%s' at %d of size %d", _curType.toString().c_str(),
			//		_curName.toString().c_str(), _curLang.toString().c_str(), resource.offset, resource.size);

			_resources[_curType][_curName][_curLang] = resource;
		}

		_exe->seek(lastOffset);
	}
}

const std::vector<PEResourceID> PEResources::getTypeList() const {
	std::vector<PEResourceID> array;

	if (!_exe)
		return array;

	for (TypeMap::const_iterator it = _resources.begin(); it != _resources.end(); ++it)
		array.push_back(it->first);

	return array;
}

const std::vector<PEResourceID> PEResources::getNameList(const PEResourceID &type) const {
	std::vector<PEResourceID> array;

	if (!_exe || _resources.find(type) == _resources.end())
		return array;

	const NameMap &nameMap = _resources.find(type)->second;

	for (NameMap::const_iterator it = nameMap.begin(); it != nameMap.end(); ++it)
		array.push_back(it->first);

	return array;
}

const std::vector<PEResourceID> PEResources::getLangList(const PEResourceID &type, const PEResourceID &name) const {
	std::vector<PEResourceID> array;

	if (!_exe || _resources.find(type) == _resources.end())
		return array;

	const NameMap &nameMap = _resources.find(type)->second;

	if (nameMap.find(name) == nameMap.end())
		return array;

	const LangMap &langMap = nameMap.find(name)->second;

	for (LangMap::const_iterator it = langMap.begin(); it != langMap.end(); ++it)
		array.push_back(it->first);

	return array;
}

SeekableReadStream *PEResources::getResource(const PEResourceID &type, const PEResourceID &name) {
	std::vector<PEResourceID> langList = getLangList(type, name);

	if (langList.empty())
		return 0;

	const Resource &resource = _resources.find(type)->second.find(name)->second.find(langList[0])->second; // fun stuff
	_exe->seek(resource.offset);
	return _exe->readStream(resource.size);
}

SeekableReadStream *PEResources::getResource(const PEResourceID &type, const PEResourceID &name, const PEResourceID &lang) {
	if (!_exe || _resources.find(type) == _resources.end())
		return 0;

	const NameMap &nameMap = _resources.find(type)->second;

	if (nameMap.find(name) == nameMap.end())
		return 0;

	const LangMap &langMap = nameMap.find(name)->second;

	if (langMap.find(lang) == langMap.end())
		return 0;

	const Resource &resource = langMap.find(lang)->second;
	_exe->seek(resource.offset);
	return _exe->readStream(resource.size);
}

} // End of namespace Common

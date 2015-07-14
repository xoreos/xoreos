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
 *  Handling version V4.0/V4.1 of BioWare's GFFs (generic file format).
 */

/* See the GFF description on the Dragon Age toolset wiki
 * (<http://social.bioware.com/wiki/datoolset/index.php/GFF>).
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/encoding.h"
#include "src/common/strutil.h"

#include "src/aurora/gff4file.h"
#include "src/aurora/util.h"
#include "src/aurora/resman.h"

static const uint32 kGFFID     = MKTAG('G', 'F', 'F', ' ');
static const uint32 kVersion40 = MKTAG('V', '4', '.', '0');
static const uint32 kVersion41 = MKTAG('V', '4', '.', '1');

namespace Aurora {

void GFF4File::Header::read(Common::SeekableReadStream &gff4, uint32 version) {
	platformID   = gff4.readUint32BE();
	type         = gff4.readUint32BE();
	typeVersion  = gff4.readUint32BE();
	structCount  = gff4.readUint32LE();

	stringCount  = 0;
	stringOffset = 0xFFFFFFFF;

	if (version == kVersion41) {
		stringCount  = gff4.readUint32LE();
		stringOffset = gff4.readUint32LE();
	}

	hasSharedStrings = (stringCount > 0) || (stringOffset != 0xFFFFFFFF);

	dataOffset = gff4.readUint32LE();
}


GFF4File::GFF4File(Common::SeekableReadStream *gff4, uint32 type) :
	_stream(gff4), _topLevelStruct(0) {

	load(type);
}

GFF4File::GFF4File(const Common::UString &gff4, FileType fileType, uint32 type) :
	_stream(0), _topLevelStruct(0) {

	_stream = ResMan.getResource(gff4, fileType);
	if (!_stream)
		throw Common::Exception("No such GFF4 \"%s\"", TypeMan.setFileType(gff4, fileType).c_str());

	load(type);
}

GFF4File::~GFF4File() {
	clear();
}

void GFF4File::clear() {
	delete _stream;
	_stream = 0;

	for (StructMap::iterator s = _structs.begin(); s != _structs.end(); ++s)
		delete s->second;

	_structs.clear();
	_topLevelStruct = 0;
}

uint32 GFF4File::getType() const {
	return _header.type;
}

uint32 GFF4File::getTypeVersion() const {
	return _header.typeVersion;
}

uint32 GFF4File::getPlatform() const {
	return _header.platformID;
}

const GFF4Struct &GFF4File::getTopLevel() const {
	assert(_topLevelStruct);

	return *_topLevelStruct;
}

// --- Loader ---

void GFF4File::load(uint32 type) {
	try {

		loadHeader(type);
		loadStructs();
		loadStrings();

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading GFF4 file");
		throw;
	}
}

void GFF4File::loadHeader(uint32 type) {
	readHeader(*_stream);

	if (_id != kGFFID)
		throw Common::Exception("Not a GFF4 file");

	if ((_version != kVersion40) && (_version != kVersion41))
		throw Common::Exception("Unsupported GFF4 file version %s", Common::debugTag(_version).c_str());

	_header.read(*_stream, _version);

	if ((type != 0xFFFFFFFF) && (_header.type != type))
		throw Common::Exception("GFF4 has invalid type (want %s, got %s)",
				Common::debugTag(type).c_str(), Common::debugTag(_header.type).c_str());
}

void GFF4File::loadStructs() {
	// Load the struct templates

	static const uint32 kStructTemplateSize = 16;
	const uint32 structTemplateStart = _stream->pos();

	_structTemplates.resize(_header.structCount);
	for (uint32 i = 0; i < _header.structCount; i++) {
		_stream->seek(structTemplateStart + i * kStructTemplateSize);

		StructTemplate &strct = _structTemplates[i];

		// Read struct properties

		strct.index = i;
		strct.label = _stream->readUint32BE();

		const uint32 fieldCount  = _stream->readUint32LE();
		const uint32 fieldOffset = _stream->readUint32LE();

		strct.size = _stream->readUint32LE();

		// Check if we need to read fields
		if (fieldOffset == 0xFFFFFFFF) {
			if (fieldCount != 0)
				throw Common::Exception("GFF4: fieldOffset NULL, but fieldCount %u", fieldCount);

			continue;
		}

		_stream->seek(fieldOffset);

		// Read the field declarations

		strct.fields.resize(fieldCount);
		for (uint32 j = 0; j < fieldCount; j++) {
			StructTemplate::Field &field = strct.fields[j];

			field.label  = _stream->readUint32LE();
			field.type   = _stream->readUint16LE();
			field.flags  = _stream->readUint16LE();
			field.offset = _stream->readUint32LE();
		}
	}

	// And load the top level struct, which itself recurses into field structs
	_topLevelStruct = new GFF4Struct(*this, _header.dataOffset, _structTemplates[0]);
	_topLevelStruct->_refCount++;
}

void GFF4File::loadStrings() {
	if (!_header.hasSharedStrings)
		return;

	_sharedStrings.resize(_header.stringCount);

	_stream->seek(_header.stringOffset);
	for (uint32 i = 0; i < _header.stringCount; i++)
		_sharedStrings[i] = Common::readString(*_stream, Common::kEncodingUTF8);
}

// --- Helpers for GFF4Struct ---

void GFF4File::registerStruct(uint64 id, GFF4Struct *strct) {
	std::pair<StructMap::iterator, bool> result;

	result = _structs.insert(std::make_pair(id, strct));
	if (!result.second)
		throw Common::Exception("GFF4: Duplicate struct");
}

void GFF4File::unregisterStruct(uint64 id) {
	_structs.erase(id);
}

GFF4Struct *GFF4File::findStruct(uint64 id) {
	StructMap::iterator s = _structs.find(id);
	if (s == _structs.end())
		return 0;

	return s->second;
}

Common::SeekableReadStream &GFF4File::getStream(uint32 offset) const {
	_stream->seek(offset);

	return *_stream;
}

uint32 GFF4File::getDataOffset() const {
	return _header.dataOffset;
}

const GFF4File::StructTemplate &GFF4File::getStructTemplate(uint32 i) const {
	assert(i < _structTemplates.size());

	return _structTemplates[i];
}

bool GFF4File::hasSharedStrings() const {
	return _header.hasSharedStrings;
}

Common::UString GFF4File::getSharedString(uint32 i) const {
	if (i == 0xFFFFFFFF)
		return "";

	assert(i < _sharedStrings.size());

	return _sharedStrings[i];
}


GFF4Struct::Field::Field() : label(0), type(kIFieldTypeNone), offset(0xFFFFFFFF),
	isList(false), isReference(false), isGeneric(false), structIndex(0) {

}

GFF4Struct::Field::Field(uint32 l, uint16 t, uint16 f, uint32 o, bool g) :
	label(l), offset(o), isGeneric(g) {

	isList      = (f & 0x8000) != 0;
	isReference = (f & 0x2000) != 0;

	// Map the struct flag to the struct type and index, if necessary
	const bool isStruct = (f & 0x4000) != 0;
	if (isStruct) {
		type        = kIFieldTypeStruct;
		structIndex = t;
	} else {
		type        = (IFieldType) t;
		structIndex = 0;
	}

	// A string is always read by reference. An extra reference flag is superfluous.
	if (type == kIFieldTypeString)
		isReference = false;
}

GFF4Struct::Field::~Field() {
}


GFF4Struct::GFF4Struct(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt) :
	_parent(&parent), _label(tmplt.label), _refCount(0), _fieldCount(0) {

	_id = generateID(offset, &tmplt);
	parent.registerStruct(_id, this);

	try {
		load(parent, offset, tmplt);
	} catch (...) {
		parent.unregisterStruct(_id);
		throw;
	}
}

GFF4Struct::GFF4Struct(GFF4File &parent, const Field &genericParent) :
	_parent(&parent), _label(0), _refCount(0), _fieldCount(0) {

	_id = generateID(genericParent.offset);
	parent.registerStruct(_id, this);

	try {
		load(parent, genericParent);
	} catch (...) {
		parent.unregisterStruct(_id);
		throw;
	}
}

GFF4Struct::~GFF4Struct() {
}

uint32 GFF4Struct::getLabel() const {
	return _label;
}

// --- Loader ---

void GFF4Struct::load(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt) {
	for (size_t i = 0; i < tmplt.fields.size(); i++) {
		const GFF4File::StructTemplate::Field &field = tmplt.fields[i];

		// Calculate the offset for the field data, but guard against NULL pointers
		uint32 fieldOffset = offset + field.offset;
		if ((offset == 0xFFFFFFFF) || (field.offset == 0xFFFFFFFF))
			fieldOffset = 0xFFFFFFFF;

		// Load the field and its struct(s), if any
		Field &f = _fields[field.label] = Field(field.label, field.type, field.flags, fieldOffset);
		if (f.type == kIFieldTypeStruct)
			loadStructs(parent, f);
		if (f.type == kIFieldTypeGeneric)
			loadGeneric(parent, f);
	}

	_fieldCount = _fields.size();
}

void GFF4Struct::loadStructs(GFF4File &parent, Field &field) {
	if (field.offset == 0xFFFFFFFF)
		return;

	const GFF4File::StructTemplate &tmplt = parent.getStructTemplate(field.structIndex);

	Common::SeekableReadStream &data = parent.getStream(field.offset);

	const uint32 structCount = getListCount(data, field);
	const uint32 structSize  = field.isReference ? 4 : tmplt.size;
	const uint32 structStart = data.pos();

	field.structs.resize(structCount, 0);
	for (uint32 i = 0; i < structCount; i++) {
		const uint32 offset = getDataOffset(field.isReference, structStart + i * structSize);
		if (offset == 0xFFFFFFFF)
			continue;

		GFF4Struct *strct = parent.findStruct(generateID(offset, &tmplt));
		if (!strct)
			strct = new GFF4Struct(parent, offset, tmplt);

		strct->_refCount++;

		field.structs[i] = strct;
	}
}

void GFF4Struct::loadGeneric(GFF4File &parent, Field &field) {
	field.offset = getDataOffset(field.isList, field.offset);
	if (field.offset == 0xFFFFFFFF)
		return;

	GFF4Struct *strct = parent.findStruct(generateID(field.offset));
	if (!strct)
		strct = new GFF4Struct(parent, field);

	strct->_refCount++;

	field.structs.push_back(strct);
}

void GFF4Struct::load(GFF4File &parent, const Field &genericParent) {
	static const uint32 kGenericSize = 8;

	Common::SeekableReadStream &data = parent.getStream(genericParent.offset);

	const uint32 genericCount = genericParent.isList ? data.readUint32LE() : 1;
	const uint32 genericStart = data.pos();

	for (uint32 i = 0; i < genericCount; i++) {
		data.seek(genericStart + i * kGenericSize);

		const uint16 fieldType   = data.readUint16LE();
		const uint16 fieldFlags  = data.readUint16LE();

		const uint32 fieldOffset = getDataOffset(genericParent.isReference, data.pos());

		if (fieldOffset == 0xFFFFFFFF)
			continue;

		// Load the field and its struct(s), if any
		Field &f = _fields[i] = Field(i, fieldType, fieldFlags, fieldOffset, true);
		if (f.type == kIFieldTypeStruct)
			loadStructs(parent, f);
	}

	_fieldCount = genericCount;
}

uint64 GFF4Struct::generateID(uint32 offset, const GFF4File::StructTemplate *tmplt) {
	return (((uint64) offset) << 32) | (tmplt ? tmplt->index : 0xFFFFFFFF);
}

// --- Field properties ---

size_t GFF4Struct::getFieldCount() const {
	return _fieldCount;
}

bool GFF4Struct::hasField(uint32 field) const {
	return getField(field) != 0;
}

GFF4Struct::FieldType GFF4Struct::getFieldType(uint32 field) const {
	bool isList;
	return getFieldType(field, isList);
}

GFF4Struct::FieldType GFF4Struct::getFieldType(uint32 field, bool &isList) const {
	const Field *f = getField(field);
	if (!f)
		return kFieldTypeNone;

	isList = f->isList;

	return convertFieldType(f->type);
}

// --- Field value reader helpers ---

const GFF4Struct::Field *GFF4Struct::getField(uint32 field) const {
	FieldMap::const_iterator f = _fields.find(field);
	if (f == _fields.end())
		return 0;

	return &f->second;
}

uint32 GFF4Struct::getDataOffset(bool isReference, uint32 offset) const {
	if (!isReference || (offset == 0xFFFFFFFF))
		return offset;

	Common::SeekableReadStream &data = _parent->getStream(offset);

	offset = data.readUint32LE();
	if (offset == 0xFFFFFFFF)
		return offset;

	return _parent->getDataOffset() + offset;
}

uint32 GFF4Struct::getDataOffset(const Field &field) const {
	if (field.type == kIFieldTypeStruct)
		return 0xFFFFFFFF;

	return getDataOffset(field.isReference, field.offset);
}

Common::SeekableReadStream *GFF4Struct::getData(const Field &field) const {
	const uint32 offset = getDataOffset(field);
	if (offset == 0xFFFFFFFF)
		return 0;

	return &_parent->getStream(offset);
}

Common::SeekableReadStream *GFF4Struct::getField(uint32 fieldID, const Field *&field) const {
	if (!(field = getField(fieldID)))
		return 0;

	return getData(*field);
}

GFF4Struct::FieldType GFF4Struct::convertFieldType(IFieldType type) const {
	switch (type) {
		case kIFieldTypeUint64:
		case kIFieldTypeUint8:
		case kIFieldTypeUint16:
		case kIFieldTypeUint32:
			return kFieldTypeUint;

		case kIFieldTypeSint8:
		case kIFieldTypeSint16:
		case kIFieldTypeSint32:
		case kIFieldTypeSint64:
			return kFieldTypeSint;

		case kIFieldTypeFloat32:
		case kIFieldTypeFloat64:
			return kFieldTypeDouble;

		case kIFieldTypeString:
		case kIFieldTypeASCIIString:
			return kFieldTypeString;

		case kIFieldTypeTlkString:
			return kFieldTypeTalkString;

		case kIFieldTypeVector3f:
			return kFieldTypeVector3;

		case kIFieldTypeVector4f:
		case kIFieldTypeQuaternionf:
		case kIFieldTypeColor4f:
			return kFieldTypeVector4;

		case kIFieldTypeMatrix4x4f:
			return kFieldTypeMatrix;

		case kIFieldTypeStruct:
			return kFieldTypeStruct;

		case kIFieldTypeGeneric:
			return kFieldTypeGeneric;

		default:
			break;
	}

	return kFieldTypeNone;
}

uint32 GFF4Struct::getVectorMatrixLength(const Field &field, uint32 maxLength) const {
	uint32 length;
	if       (field.type == kIFieldTypeVector3f)
		length =  3;
	else if ((field.type == kIFieldTypeVector4f)    ||
	         (field.type == kIFieldTypeQuaternionf) ||
	         (field.type == kIFieldTypeColor4f))
		length =  4;
	else if  (field.type == kIFieldTypeMatrix4x4f)
		length = 16;
	else
		throw Common::Exception("GFF4: Field is not of Vector/Matrix type");

	if (length > maxLength)
		throw Common::Exception("GFF4: Vector/Matrix type is too long (%d > %d)", length, maxLength);

	return length;
}

uint32 GFF4Struct::getListCount(Common::SeekableReadStream &data, const Field &field) const {
	if (!field.isList)
		return 1;

	const uint32 listOffset = data.readUint32LE();
	if (listOffset == 0xFFFFFFFF)
		return 0;

	data.seek(_parent->getDataOffset() + listOffset);

	return data.readUint32LE();
}

uint32 GFF4Struct::getFieldSize(IFieldType type) const {
	switch (type) {
		case kIFieldTypeUint8:
		case kIFieldTypeSint8:
			return 1;

		case kIFieldTypeUint16:
		case kIFieldTypeSint16:
			return 2;

		case kIFieldTypeUint32:
		case kIFieldTypeSint32:
		case kIFieldTypeFloat32:
			return 4;

		case kIFieldTypeUint64:
		case kIFieldTypeSint64:
		case kIFieldTypeFloat64:
			return 8;

		case kIFieldTypeVector3f:
			return 3 * 4;

		case kIFieldTypeVector4f:
		case kIFieldTypeQuaternionf:
		case kIFieldTypeColor4f:
			return 4 * 4;

		case kIFieldTypeMatrix4x4f:
			return 16 * 4;

		case kIFieldTypeTlkString:
			return 2 * 4;

		case kIFieldTypeString:
			// The raw form is a pointer...
			return 4;

		case kIFieldTypeASCIIString:
			// Actually depending on the content...
			return 4;

		default:
			break;
	}

	return 0;
}

// --- Low-level value readers ---

uint64 GFF4Struct::getUint(Common::SeekableReadStream &data, IFieldType type) const {
	switch (type) {
		case kIFieldTypeUint8:
			return (uint64) data.readByte();

		case kIFieldTypeSint8:
			return (uint64) ((int64) data.readSByte());

		case kIFieldTypeUint16:
			return (uint64) data.readUint16LE();

		case kIFieldTypeSint16:
			return (uint64) ((int64) data.readSint16LE());

		case kIFieldTypeUint32:
			return (uint64) data.readUint32LE();

		case kIFieldTypeSint32:
			return (uint64) ((int64) data.readSint32LE());

		case kIFieldTypeUint64:
			return (uint64) data.readUint64LE();

		case kIFieldTypeSint64:
			return (uint64) ((int64) data.readSint64LE());

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not an int type");
}

int64 GFF4Struct::getSint(Common::SeekableReadStream &data, IFieldType type) const {
	switch (type) {
		case kIFieldTypeUint8:
			return (int64) ((uint64) data.readByte());

		case kIFieldTypeSint8:
			return (int64) data.readSByte();

		case kIFieldTypeUint16:
			return (int64) ((uint64) data.readUint16LE());

		case kIFieldTypeSint16:
			return (int64) data.readSint16LE();

		case kIFieldTypeUint32:
			return (int64) ((uint64) data.readUint32LE());

		case kIFieldTypeSint32:
			return (int64) data.readSint32LE();

		case kIFieldTypeUint64:
			return (int64) ((uint64) data.readUint64LE());

		case kIFieldTypeSint64:
			return (int64) data.readSint64LE();

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not an int type");
}

double GFF4Struct::getDouble(Common::SeekableReadStream &data, IFieldType type) const {
	switch (type) {
		case kIFieldTypeFloat32:
			return (double) data.readIEEEFloatLE();

		case kIFieldTypeFloat64:
			return (double) data.readIEEEDoubleLE();

		case kIFieldTypeNDSFixed:
			return readNintendoFixedPoint(data.readUint32LE(), true, 19, 12);

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not a float type");
}

float GFF4Struct::getFloat(Common::SeekableReadStream &data, IFieldType type) const {
	switch (type) {
		case kIFieldTypeFloat32:
			return (float) data.readIEEEFloatLE();

		case kIFieldTypeFloat64:
			return (float) data.readIEEEDoubleLE();

		case kIFieldTypeNDSFixed:
			return (float) readNintendoFixedPoint(data.readUint32LE(), true, 19, 12);

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not a float type");
}

Common::UString GFF4Struct::getString(Common::SeekableReadStream &data, Common::Encoding encoding) const {
	/* When the string is encoded in UTF-8, then length field specifies the length in bytes.
	 * Otherwise, it's the length in characters. */
	const size_t lengthMult = encoding == Common::kEncodingUTF8 ? 1 : Common::getBytesPerCodepoint(encoding);

	const size_t offset = data.pos();

	const uint32 length = data.readUint32LE();
	const size_t size   = length * lengthMult;

	try {
		return readStringFixed(data, encoding, size);
	} catch (...) {
	}

	return Common::UString::format("GFF4: Invalid string encoding (0x%08X)", (uint) offset);
}

Common::UString GFF4Struct::getString(Common::SeekableReadStream &data, Common::Encoding encoding,
                                      uint32 offset) const {

	const uint32 pos = data.seek(offset);

	Common::UString str = getString(data, encoding);

	data.seek(pos);

	return str;
}

Common::UString GFF4Struct::getString(Common::SeekableReadStream &data, const Field &field,
                                      Common::Encoding encoding) const {

	if (field.type == kIFieldTypeString) {
		if (_parent->hasSharedStrings())
			return _parent->getSharedString(data.readUint32LE());

		uint32 offset = data.pos();
		if (!field.isGeneric) {
			offset = data.readUint32LE();
			if (offset == 0xFFFFFFFF)
				return "";

			offset += _parent->getDataOffset();
		}

		return getString(data, encoding, offset);
	}

	if (field.type == kIFieldTypeASCIIString)
		return getString(data, Common::kEncodingASCII, data.pos());

	throw Common::Exception("GFF4: Field is not a string type");
}

// --- Single value readers ---

uint64 GFF4Struct::getUint(uint32 field, uint64 def) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return def;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	return getUint(*data, f->type);
}

int64 GFF4Struct::getSint(uint32 field, int64 def) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return def;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	return getSint(*data, f->type);
}

bool GFF4Struct::getBool(uint32 field, bool def) const {
	return getUint(field, def) != 0;
}

double GFF4Struct::getDouble(uint32 field, double def) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return def;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	return getDouble(*data, f->type);
}

float GFF4Struct::getFloat(uint32 field, float def) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return def;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	return getFloat(*data, f->type);
}

Common::UString GFF4Struct::getString(uint32 field, Common::Encoding encoding,
                                      const Common::UString &def) const {

	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return def;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	return getString(*data, *f, encoding);
}

Common::UString GFF4Struct::getString(uint32 field, const Common::UString &def) const {
	return getString(field, Common::kEncodingUTF16LE, def);
}

bool GFF4Struct::getTalkString(uint32 field, Common::Encoding encoding,
                               uint32 &strRef, Common::UString &str) const {

	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->type != kIFieldTypeTlkString)
		throw Common::Exception("GFF4: Field is not of TalkString type");
	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	strRef = getUint(*data, kIFieldTypeUint32);

	const uint32 offset = getUint(*data, kIFieldTypeUint32);

	str.clear();
	if ((offset != 0xFFFFFFFF) && (offset != 0))
		str = getString(*data, encoding, _parent->getDataOffset() + offset);

	return true;
}

bool GFF4Struct::getTalkString(uint32 field, uint32 &strRef, Common::UString &str) const {
	return getTalkString(field, Common::kEncodingUTF16LE, strRef, str);
}

bool GFF4Struct::getVector3(uint32 field, double &v1, double &v2, double &v3) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 3);

	v1 = getDouble(*data, kIFieldTypeFloat32);
	v2 = getDouble(*data, kIFieldTypeFloat32);
	v3 = getDouble(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector3(uint32 field, float &v1, float &v2, float &v3) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 3);

	v1 = getFloat(*data, kIFieldTypeFloat32);
	v2 = getFloat(*data, kIFieldTypeFloat32);
	v3 = getFloat(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector4(uint32 field, double &v1, double &v2, double &v3, double &v4) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 4);

	v1 = getDouble(*data, kIFieldTypeFloat32);
	v2 = getDouble(*data, kIFieldTypeFloat32);
	v3 = getDouble(*data, kIFieldTypeFloat32);
	v4 = getDouble(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector4(uint32 field, float &v1, float &v2, float &v3, float &v4) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 4);

	v1 = getFloat(*data, kIFieldTypeFloat32);
	v2 = getFloat(*data, kIFieldTypeFloat32);
	v3 = getFloat(*data, kIFieldTypeFloat32);
	v4 = getFloat(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, double (&m)[16]) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16);
	for (uint32 i = 0; i < length; i++)
		m[i] = getDouble(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, float (&m)[16]) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16);
	for (uint32 i = 0; i < length; i++)
		m[i] = getFloat(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector<double> &vectorMatrix) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16);

	vectorMatrix.resize(length);
	for (uint32 i = 0; i < length; i++)
		vectorMatrix[i] = getDouble(*data, kIFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector<float> &vectorMatrix) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16);

	vectorMatrix.resize(length);
	for (uint32 i = 0; i < length; i++)
		vectorMatrix[i] = getFloat(*data, kIFieldTypeFloat32);

	return true;
}

// --- List value readers ---

bool GFF4Struct::getUint(uint32 field, std::vector<uint64> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getUint(*data, f->type);

	return true;
}

bool GFF4Struct::getSint(uint32 field, std::vector<int64> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getSint(*data, f->type);

	return true;
}

bool GFF4Struct::getBool(uint32 field, std::vector<bool> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getUint(*data, f->type) != 0;

	return true;
}

bool GFF4Struct::getDouble(uint32 field, std::vector<double> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getDouble(*data, f->type);

	return true;
}

float GFF4Struct::getFloat(uint32 field, std::vector<float> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getFloat(*data, f->type);

	return true;
}

bool GFF4Struct::getString(uint32 field, Common::Encoding encoding,
                           std::vector<Common::UString> &list) const {

	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data) {
		if (f && !f->isList) {
			list.push_back("");
			return true;
		}

		return false;
	}

	const uint32 count = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++)
		list[i] = getString(*data, *f, encoding);

	return true;
}

bool GFF4Struct::getString(uint32 field, std::vector<Common::UString> &list) const {
	return getString(field, Common::kEncodingUTF16LE, list);
}

bool GFF4Struct::getTalkString(uint32 field, Common::Encoding encoding,
                               std::vector<uint32> &strRefs, std::vector<Common::UString> &strs) const {


	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->type != kIFieldTypeTlkString)
		throw Common::Exception("GFF4: Field is not of TalkString type");

	const uint32 count = getListCount(*data, *f);

	strRefs.resize(count);
	strs.resize(count);

	std::vector<uint32> offsets;
	offsets.resize(count);

	for (uint32 i = 0; i < count; i++) {
		strRefs[i] = getUint(*data, kIFieldTypeUint32);

		const uint32 offset = getUint(*data, kIFieldTypeUint32);
		if ((offset != 0xFFFFFFFF) && (offset != 0))
			strs[i] = getString(*data, encoding, _parent->getDataOffset() + offset);
	}

	return true;
}

bool GFF4Struct::getTalkString(uint32 field,
                               std::vector<uint32> &strRefs, std::vector<Common::UString> &strs) const {

	return getTalkString(field, Common::kEncodingUTF16LE, strRefs, strs);
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector< std::vector<double> > &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 length = getVectorMatrixLength(*f, 16);
	const uint32 count  = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++) {

		list[i].resize(length);
		for (uint32 j = 0; j < length; j++)
			list[i][j] = getDouble(*data, kIFieldTypeFloat32);
	}

	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector< std::vector<float> > &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 length = getVectorMatrixLength(*f, 16);
	const uint32 count  = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++) {

		list[i].resize(length);
		for (uint32 j = 0; j < length; j++)
			list[i][j] = getFloat(*data, kIFieldTypeFloat32);
	}

	return true;
}

// --- Struct reader ---

const GFF4Struct *GFF4Struct::getStruct(uint32 field) const {
	const Field *f = getField(field);
	if (!f)
		return 0;

	if (f->type != kIFieldTypeStruct)
		throw Common::Exception("GFF4: Field is not of struct type");
	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	if (!f->structs.empty())
		return f->structs[0];

	return 0;
}

// --- Generic reader ---

const GFF4Struct *GFF4Struct::getGeneric(uint32 field) const {
	const Field *f = getField(field);
	if (!f)
		return 0;

	if (f->type != kIFieldTypeGeneric)
		throw Common::Exception("GFF4: Field is not of generic type");

	if (!f->structs.empty())
		return f->structs[0];

	return 0;
}

// --- Struct list reader ---

const GFF4List &GFF4Struct::getList(uint32 field) const {
	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("GFF4: No such field");

	if (f->type != kIFieldTypeStruct)
		throw Common::Exception("GFF4: Field is not of struct type");

	return f->structs;
}

// --- Struct data reader ---

Common::SeekableReadStream *GFF4Struct::getData(uint32 field) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return 0;

	const uint32 count = getListCount(*data, *f);
	const uint32 size  = getFieldSize(f->type);

	if ((size == 0) || (count == 0))
		return 0;

	const uint32 dataBegin = data->pos();
	const uint32 dataEnd   = data->pos() + (count * size);

	return new Common::SeekableSubReadStream(data, dataBegin, dataEnd);
}

} // End of namespace Aurora

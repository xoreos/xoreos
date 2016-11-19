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
#include "src/common/matrix4x4.h"

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

	// Only V4.1 has the global string table
	if (version == kVersion41) {
		stringCount  = gff4.readUint32LE();
		stringOffset = gff4.readUint32LE();
	}

	hasSharedStrings = (stringCount > 0) || (stringOffset != 0xFFFFFFFF);

	dataOffset = gff4.readUint32LE();
}


GFF4File::GFF4File(Common::SeekableReadStream *gff4, uint32 type) :
	_stream(gff4), _topLevelStruct(0) {

	assert(_stream);

	load(type);
}

GFF4File::GFF4File(const Common::UString &gff4, FileType fileType, uint32 type) :
	_topLevelStruct(0) {

	_stream.reset(ResMan.getResource(gff4, fileType));
	if (!_stream)
		throw Common::Exception("No such GFF4 \"%s\"", TypeMan.setFileType(gff4, fileType).c_str());

	load(type);
}

GFF4File::~GFF4File() {
	clear();
}

void GFF4File::clear() {
	_stream.reset();

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

	if (_header.structCount == 0)
		throw Common::Exception("GFF4 has no structs");
}

void GFF4File::loadStructs() {
	/* Load the struct templates.
	 *
	 * The struct template defines the structure of a struct, i.e. how
	 * many fields there are and of what type. Each field of a struct type
	 * references one of these templates. When two structs contain the same
	 * structure (but not necessarily the same field *values*), they can
	 * both reference the same struct template.
	 *
	 * So while in a GFF3, each individual struct said how its fields
	 * looked, in a GFF4 this has been sourced out into these templates. */

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

	/* And load the top level struct, which itself recurses into field structs.
	 * The top level struct is always constructed using the first template. */
	_topLevelStruct = new GFF4Struct(*this, _header.dataOffset, _structTemplates[0]);
	_topLevelStruct->_refCount++;
}

void GFF4File::loadStrings() {
	/* Load the global, shared string table.
	 *
	 * If this GFF4 file has such a table (which is only supported in V4.1),
	 * each individual string field in a struct doesn't provide its own data.
	 * Instead, they then reference this shared string table. */

	if (!_header.hasSharedStrings)
		return;

	_sharedStrings.resize(_header.stringCount);

	_stream->seek(_header.stringOffset);
	for (uint32 i = 0; i < _header.stringCount; i++)
		_sharedStrings[i] = Common::readString(*_stream, Common::kEncodingUTF8);
}

// --- Helpers for GFF4Struct ---

void GFF4File::registerStruct(uint64 id, GFF4Struct *strct) {
	/* Each struct, on creation, registers itself to the GFF4 files it
	 * belongs in.
	 *
	 * This is especially necessary for finding reference duplicates:
	 * a struct can be referenced by multiple struct type fields. For
	 * example, let there be structs A, B, C, and D. Structs, A, B and C
	 * can each contain a field "x" of type struct, each linking to
	 * struct D. Moreover, D can even contain field "y" of type struct,
	 * linking back to A, thus creating a loop. */

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
	if (i >= _structTemplates.size())
		throw Common::Exception("GFF4: Struct template out of range (%u >= %u)",
		                        i, (uint) _structTemplates.size());

	return _structTemplates[i];
}

bool GFF4File::hasSharedStrings() const {
	return _header.hasSharedStrings;
}

Common::UString GFF4File::getSharedString(uint32 i) const {
	if (i == 0xFFFFFFFF)
		return "";

	if (i >= _sharedStrings.size())
		throw Common::Exception("GFF4: Shared string index out of range (%u >= %u)",
		                        i, (uint) _sharedStrings.size());

	return _sharedStrings[i];
}


GFF4Struct::Field::Field() : label(0), type(kFieldTypeNone), offset(0xFFFFFFFF),
	isList(false), isReference(false), isGeneric(false), structIndex(0) {

}

GFF4Struct::Field::Field(uint32 l, uint16 t, uint16 f, uint32 o, bool g) :
	label(l), offset(o), isGeneric(g) {

	isList      = (f & 0x8000) != 0;
	isReference = (f & 0x2000) != 0;

	// Map the struct flag to the struct type and index, if necessary
	const bool isStruct = (f & 0x4000) != 0;
	if (isStruct) {
		type        = kFieldTypeStruct;
		structIndex = t;
	} else {
		type        = (FieldType) t;
		structIndex = 0;
	}

	// A string is always read by reference. An extra reference flag is superfluous.
	if (type == kFieldTypeString)
		isReference = false;

	bool supportedConfig = true;

	// We don't know how any of these work
	if (isList && (type == kFieldTypeASCIIString))
		supportedConfig = false;
	if (isList && (type == kFieldTypeTlkString))
		supportedConfig = false;
	if (isList &&  isReference && (type != kFieldTypeStruct) && (type != kFieldTypeGeneric))
		supportedConfig = false;
	if (isList && !isReference && (type == kFieldTypeGeneric))
		supportedConfig = false;

	if (!supportedConfig)
		throw Common::Exception("GFF4: TODO: Field type %d, isList %d, isReference %d",
		                        (int) type, isList, isReference);
}

GFF4Struct::Field::~Field() {
}


GFF4Struct::GFF4Struct(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt) :
	_parent(&parent), _label(tmplt.label), _refCount(0), _fieldCount(0) {

	// Constructor for a real struct, from a template

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

	// Constructor for a generic, converted into a struct

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

uint64 GFF4Struct::getID() const {
	return _id;
}

uint32 GFF4Struct::getRefCount() const {
	return _refCount;
}

uint32 GFF4Struct::getLabel() const {
	return _label;
}

// --- Loader ---

void GFF4Struct::load(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt) {
	/* Loader for a real struct, from a template.
	 *
	 * Go through all the fields in the template and create field
	 * instances within this struct instance. If the field is itself
	 * a struct, recursively create a new struct instance for it. If
	 * the field is a generic, create a struct for it as well. */

	for (size_t i = 0; i < tmplt.fields.size(); i++) {
		const GFF4File::StructTemplate::Field &field = tmplt.fields[i];

		_fieldLabels.push_back(field.label);

		// Calculate the offset for the field data, but guard against NULL pointers
		uint32 fieldOffset = offset + field.offset;
		if ((offset == 0xFFFFFFFF) || (field.offset == 0xFFFFFFFF))
			fieldOffset = 0xFFFFFFFF;

		// Load the field and its struct(s), if any
		Field &f = _fields[field.label] = Field(field.label, field.type, field.flags, fieldOffset);
		if (f.type == kFieldTypeStruct)
			loadStructs(parent, f);
		if (f.type == kFieldTypeGeneric)
			loadGeneric(parent, f);

		if ((f.type == kFieldTypeASCIIString) && parent.hasSharedStrings())
			throw Common::Exception("GFF4: TODO: ASCII string field in a file with shared strings");
	}

	_fieldCount = _fields.size();
}

void GFF4Struct::loadStructs(GFF4File &parent, Field &field) {
	if (field.offset == 0xFFFFFFFF)
		return;

	/* Loader for fields of struct type.
	 *
	 * This field can be
	 * a) a list of structs
	 * b) a reference (pointer) to a struct
	 * c) both
	 *
	 * We figure out how many structs there are (1 if not a list),
	 * where the offset is (dependent on whether it's a reference)
	 * and then we load every single one of them. However, we also
	 * ask the parent GFF4 if we already have loaded the struct in
	 * question (which can happen, because more than one reference
	 * can point to the same struct). If that is the case, we don't
	 * need to load it again. */

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

	// Loader for fields of generic type. We map the generic to a struct.

	GFF4Struct *strct = parent.findStruct(generateID(field.offset));
	if (!strct)
		strct = new GFF4Struct(parent, field);

	strct->_refCount++;

	field.structs.push_back(strct);
}

void GFF4Struct::load(GFF4File &parent, const Field &genericParent) {
	/* Loader for generic, converting it into a struct.
	 *
	 * Go through all the elements of the generic and create fields
	 * for them in this struct instance. If the element itself is a
	 * struct, recursively create a new struct instance for it. */

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

		_fieldLabels.push_back(i);

		// Load the field and its struct(s), if any
		Field &f = _fields[i] = Field(i, fieldType, fieldFlags, fieldOffset, true);
		if (f.type == kFieldTypeStruct)
			loadStructs(parent, f);
		if (f.type == kFieldTypeGeneric)
			throw Common::Exception("GFF4: Found a generic with type generic?");

		if ((f.type == kFieldTypeASCIIString) && parent.hasSharedStrings())
			throw Common::Exception("GFF4: TODO: ASCII string field in a file with shared strings");
	}

	_fieldCount = genericCount;
}

uint64 GFF4Struct::generateID(uint32 offset, const GFF4File::StructTemplate *tmplt) {
	/* Generate a unique ID identifying this struct within the GFF4 file.
	 * The offset is an obvious choice. We also add the template index,
	 * just to make sure.
	 *
	 * (However, if this struct is actually a mapped generic, there is
	 * no template index). */

	return (((uint64) offset) << 32) | (tmplt ? tmplt->index : 0xFFFFFFFF);
}

// --- Field properties ---

size_t GFF4Struct::getFieldCount() const {
	return _fieldCount;
}

bool GFF4Struct::hasField(uint32 field) const {
	return getField(field) != 0;
}

const std::vector<uint32> &GFF4Struct::getFieldLabels() const {
	return _fieldLabels;
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

	return f->type;
}

bool GFF4Struct::getFieldProperties(uint32 field, FieldType &type, uint32 &label, bool &isList) const {
	const Field *f = getField(field);
	if (!f)
		return false;

	type   = f->type;
	label  = f->label;
	isList = f->isList;

	return true;
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
	if (field.type == kFieldTypeStruct)
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

uint32 GFF4Struct::getVectorMatrixLength(const Field &field, uint32 minLength, uint32 maxLength) const {
	uint32 length;
	if       (field.type == kFieldTypeVector3f)
		length =  3;
	else if ((field.type == kFieldTypeVector4f)    ||
	         (field.type == kFieldTypeQuaternionf) ||
	         (field.type == kFieldTypeColor4f))
		length =  4;
	else if  (field.type == kFieldTypeMatrix4x4f)
		length = 16;
	else
		throw Common::Exception("GFF4: Field is not of Vector/Matrix type");

	if (length < minLength)
		throw Common::Exception("GFF4: Vector/Matrix type is too short (%d < %d)", length, minLength);
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

uint32 GFF4Struct::getFieldSize(FieldType type) const {
	switch (type) {
		case kFieldTypeUint8:
		case kFieldTypeSint8:
			return 1;

		case kFieldTypeUint16:
		case kFieldTypeSint16:
			return 2;

		case kFieldTypeUint32:
		case kFieldTypeSint32:
		case kFieldTypeFloat32:
			return 4;

		case kFieldTypeUint64:
		case kFieldTypeSint64:
		case kFieldTypeFloat64:
			return 8;

		case kFieldTypeVector3f:
			return 3 * 4;

		case kFieldTypeVector4f:
		case kFieldTypeQuaternionf:
		case kFieldTypeColor4f:
			return 4 * 4;

		case kFieldTypeMatrix4x4f:
			return 16 * 4;

		case kFieldTypeTlkString:
			return 2 * 4;

		case kFieldTypeString:
			// The raw form is a pointer...
			return 4;

		case kFieldTypeASCIIString:
			// Actually depending on the content...
			return 4;

		default:
			break;
	}

	return 0;
}

// --- Low-level value readers ---

uint64 GFF4Struct::getUint(Common::SeekableReadStream &data, FieldType type) const {
	switch (type) {
		case kFieldTypeUint8:
			return (uint64) data.readByte();

		case kFieldTypeSint8:
			return (uint64) ((int64) data.readSByte());

		case kFieldTypeUint16:
			return (uint64) data.readUint16LE();

		case kFieldTypeSint16:
			return (uint64) ((int64) data.readSint16LE());

		case kFieldTypeUint32:
			return (uint64) data.readUint32LE();

		case kFieldTypeSint32:
			return (uint64) ((int64) data.readSint32LE());

		case kFieldTypeUint64:
			return (uint64) data.readUint64LE();

		case kFieldTypeSint64:
			return (uint64) ((int64) data.readSint64LE());

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not an int type");
}

int64 GFF4Struct::getSint(Common::SeekableReadStream &data, FieldType type) const {
	switch (type) {
		case kFieldTypeUint8:
			return (int64) ((uint64) data.readByte());

		case kFieldTypeSint8:
			return (int64) data.readSByte();

		case kFieldTypeUint16:
			return (int64) ((uint64) data.readUint16LE());

		case kFieldTypeSint16:
			return (int64) data.readSint16LE();

		case kFieldTypeUint32:
			return (int64) ((uint64) data.readUint32LE());

		case kFieldTypeSint32:
			return (int64) data.readSint32LE();

		case kFieldTypeUint64:
			return (int64) ((uint64) data.readUint64LE());

		case kFieldTypeSint64:
			return (int64) data.readSint64LE();

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not an int type");
}

double GFF4Struct::getDouble(Common::SeekableReadStream &data, FieldType type) const {
	switch (type) {
		case kFieldTypeFloat32:
			return (double) data.readIEEEFloatLE();

		case kFieldTypeFloat64:
			return (double) data.readIEEEDoubleLE();

		case kFieldTypeNDSFixed:
			return readNintendoFixedPoint(data.readUint32LE(), true, 19, 12);

		default:
			break;
	}

	throw Common::Exception("GFF4: Field is not a float type");
}

float GFF4Struct::getFloat(Common::SeekableReadStream &data, FieldType type) const {
	switch (type) {
		case kFieldTypeFloat32:
			return (float) data.readIEEEFloatLE();

		case kFieldTypeFloat64:
			return (float) data.readIEEEDoubleLE();

		case kFieldTypeNDSFixed:
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
		return Common::readStringFixed(data, encoding, size);
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

	if (field.type == kFieldTypeString) {
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

	if (field.type == kFieldTypeASCIIString)
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

	if (f->type != kFieldTypeTlkString)
		throw Common::Exception("GFF4: Field is not of TalkString type");
	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	strRef = getUint(*data, kFieldTypeUint32);

	const uint32 offset = getUint(*data, kFieldTypeUint32);

	str.clear();
	if (offset != 0xFFFFFFFF) {
		if (_parent->hasSharedStrings())
			str = _parent->getSharedString(offset);
		else if (offset != 0)
			str = getString(*data, encoding, _parent->getDataOffset() + offset);
	}

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

	getVectorMatrixLength(*f, 3, 3);

	v1 = getDouble(*data, kFieldTypeFloat32);
	v2 = getDouble(*data, kFieldTypeFloat32);
	v3 = getDouble(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector3(uint32 field, float &v1, float &v2, float &v3) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 3, 3);

	v1 = getFloat(*data, kFieldTypeFloat32);
	v2 = getFloat(*data, kFieldTypeFloat32);
	v3 = getFloat(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector4(uint32 field, double &v1, double &v2, double &v3, double &v4) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 4, 4);

	v1 = getDouble(*data, kFieldTypeFloat32);
	v2 = getDouble(*data, kFieldTypeFloat32);
	v3 = getDouble(*data, kFieldTypeFloat32);
	v4 = getDouble(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVector4(uint32 field, float &v1, float &v2, float &v3, float &v4) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	getVectorMatrixLength(*f, 4, 4);

	v1 = getFloat(*data, kFieldTypeFloat32);
	v2 = getFloat(*data, kFieldTypeFloat32);
	v3 = getFloat(*data, kFieldTypeFloat32);
	v4 = getFloat(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, double (&m)[16]) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16, 16);
	for (uint32 i = 0; i < length; i++)
		m[i] = getDouble(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, float (&m)[16]) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 16, 16);
	for (uint32 i = 0; i < length; i++)
		m[i] = getFloat(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, Common::Matrix4x4 &m) const {
	float f[16];
	if (!getMatrix4x4(field, f))
		return false;

	m = f;
	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector<double> &vectorMatrix) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 0, 16);

	vectorMatrix.resize(length);
	for (uint32 i = 0; i < length; i++)
		vectorMatrix[i] = getDouble(*data, kFieldTypeFloat32);

	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector<float> &vectorMatrix) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	if (f->isList)
		throw Common::Exception("GFF4: Tried reading list as singular value");

	const uint32 length = getVectorMatrixLength(*f, 0, 16);

	vectorMatrix.resize(length);
	for (uint32 i = 0; i < length; i++)
		vectorMatrix[i] = getFloat(*data, kFieldTypeFloat32);

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

bool GFF4Struct::getFloat(uint32 field, std::vector<float> &list) const {
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

	if (f->type != kFieldTypeTlkString)
		throw Common::Exception("GFF4: Field is not of TalkString type");

	const uint32 count = getListCount(*data, *f);

	strRefs.resize(count);
	strs.resize(count);

	std::vector<uint32> offsets;
	offsets.resize(count);

	for (uint32 i = 0; i < count; i++) {
		strRefs[i] = getUint(*data, kFieldTypeUint32);

		const uint32 offset = getUint(*data, kFieldTypeUint32);

		if (offset != 0xFFFFFFFF) {
			if (_parent->hasSharedStrings())
				strs[i] = _parent->getSharedString(offset);
			else if (offset != 0)
				strs[i] = getString(*data, encoding, _parent->getDataOffset() + offset);
		}
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

	const uint32 length = getVectorMatrixLength(*f, 0, 16);
	const uint32 count  = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++) {

		list[i].resize(length);
		for (uint32 j = 0; j < length; j++)
			list[i][j] = getDouble(*data, kFieldTypeFloat32);
	}

	return true;
}

bool GFF4Struct::getVectorMatrix(uint32 field, std::vector< std::vector<float> > &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 length = getVectorMatrixLength(*f, 0, 16);
	const uint32 count  = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++) {

		list[i].resize(length);
		for (uint32 j = 0; j < length; j++)
			list[i][j] = getFloat(*data, kFieldTypeFloat32);
	}

	return true;
}

bool GFF4Struct::getMatrix4x4(uint32 field, std::vector<Common::Matrix4x4> &list) const {
	const Field *f;
	Common::SeekableReadStream *data = getField(field, f);
	if (!data)
		return false;

	const uint32 length = getVectorMatrixLength(*f, 0, 16);
	const uint32 count  = getListCount(*data, *f);

	list.resize(count);
	for (uint32 i = 0; i < count; i++) {
		float m[16];

		for (uint32 j = 0; j < length; j++)
			m[j] = getFloat(*data, kFieldTypeFloat32);

		list[i] = m;
	}

	return true;
}

// --- Struct reader ---

const GFF4Struct *GFF4Struct::getStruct(uint32 field) const {
	const Field *f = getField(field);
	if (!f)
		return 0;

	if (f->type != kFieldTypeStruct)
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

	if (f->type != kFieldTypeGeneric)
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

	if (f->type != kFieldTypeStruct)
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

	const size_t dataSize  = count * size;
	const size_t dataBegin = data->pos();
	const size_t dataEnd   = data->pos() + dataSize;

	if ((dataBegin >= data->size()) || ((data->size() - dataBegin) < dataSize))
		throw Common::Exception("Invalid data offset (%u, %u, %u)",
		                        (uint) dataBegin, (uint) dataSize, (uint) data->size());

	return new Common::SeekableSubReadStream(data, dataBegin, dataEnd);
}

} // End of namespace Aurora

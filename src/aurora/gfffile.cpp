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

/** @file aurora/gfffile.cpp
 *  Handling BioWare's GFFs (generic file format).
 */

#include "common/endianness.h"
#include "common/error.h"
#include "common/stream.h"
#include "common/ustring.h"

#include "aurora/gfffile.h"
#include "aurora/error.h"
#include "aurora/util.h"
#include "aurora/locstring.h"
#include "aurora/resman.h"

static const uint32 kVersion32 = MKTAG('V', '3', '.', '2');
static const uint32 kVersion33 = MKTAG('V', '3', '.', '3'); // Found in The Witcher, different language table

namespace Aurora {

GFFFile::Header::Header() {
	clear();
}

void GFFFile::Header::clear() {
	structOffset       = 0;
	structCount        = 0;
	fieldOffset        = 0;
	fieldCount         = 0;
	labelOffset        = 0;
	labelCount         = 0;
	fieldDataOffset    = 0;
	fieldDataCount     = 0;
	fieldIndicesOffset = 0;
	fieldIndicesCount  = 0;
	listIndicesOffset  = 0;
	listIndicesCount   = 0;
}

void GFFFile::Header::read(Common::SeekableReadStream &gff) {
	structOffset       = gff.readUint32LE();
	structCount        = gff.readUint32LE();
	fieldOffset        = gff.readUint32LE();
	fieldCount         = gff.readUint32LE();
	labelOffset        = gff.readUint32LE();
	labelCount         = gff.readUint32LE();
	fieldDataOffset    = gff.readUint32LE();
	fieldDataCount     = gff.readUint32LE();
	fieldIndicesOffset = gff.readUint32LE();
	fieldIndicesCount  = gff.readUint32LE();
	listIndicesOffset  = gff.readUint32LE();
	listIndicesCount   = gff.readUint32LE();
}


GFFFile::GFFFile(Common::SeekableReadStream *gff, uint32 id) : _stream(gff) {
	load(id);
}

GFFFile::GFFFile(const Common::UString &gff, FileType type, uint32 id) : _stream(0) {
	_stream = ResMan.getResource(gff, type);
	if (!_stream)
		throw Common::Exception("No such GFF \"%s\"", TypeMan.setFileType(gff, type).c_str());

	load(id);
}

GFFFile::~GFFFile() {
	delete _stream;

	for (StructArray::iterator strct = _structs.begin(); strct != _structs.end(); ++strct)
		delete *strct;
}

void GFFFile::load(uint32 id) {
	readHeader(*_stream);

	if (_id != id)
		throw Common::Exception("GFF has invalid ID (want 0x%08X, got 0x%08X)", id, _id);
	if ((_version != kVersion32) && (_version != kVersion33))
		throw Common::Exception("Unsupported GFF file version %08X", _version);

	_header.read(*_stream);

	try {

		readStructs();
		readLists();

		if (_stream->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading GFF file");
		throw;
	}

}

const GFFStruct &GFFFile::getTopLevel() const {
	return getStruct(0);
}

const GFFStruct &GFFFile::getStruct(uint32 i) const {
	assert(i < _structs.size());

	return *_structs[i];
}

const GFFList &GFFFile::getList(uint32 i, uint32 &size) const {
	assert(i < _listOffsetToIndex.size());

	i = _listOffsetToIndex[i];

	assert(i < _lists.size());

	size = _listSizes[i];

	return _lists[i];
}

void GFFFile::readStructs() {
	_structs.reserve(_header.structCount);
	for (uint32 i = 0; i < _header.structCount; i++)
		_structs.push_back(new GFFStruct(*this, *_stream));
}

void GFFFile::readLists() {
	if (!_stream->seek(_header.listIndicesOffset))
		throw Common::Exception(Common::kSeekError);

	// Read list array
	std::vector<uint32> rawLists;
	rawLists.resize(_header.listIndicesCount / 4);
	for (std::vector<uint32>::iterator it = rawLists.begin(); it != rawLists.end(); ++it)
		*it = _stream->readUint32LE();

	// Counting the actual amount of lists
	uint32 listCount = 0;
	for (uint32 i = 0; i < rawLists.size(); i++) {
		uint32 n = rawLists[i];

		if ((i + n) > rawLists.size())
			throw Common::Exception("List indices broken");

		i += n;
		listCount++;
	}

	// Converting the raw list array into real, useable lists
	for (std::vector<uint32>::iterator it = rawLists.begin(); it != rawLists.end(); ) {
		_listOffsetToIndex.push_back(_lists.size());

		_lists.push_back(GFFList());
		_listSizes.push_back(0);

		GFFList &list = _lists.back();
		uint32  &size = _listSizes.back();

		uint32 n = *it++;
		for (uint32 j = 0; j < n; j++, ++it) {
			assert(it != rawLists.end());

			list.push_back(_structs[*it]);
			size++;
			_listOffsetToIndex.push_back(0xFFFFFFFF);
		}
	}

}

Common::SeekableReadStream &GFFFile::getStream() const {
	return *_stream;
}

Common::SeekableReadStream &GFFFile::getFieldData() const {
	_stream->seek(_header.fieldDataOffset);

	return *_stream;
}


GFFStruct::Field::Field() : type(kFieldTypeNone), data(0), extended(false) {
}

GFFStruct::Field::Field(FieldType t, uint32 d) : type(t), data(d) {
	// These field types need extended field data
	extended = (type == kFieldTypeUint64     ) ||
	           (type == kFieldTypeSint64     ) ||
	           (type == kFieldTypeDouble     ) ||
	           (type == kFieldTypeExoString  ) ||
	           (type == kFieldTypeResRef     ) ||
	           (type == kFieldTypeLocString  ) ||
	           (type == kFieldTypeVoid       ) ||
	           (type == kFieldTypeOrientation) ||
	           (type == kFieldTypeVector     );
}


GFFStruct::GFFStruct(const GFFFile &parent, Common::SeekableReadStream &gff) :
	_parent(&parent) {

	_id         = gff.readUint32LE();
	_fieldIndex = gff.readUint32LE();
	_fieldCount = gff.readUint32LE();
}

GFFStruct::~GFFStruct() {
}

void GFFStruct::load() const {
	if (!_fields.empty())
		return;

	Common::SeekableReadStream &gff = _parent->getStream();

	// Read the field(s)
	if      (_fieldCount == 1)
		readField (gff, _fieldIndex);
	else if (_fieldCount > 1)
		readFields(gff, _fieldIndex, _fieldCount);
}

void GFFStruct::readField(Common::SeekableReadStream &gff, uint32 index) const {
	// Sanity check
	if (index > _parent->_header.fieldCount)
		throw Common::Exception("Field index out of range (%d/%d)",
				index, _parent->_header.fieldCount);

	// Seek
	if (!gff.seek(_parent->_header.fieldOffset + index * 12))
		throw Common::Exception(Common::kSeekError);

	// Read the field data
	uint32 type  = gff.readUint32LE();
	uint32 label = gff.readUint32LE();
	uint32 data  = gff.readUint32LE();

	// And add it to the map
	_fields[readLabel(gff, label)] = Field((FieldType) type, data);
}

void GFFStruct::readFields(Common::SeekableReadStream &gff,
                           uint32 index, uint32 count) const {
	// Sanity check
	if (index > _parent->_header.fieldIndicesCount)
		throw Common::Exception("Field indices index out of range (%d/%d)",
		                        index , _parent->_header.fieldIndicesCount);

	// Seek
	if (!gff.seek(_parent->_header.fieldIndicesOffset + index))
		throw Common::Exception(Common::kSeekError);

	// Read the field indices
	std::vector<uint32> indices;
	readIndices(gff, indices, count);

	// Read the fields
	for (std::vector<uint32>::const_iterator i = indices.begin(); i != indices.end(); ++i)
		readField(gff, *i);
}

void GFFStruct::readIndices(Common::SeekableReadStream &gff,
                            std::vector<uint32> &indices, uint32 count) const {
	indices.reserve(count);
	while (count-- > 0)
		indices.push_back(gff.readUint32LE());
}

Common::UString GFFStruct::readLabel(Common::SeekableReadStream &gff, uint32 index) const {
	gff.seek(_parent->_header.labelOffset + index * 16);

	Common::UString label;
	label.readFixedASCII(gff, 16);

	return label;
}

Common::SeekableReadStream &GFFStruct::getData(const Field &field) const {
	assert(field.extended);

	Common::SeekableReadStream &data = _parent->getFieldData();

	data.seek(field.data, SEEK_CUR);

	return data;
}

const GFFStruct::Field *GFFStruct::getField(const Common::UString &name) const {
	FieldMap::const_iterator field = _fields.find(name);
	if (field == _fields.end())
		return 0;

	return &field->second;
}

uint GFFStruct::getFieldCount() const {
	return _fields.size();
}

bool GFFStruct::hasField(const Common::UString &field) const {
	load();

	return getField(field) != 0;
}

char GFFStruct::getChar(const Common::UString &field, char def) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return def;
	if (f->type != kFieldTypeChar)
		throw Common::Exception("Field is not a char type");

	return (char) f->data;
}

uint64 GFFStruct::getUint(const Common::UString &field, uint64 def) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return def;

	// Int types
	if (f->type == kFieldTypeByte)
		return (uint64) ((uint8 ) f->data);
	if (f->type == kFieldTypeUint16)
		return (uint64) ((uint16) f->data);
	if (f->type == kFieldTypeUint32)
		return (uint64) ((uint32) f->data);
	if (f->type == kFieldTypeChar)
		return (uint64) ((int64) ((int8 ) ((uint8 ) f->data)));
	if (f->type == kFieldTypeSint16)
		return (uint64) ((int64) ((int16) ((uint16) f->data)));
	if (f->type == kFieldTypeSint32)
		return (uint64) ((int64) ((int32) ((uint32) f->data)));
	if (f->type == kFieldTypeUint64)
		return (uint64) getData(*f).readUint64LE();
	if (f->type == kFieldTypeSint64)
		return ( int64) getData(*f).readUint64LE();

	throw Common::Exception("Field is not an int type");
}

int64 GFFStruct::getSint(const Common::UString &field, int64 def) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return def;

	// Int types
	if (f->type == kFieldTypeByte)
		return (int64) ((int8 ) ((uint8 ) f->data));
	if (f->type == kFieldTypeUint16)
		return (int64) ((int16) ((uint16) f->data));
	if (f->type == kFieldTypeUint32)
		return (int64) ((int32) ((uint32) f->data));
	if (f->type == kFieldTypeChar)
		return (int64) ((int8 ) ((uint8 ) f->data));
	if (f->type == kFieldTypeSint16)
		return (int64) ((int16) ((uint16) f->data));
	if (f->type == kFieldTypeSint32)
		return (int64) ((int32) ((uint32) f->data));
	if (f->type == kFieldTypeUint64)
		return (int64) getData(*f).readUint64LE();
	if (f->type == kFieldTypeSint64)
		return (int64) getData(*f).readUint64LE();

	throw Common::Exception("Field is not an int type");
}

bool GFFStruct::getBool(const Common::UString &field, bool def) const {
	load();

	return getUint(field, def) != 0;
}

double GFFStruct::getDouble(const Common::UString &field, double def) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return def;

	if (f->type == kFieldTypeFloat)
		return convertIEEEFloat(f->data);
	if (f->type == kFieldTypeDouble)
		return getData(*f).readIEEEDoubleLE();

	throw Common::Exception("Field is not a double type");
}

Common::UString GFFStruct::getString(const Common::UString &field,
                                        const Common::UString &def) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return def;

	if (f->type == kFieldTypeExoString) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 length = data.readUint32LE();

		Common::UString str;
		str.readFixedASCII(data, length);
		return str;
	}

	if (f->type == kFieldTypeResRef) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 length = data.readByte();

		Common::UString str;
		str.readFixedASCII(data, length);
		return str;
	}

	if ((f->type == kFieldTypeByte  ) ||
	    (f->type == kFieldTypeUint16) ||
	    (f->type == kFieldTypeUint32) ||
	    (f->type == kFieldTypeUint64)) {

		return Common::UString::sprintf("%lu", getUint(field));
	}

	if ((f->type == kFieldTypeChar  ) ||
	    (f->type == kFieldTypeSint16) ||
	    (f->type == kFieldTypeSint32) ||
	    (f->type == kFieldTypeSint64)) {

		return Common::UString::sprintf("%ld", getSint(field));
	}

	if ((f->type == kFieldTypeFloat) ||
	    (f->type == kFieldTypeDouble)) {

		return Common::UString::sprintf("%lf", getDouble(field));
	}

	if (f->type == kFieldTypeVector) {
		float x, y, z;

		getVector(field, x, y, z);
		return Common::UString::sprintf("%f/%f/%f", x, y, z);
	}

	if (f->type == kFieldTypeOrientation) {
		float a, b, c, d;

		getOrientation(field, a, b, c, d);
		return Common::UString::sprintf("%f/%f/%f/%f", a, b, c, d);
	}

	throw Common::Exception("Field is not a string(able) type");
}

void GFFStruct::getLocString(const Common::UString &field, LocString &str) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeLocString)
		throw Common::Exception("Field is not of a localized string type");

	Common::SeekableReadStream &data = getData(*f);

	uint32 size = data.readUint32LE();

	Common::SeekableSubReadStream gff(&data, data.pos(), data.pos() + size);

	str.readLocString(gff);
}

Common::SeekableReadStream *GFFStruct::getData(const Common::UString &field) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return 0;
	if (f->type != kFieldTypeVoid)
		throw Common::Exception("Field is not a data type");

	Common::SeekableReadStream &data = getData(*f);

	uint32 size = data.readUint32LE();

	return data.readStream(size);
}

void GFFStruct::getVector(const Common::UString &field,
                          float &x, float &y, float &z) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeVector)
		throw Common::Exception("Field is not a vector type");

	Common::SeekableReadStream &data = getData(*f);

	x = data.readIEEEFloatLE();
	y = data.readIEEEFloatLE();
	z = data.readIEEEFloatLE();
}

void GFFStruct::getOrientation(const Common::UString &field,
                               float &a, float &b, float &c, float &d) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeOrientation)
		throw Common::Exception("Field is not an orientation type");

	Common::SeekableReadStream &data = getData(*f);

	a = data.readIEEEFloatLE();
	b = data.readIEEEFloatLE();
	c = data.readIEEEFloatLE();
	d = data.readIEEEFloatLE();
}

void GFFStruct::getVector(const Common::UString &field,
                          double &x, double &y, double &z) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeVector)
		throw Common::Exception("Field is not a vector type");

	Common::SeekableReadStream &data = getData(*f);

	x = data.readIEEEFloatLE();
	y = data.readIEEEFloatLE();
	z = data.readIEEEFloatLE();
}

void GFFStruct::getOrientation(const Common::UString &field,
                               double &a, double &b, double &c, double &d) const {
	load();

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeOrientation)
		throw Common::Exception("Field is not an orientation type");

	Common::SeekableReadStream &data = getData(*f);

	a = data.readIEEEFloatLE();
	b = data.readIEEEFloatLE();
	c = data.readIEEEFloatLE();
	d = data.readIEEEFloatLE();
}

const GFFStruct &GFFStruct::getStruct(const Common::UString &field) const {
	load();

	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("No such field");
	if (f->type != kFieldTypeStruct)
		throw Common::Exception("Field is not a struct type");

	// Direct index into the struct array
	return _parent->getStruct(f->data);
}

const GFFList &GFFStruct::getList(const Common::UString &field, uint32 &size) const {
	load();

	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("No such field");
	if (f->type != kFieldTypeList)
		throw Common::Exception("Field is not a list type");

	// Byte offset into the list area, all 32bit values.
	return _parent->getList(f->data / 4, size);
}

const GFFList &GFFStruct::getList(const Common::UString &field) const {
	uint32 size;

	return getList(field, size);
}

} // End of namespace Aurora

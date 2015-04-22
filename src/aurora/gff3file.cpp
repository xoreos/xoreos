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
 *  Handling version V3.2/V3.3 of BioWare's GFFs (generic file format).
 */

#include "src/common/endianness.h"
#include "src/common/error.h"
#include "src/common/stream.h"
#include "src/common/encoding.h"
#include "src/common/ustring.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/util.h"
#include "src/aurora/locstring.h"
#include "src/aurora/resman.h"

static const uint32 kVersion32 = MKTAG('V', '3', '.', '2');
static const uint32 kVersion33 = MKTAG('V', '3', '.', '3'); // Found in The Witcher, different language table

namespace Aurora {

GFF3File::Header::Header() {
	clear();
}

void GFF3File::Header::clear() {
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

void GFF3File::Header::read(Common::SeekableReadStream &gff3) {
	structOffset       = gff3.readUint32LE();
	structCount        = gff3.readUint32LE();
	fieldOffset        = gff3.readUint32LE();
	fieldCount         = gff3.readUint32LE();
	labelOffset        = gff3.readUint32LE();
	labelCount         = gff3.readUint32LE();
	fieldDataOffset    = gff3.readUint32LE();
	fieldDataCount     = gff3.readUint32LE();
	fieldIndicesOffset = gff3.readUint32LE();
	fieldIndicesCount  = gff3.readUint32LE();
	listIndicesOffset  = gff3.readUint32LE();
	listIndicesCount   = gff3.readUint32LE();
}


GFF3File::GFF3File(Common::SeekableReadStream *gff3, uint32 id) : _stream(gff3) {
	load(id);
}

GFF3File::GFF3File(const Common::UString &gff3, FileType type, uint32 id) : _stream(0) {
	_stream = ResMan.getResource(gff3, type);
	if (!_stream)
		throw Common::Exception("No such GFF \"%s\"", TypeMan.setFileType(gff3, type).c_str());

	load(id);
}

GFF3File::~GFF3File() {
	clear();
}

void GFF3File::clear() {
	delete _stream;
	_stream = 0;

	for (StructArray::iterator strct = _structs.begin(); strct != _structs.end(); ++strct)
		delete *strct;

	_structs.clear();
}

void GFF3File::load(uint32 id) {
	readHeader(*_stream);

	try {

		if (_id != id)
			throw Common::Exception("GFF has invalid ID (want 0x%08X, got 0x%08X)", id, _id);
		if ((_version != kVersion32) && (_version != kVersion33))
			throw Common::Exception("Unsupported GFF file version %08X", _version);

	} catch (...) {
		clear();
		throw;
	}

	_header.read(*_stream);

	try {

		readStructs();
		readLists();

		if (_stream->err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading GFF file");
		throw;
	}

}

const GFF3Struct &GFF3File::getTopLevel() const {
	return getStruct(0);
}

const GFF3Struct &GFF3File::getStruct(uint32 i) const {
	assert(i < _structs.size());

	return *_structs[i];
}

const GFF3List &GFF3File::getList(uint32 i) const {
	assert(i < _listOffsetToIndex.size());

	i = _listOffsetToIndex[i];

	assert(i < _lists.size());

	return _lists[i];
}

void GFF3File::readStructs() {
	static const uint32 kStructSize = 12;

	_structs.reserve(_header.structCount);
	for (uint32 i = 0; i < _header.structCount; i++)
		_structs.push_back(new GFF3Struct(*this, _header.structOffset + i * kStructSize));
}

void GFF3File::readLists() {
	_stream->seek(_header.listIndicesOffset);

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

	_lists.resize(listCount);
	_listOffsetToIndex.resize(rawLists.size(), 0xFFFFFFFF);

	// Converting the raw list array into real, useable lists
	uint32 listIndex = 0;
	for (uint32 i = 0; i < rawLists.size(); listIndex++) {
		_listOffsetToIndex[i] = listIndex;

		uint32 n = rawLists[i++];
		assert((i + n) <= rawLists.size());

		_lists[listIndex].resize(n);
		for (uint32 j = 0; j < n; j++, i++)
			_lists[listIndex][j] = _structs[rawLists[i]];
	}
}

Common::SeekableReadStream &GFF3File::getStream() const {
	return *_stream;
}

Common::SeekableReadStream &GFF3File::getFieldData() const {
	_stream->seek(_header.fieldDataOffset);

	return *_stream;
}


GFF3Struct::Field::Field() : type(kFieldTypeNone), data(0), extended(false) {
}

GFF3Struct::Field::Field(FieldType t, uint32 d) : type(t), data(d) {
	// These field types need extended field data
	extended = (type == kFieldTypeUint64     ) ||
	           (type == kFieldTypeSint64     ) ||
	           (type == kFieldTypeDouble     ) ||
	           (type == kFieldTypeExoString  ) ||
	           (type == kFieldTypeResRef     ) ||
	           (type == kFieldTypeLocString  ) ||
	           (type == kFieldTypeVoid       ) ||
	           (type == kFieldTypeOrientation) ||
	           (type == kFieldTypeVector     ) ||
	           (type == kFieldTypeStrRef     );
}


GFF3Struct::GFF3Struct(const GFF3File &parent, uint32 offset) : _parent(&parent) {
	load(offset);
}

GFF3Struct::~GFF3Struct() {
}

void GFF3Struct::load(uint32 offset) {
	Common::SeekableReadStream &gff3 = _parent->getStream();
	gff3.seek(offset);

	_id         = gff3.readUint32LE();
	_fieldIndex = gff3.readUint32LE();
	_fieldCount = gff3.readUint32LE();

	// Read the field(s)
	if      (_fieldCount == 1)
		readField (gff3, _fieldIndex);
	else if (_fieldCount > 1)
		readFields(gff3, _fieldIndex, _fieldCount);
}

void GFF3Struct::readField(Common::SeekableReadStream &gff3, uint32 index) {
	// Sanity check
	if (index > _parent->_header.fieldCount)
		throw Common::Exception("Field index out of range (%d/%d)",
				index, _parent->_header.fieldCount);

	// Seek
	gff3.seek(_parent->_header.fieldOffset + index * 12);

	// Read the field data
	uint32 type  = gff3.readUint32LE();
	uint32 label = gff3.readUint32LE();
	uint32 data  = gff3.readUint32LE();

	// And add it to the map
	_fields[readLabel(gff3, label)] = Field((FieldType) type, data);
}

void GFF3Struct::readFields(Common::SeekableReadStream &gff3,
                            uint32 index, uint32 count) {
	// Sanity check
	if (index > _parent->_header.fieldIndicesCount)
		throw Common::Exception("Field indices index out of range (%d/%d)",
		                        index , _parent->_header.fieldIndicesCount);

	// Seek
	gff3.seek(_parent->_header.fieldIndicesOffset + index);

	// Read the field indices
	std::vector<uint32> indices;
	readIndices(gff3, indices, count);

	// Read the fields
	for (std::vector<uint32>::const_iterator i = indices.begin(); i != indices.end(); ++i)
		readField(gff3, *i);
}

void GFF3Struct::readIndices(Common::SeekableReadStream &gff3,
                             std::vector<uint32> &indices, uint32 count) const {
	indices.reserve(count);
	while (count-- > 0)
		indices.push_back(gff3.readUint32LE());
}

Common::UString GFF3Struct::readLabel(Common::SeekableReadStream &gff3, uint32 index) const {
	gff3.seek(_parent->_header.labelOffset + index * 16);

	return Common::readStringFixed(gff3, Common::kEncodingASCII, 16);
}

Common::SeekableReadStream &GFF3Struct::getData(const Field &field) const {
	assert(field.extended);

	Common::SeekableReadStream &data = _parent->getFieldData();

	data.seek(field.data, SEEK_CUR);

	return data;
}

const GFF3Struct::Field *GFF3Struct::getField(const Common::UString &name) const {
	FieldMap::const_iterator field = _fields.find(name);
	if (field == _fields.end())
		return 0;

	return &field->second;
}

uint GFF3Struct::getFieldCount() const {
	return _fields.size();
}

bool GFF3Struct::hasField(const Common::UString &field) const {
	return getField(field) != 0;
}

char GFF3Struct::getChar(const Common::UString &field, char def) const {
	const Field *f = getField(field);
	if (!f)
		return def;
	if (f->type != kFieldTypeChar)
		throw Common::Exception("Field is not a char type");

	return (char) f->data;
}

uint64 GFF3Struct::getUint(const Common::UString &field, uint64 def) const {
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
	if (f->type == kFieldTypeStrRef) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 size = data.readUint32LE();
		if (size != 4)
			Common::Exception("StrRef field with invalid size (%d)", size);

		return (uint64) data.readUint32LE();
	}

	throw Common::Exception("Field is not an int type");
}

int64 GFF3Struct::getSint(const Common::UString &field, int64 def) const {
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
	if (f->type == kFieldTypeStrRef) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 size = data.readUint32LE();
		if (size != 4)
			Common::Exception("StrRef field with invalid size (%d)", size);

		return (int64) ((uint64) data.readUint32LE());
	}

	throw Common::Exception("Field is not an int type");
}

bool GFF3Struct::getBool(const Common::UString &field, bool def) const {
	return getUint(field, def) != 0;
}

double GFF3Struct::getDouble(const Common::UString &field, double def) const {
	const Field *f = getField(field);
	if (!f)
		return def;

	if (f->type == kFieldTypeFloat)
		return convertIEEEFloat(f->data);
	if (f->type == kFieldTypeDouble)
		return getData(*f).readIEEEDoubleLE();

	throw Common::Exception("Field is not a double type");
}

Common::UString GFF3Struct::getString(const Common::UString &field,
                                      const Common::UString &def) const {

	const Field *f = getField(field);
	if (!f)
		return def;

	if (f->type == kFieldTypeExoString) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 length = data.readUint32LE();

		return Common::readStringFixed(data, Common::kEncodingASCII, length);
	}

	if (f->type == kFieldTypeResRef) {
		Common::SeekableReadStream &data = getData(*f);

		uint32 length = data.readByte();

		return Common::readStringFixed(data, Common::kEncodingASCII, length);
	}

	if ((f->type == kFieldTypeByte  ) ||
	    (f->type == kFieldTypeUint16) ||
	    (f->type == kFieldTypeUint32) ||
	    (f->type == kFieldTypeUint64) ||
	    (f->type == kFieldTypeStrRef)) {

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

void GFF3Struct::getLocString(const Common::UString &field, LocString &str) const {
	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeLocString)
		throw Common::Exception("Field is not of a localized string type");

	Common::SeekableReadStream &data = getData(*f);

	uint32 size = data.readUint32LE();

	Common::SeekableSubReadStream gff3(&data, data.pos(), data.pos() + size);

	str.readLocString(gff3);
}

Common::SeekableReadStream *GFF3Struct::getData(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		return 0;
	if (f->type != kFieldTypeVoid)
		throw Common::Exception("Field is not a data type");

	Common::SeekableReadStream &data = getData(*f);

	uint32 size = data.readUint32LE();

	return data.readStream(size);
}

void GFF3Struct::getVector(const Common::UString &field,
                           float &x, float &y, float &z) const {

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

void GFF3Struct::getOrientation(const Common::UString &field,
                                float &a, float &b, float &c, float &d) const {

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

void GFF3Struct::getVector(const Common::UString &field,
                           double &x, double &y, double &z) const {

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

void GFF3Struct::getOrientation(const Common::UString &field,
                                double &a, double &b, double &c, double &d) const {

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

const GFF3Struct &GFF3Struct::getStruct(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("No such field");
	if (f->type != kFieldTypeStruct)
		throw Common::Exception("Field is not a struct type");

	// Direct index into the struct array
	return _parent->getStruct(f->data);
}

const GFF3List &GFF3Struct::getList(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("No such field");
	if (f->type != kFieldTypeList)
		throw Common::Exception("Field is not a list type");

	// Byte offset into the list area, all 32bit values.
	return _parent->getList(f->data / 4);
}

} // End of namespace Aurora

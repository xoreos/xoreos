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

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/memreadstream.h"
#include "src/common/encoding.h"
#include "src/common/ustring.h"
#include "src/common/strutil.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/util.h"
#include "src/aurora/locstring.h"
#include "src/aurora/resman.h"

static const uint32 kVersion32 = MKTAG('V', '3', '.', '2');
static const uint32 kVersion33 = MKTAG('V', '3', '.', '3'); // Found in The Witcher, different language table

namespace Aurora {

GFF3File::Header::Header() {
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
		throw Common::Exception("No such GFF3 \"%s\"", TypeMan.setFileType(gff3, type).c_str());

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

uint32 GFF3File::getType() const {
	return _id;
}

const GFF3Struct &GFF3File::getTopLevel() const {
	return getStruct(0);
}

// --- Loader ---

void GFF3File::load(uint32 id) {
	try {

		loadHeader(id);
		loadStructs();
		loadLists();

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading GFF3 file");
		throw;
	}
}

void GFF3File::loadHeader(uint32 id) {
	readHeader(*_stream);

	if ((id != 0xFFFFFFFF) && (_id != id))
		throw Common::Exception("GFF3 has invalid ID (want %s, got %s)",
				Common::debugTag(id).c_str(), Common::debugTag(_id).c_str());

	if ((_version != kVersion32) && (_version != kVersion33))
		throw Common::Exception("Unsupported GFF3 file version %s", Common::debugTag(_version).c_str());

	_header.read(*_stream);
}

void GFF3File::loadStructs() {
	static const uint32 kStructSize = 12;

	_structs.reserve(_header.structCount);
	for (uint32 i = 0; i < _header.structCount; i++)
		_structs.push_back(new GFF3Struct(*this, _header.structOffset + i * kStructSize));
}

void GFF3File::loadLists() {
	_stream->seek(_header.listIndicesOffset);

	// Read list array
	std::vector<uint32> rawLists;
	rawLists.resize(_header.listIndicesCount / 4);
	for (std::vector<uint32>::iterator it = rawLists.begin(); it != rawLists.end(); ++it)
		*it = _stream->readUint32LE();

	// Counting the actual amount of lists
	uint32 listCount = 0;
	for (size_t i = 0; i < rawLists.size(); i++) {
		uint32 n = rawLists[i];

		if ((i + n) > rawLists.size())
			throw Common::Exception("GFF3: List indices broken");

		i += n;
		listCount++;
	}

	_lists.resize(listCount);
	_listOffsetToIndex.resize(rawLists.size(), 0xFFFFFFFF);

	// Converting the raw list array into real, useable lists
	uint32 listIndex = 0;
	for (size_t i = 0; i < rawLists.size(); listIndex++) {
		_listOffsetToIndex[i] = listIndex;

		const uint32 n = rawLists[i++];
		assert((i + n) <= rawLists.size());

		_lists[listIndex].resize(n);
		for (uint32 j = 0; j < n; j++, i++)
			_lists[listIndex][j] = _structs[rawLists[i]];
	}
}

// --- Helpers for GFF3Struct ---

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

Common::SeekableReadStream &GFF3File::getStream(uint32 offset) const {
	_stream->seek(offset);

	return *_stream;
}

Common::SeekableReadStream &GFF3File::getFieldData() const {
	return getStream(_header.fieldDataOffset);
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

// --- Loader ---

void GFF3Struct::load(uint32 offset) {
	Common::SeekableReadStream &data = _parent->getStream(offset);

	_id         = data.readUint32LE();
	_fieldIndex = data.readUint32LE();
	_fieldCount = data.readUint32LE();

	// Read the field(s)
	if      (_fieldCount == 1)
		readField (data, _fieldIndex);
	else if (_fieldCount > 1)
		readFields(data, _fieldIndex, _fieldCount);
}

void GFF3Struct::readField(Common::SeekableReadStream &data, uint32 index) {
	// Sanity check
	if (index > _parent->_header.fieldCount)
		throw Common::Exception("GFF3: Field index out of range (%d/%d)",
				index, _parent->_header.fieldCount);

	// Seek
	data.seek(_parent->_header.fieldOffset + index * 12);

	// Read the field data
	const uint32 fieldType  = data.readUint32LE();
	const uint32 fieldLabel = data.readUint32LE();
	const uint32 fieldData  = data.readUint32LE();

	// And add it to the map
	_fields[readLabel(data, fieldLabel)] = Field((FieldType) fieldType, fieldData);
}

void GFF3Struct::readFields(Common::SeekableReadStream &data, uint32 index, uint32 count) {
	// Sanity check
	if (index > _parent->_header.fieldIndicesCount)
		throw Common::Exception("GFF3: Field indices index out of range (%d/%d)",
		                        index , _parent->_header.fieldIndicesCount);

	// Seek
	data.seek(_parent->_header.fieldIndicesOffset + index);

	// Read the field indices
	std::vector<uint32> indices;
	readIndices(data, indices, count);

	// Read the fields
	for (std::vector<uint32>::const_iterator i = indices.begin(); i != indices.end(); ++i)
		readField(data, *i);
}

void GFF3Struct::readIndices(Common::SeekableReadStream &data,
                             std::vector<uint32> &indices, uint32 count) const {
	indices.reserve(count);
	while (count-- > 0)
		indices.push_back(data.readUint32LE());
}

Common::UString GFF3Struct::readLabel(Common::SeekableReadStream &data, uint32 index) const {
	data.seek(_parent->_header.labelOffset + index * 16);

	return Common::readStringFixed(data, Common::kEncodingASCII, 16);
}

Common::SeekableReadStream &GFF3Struct::getData(const Field &field) const {
	assert(field.extended);

	Common::SeekableReadStream &data = _parent->getFieldData();
	data.skip(field.data);

	return data;
}

// --- Field properties ---

size_t GFF3Struct::getFieldCount() const {
	return _fields.size();
}

bool GFF3Struct::hasField(const Common::UString &field) const {
	return getField(field) != 0;
}

uint32 GFF3Struct::getID() const {
	return _id;
}

// --- Field value reader helpers ---

const GFF3Struct::Field *GFF3Struct::getField(const Common::UString &name) const {
	FieldMap::const_iterator field = _fields.find(name);
	if (field == _fields.end())
		return 0;

	return &field->second;
}

char GFF3Struct::getChar(const Common::UString &field, char def) const {
	const Field *f = getField(field);
	if (!f)
		return def;
	if (f->type != kFieldTypeChar)
		throw Common::Exception("GFF3: Field is not a char type");

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

		const uint32 size = data.readUint32LE();
		if (size != 4)
			Common::Exception("StrRef field with invalid size (%d)", size);

		return (uint64) data.readUint32LE();
	}

	throw Common::Exception("GFF3: Field is not an int type");
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

		const uint32 size = data.readUint32LE();
		if (size != 4)
			Common::Exception("GFF3: StrRef field with invalid size (%d)", size);

		return (int64) ((uint64) data.readUint32LE());
	}

	throw Common::Exception("GFF3: Field is not an int type");
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

	throw Common::Exception("GFF3: Field is not a double type");
}

Common::UString GFF3Struct::getString(const Common::UString &field,
                                      const Common::UString &def) const {

	const Field *f = getField(field);
	if (!f)
		return def;

	if (f->type == kFieldTypeExoString) {
		Common::SeekableReadStream &data = getData(*f);

		const uint32 length = data.readUint32LE();
		return Common::readStringFixed(data, Common::kEncodingASCII, length);
	}

	if (f->type == kFieldTypeResRef) {
		Common::SeekableReadStream &data = getData(*f);

		const uint32 length = data.readByte();
		return Common::readStringFixed(data, Common::kEncodingASCII, length);
	}

	if ((f->type == kFieldTypeByte  ) ||
	    (f->type == kFieldTypeUint16) ||
	    (f->type == kFieldTypeUint32) ||
	    (f->type == kFieldTypeUint64) ||
	    (f->type == kFieldTypeStrRef)) {

		return Common::composeString(getUint(field));
	}

	if ((f->type == kFieldTypeChar  ) ||
	    (f->type == kFieldTypeSint16) ||
	    (f->type == kFieldTypeSint32) ||
	    (f->type == kFieldTypeSint64)) {

		return Common::composeString(getSint(field));
	}

	if ((f->type == kFieldTypeFloat) ||
	    (f->type == kFieldTypeDouble)) {

		return Common::composeString(getDouble(field));
	}

	if (f->type == kFieldTypeVector) {
		float x = 0.0, y = 0.0, z = 0.0;

		getVector(field, x, y, z);
		return Common::composeString(x) + "/" +
		       Common::composeString(y) + "/" +
		       Common::composeString(z);
	}

	if (f->type == kFieldTypeOrientation) {
		float a = 0.0, b = 0.0, c = 0.0, d = 0.0;

		getOrientation(field, a, b, c, d);
		return Common::composeString(a) + "/" +
		       Common::composeString(b) + "/" +
		       Common::composeString(c) + "/" +
		       Common::composeString(d);
	}

	throw Common::Exception("GFF3: Field is not a string(able) type");
}

void GFF3Struct::getLocString(const Common::UString &field, LocString &str) const {
	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeLocString)
		throw Common::Exception("GFF3: Field is not of a localized string type");

	Common::SeekableReadStream &data = getData(*f);

	const uint32 size = data.readUint32LE();
	Common::SeekableSubReadStream locStringData(&data, data.pos(), data.pos() + size);

	str.readLocString(locStringData);
}

Common::SeekableReadStream *GFF3Struct::getData(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		return 0;
	if (f->type != kFieldTypeVoid)
		throw Common::Exception("GFF3: Field is not a data type");

	Common::SeekableReadStream &data = getData(*f);

	const uint32 size = data.readUint32LE();
	return data.readStream(size);
}

void GFF3Struct::getVector(const Common::UString &field,
                           float &x, float &y, float &z) const {

	const Field *f = getField(field);
	if (!f)
		return;
	if (f->type != kFieldTypeVector)
		throw Common::Exception("GFF3: Field is not a vector type");

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
		throw Common::Exception("GFF3: Field is not an orientation type");

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
		throw Common::Exception("GFF3: Field is not a vector type");

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
		throw Common::Exception("GFF3: Field is not an orientation type");

	Common::SeekableReadStream &data = getData(*f);

	a = data.readIEEEFloatLE();
	b = data.readIEEEFloatLE();
	c = data.readIEEEFloatLE();
	d = data.readIEEEFloatLE();
}

// --- Struct reader ---

const GFF3Struct &GFF3Struct::getStruct(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("GFF3: No such field");
	if (f->type != kFieldTypeStruct)
		throw Common::Exception("GFF3: Field is not a struct type");

	// Direct index into the struct array
	return _parent->getStruct(f->data);
}

// --- Struct list reader ---

const GFF3List &GFF3Struct::getList(const Common::UString &field) const {
	const Field *f = getField(field);
	if (!f)
		throw Common::Exception("GFF3: No such field");
	if (f->type != kFieldTypeList)
		throw Common::Exception("GFF3: Field is not a list type");

	// Byte offset into the list area, all 32bit values.
	return _parent->getList(f->data / 4);
}

} // End of namespace Aurora

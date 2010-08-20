/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gfffile.cpp
 *  Handling BioWare's GFFs (generic file format).
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/error.h"
#include "aurora/locstring.h"

static const uint32 kVersion32 = MKID_BE('V3.2');
static const uint32 kVersion33 = MKID_BE('V3.3'); // Found in The Witcher, different language table

namespace Aurora {

GFFFile::StructIterator::StructIterator(const StructIterator &it) {
	_it  = it._it;
	_gff = it._gff;
}

GFFFile::StructIterator::StructIterator(const Struct::const_iterator &it, const GFFFile &gff) {
	_it  =  it;
	_gff = &gff;
}

GFFFile::StructIterator &GFFFile::StructIterator::operator++() {
	++_it;

	return *this;
}

GFFFile::StructIterator GFFFile::StructIterator::operator++(int) {
	StructIterator tmp(*this);
	++(*this);
	return tmp;
}

GFFFile::StructIterator &GFFFile::StructIterator::operator--() {
	--_it;

	return *this;
}

GFFFile::StructIterator GFFFile::StructIterator::operator--(int) {
	StructIterator tmp(*this);
	--(*this);
	return tmp;
}

const GFFField &GFFFile::StructIterator::operator*() const {
	return *_it;
}

const GFFField *GFFFile::StructIterator::operator->() const {
	return &*_it;
}

bool GFFFile::StructIterator::operator==(const StructIterator &x) const {
	return _it == x._it;
}

bool GFFFile::StructIterator::operator!=(const StructIterator &x) const {
	return _it != x._it;
}

GFFFile::StructRange GFFFile::StructIterator::structRange(uint32 structID) const {
	return _gff->structRange(structID);
}

GFFFile::ListRange GFFFile::StructIterator::listRange(uint32 listID) const {
	return _gff->listRange(listID);
}


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


GFFFile::GFFFile() {
}

GFFFile::~GFFFile() {
}

void GFFFile::clear() {
	_header.clear();

	_structArray.clear();
	_listArray.clear();
}

void GFFFile::load(Common::SeekableReadStream &gff) {
	clear();

	readHeader(gff);

	if ((_version != kVersion32) && (_version != kVersion33))
		throw Common::Exception("Unsupported GFF file version %08X", _version);

	_header.read(gff);

	try {

		// Read structs
		_structArray.resize(_header.structCount);
		for (uint32 i = 0; i < _header.structCount; i++) {
			gff.skip(4); // Programmer-defined ID

			uint32 data  = gff.readUint32LE();
			uint32 count = gff.readUint32LE();

			uint32 curPos = gff.pos();

			_structArray[i].resize(count);
			if (count > 1)
				readFields(gff, _structArray[i], data);
			else
				readField(gff, _structArray[i][0], data);

			gff.seek(curPos);
		}

		if (!gff.seek(_header.listIndicesOffset))
			throw Common::Exception(Common::kSeekError);

		// Read list array
		std::vector<uint32> rawListArray;
		uint32 listArrayCount = _header.listIndicesCount / 4;
		rawListArray.resize(listArrayCount);
		for (uint32 i = 0; i < listArrayCount; i++)
			rawListArray[i] = gff.readUint32LE();

		// Counting the actual amount of lists
		uint32 realListCount = 0;
		for (uint32 i = 0; i < listArrayCount; i++) {
			uint32 n = rawListArray[i];

			if ((i + n) > listArrayCount)
				throw Common::Exception("List indices broken");

			i += n;
			realListCount++;
		}

		// Converting the raw list to an useable list
		_listArray.reserve(realListCount);
		_rawListToListMap.resize(listArrayCount);
		for (uint32 i = 0; i < listArrayCount; ) {
			List list;

			uint32 n = rawListArray[i];
			for (uint32 j = 0; j < n; j++) {
				uint32 k = rawListArray[i + 1 + j];

				if (k >= _structArray.size())
					throw Common::Exception("List indices broken");

				_rawListToListMap[i + 1 + j] = _listArray.end();

				list.push_back(std::make_pair(StructIterator(_structArray[k].begin(), *this),
				                              StructIterator(_structArray[k].end()  , *this)));
			}

			_listArray.push_back(list);
			_rawListToListMap[i] = --_listArray.end();
			i += n + 1;
		}

	} catch (Common::Exception &e) {
		e.add("Failed reading GFF file");
		throw e;
	}

}

void GFFFile::readField(Common::SeekableReadStream &gff, GFFField &field, uint32 fieldIndex) {
	// Sanity check
	if (fieldIndex > _header.fieldCount)
		throw Common::Exception("Field index out of range (%d/%d)", fieldIndex, _header.fieldCount);

	// Seek
	if (!gff.seek(_header.fieldOffset + fieldIndex * 12))
		throw Common::Exception(Common::kSeekError);

	field.read(gff, _header);
}

void GFFFile::readFields(Common::SeekableReadStream &gff, Struct &strct, uint32 fieldIndicesIndex) {
	// Sanity check
	if (fieldIndicesIndex > _header.fieldIndicesCount)
		throw Common::Exception("Field indices index out of range (%d/%d)",
		                        fieldIndicesIndex, _header.fieldIndicesCount);

	// Seek
	if (!gff.seek(_header.fieldIndicesOffset + fieldIndicesIndex))
		throw Common::Exception(Common::kSeekError);

	// Get the number of structs in the list
	uint32 count = strct.size();

	std::vector<uint32> indices;

	// Read all struct indices
	indices.resize(count);
	for (uint32 i = 0; i < count; i++)
		indices[i] = gff.readUint32LE();

	// Read the structs
	for (uint32 i = 0; i < count; i++)
		readField(gff, strct[i], indices[i]);
}

GFFFile::StructRange GFFFile::structRange(uint32 structID) const {
	return std::make_pair(StructIterator(_structArray[structID].begin(), *this),
	                      StructIterator(_structArray[structID].end()  , *this));
}

GFFFile::ListRange GFFFile::listRange(uint32 listID) const {
	return std::make_pair(_rawListToListMap[listID]->begin(), _rawListToListMap[listID]->end());
}


GFFField::GFFField() {
	_gffType = kGFFTypeNone;
	_type    = kTypeNone;
}

GFFField::~GFFField() {
	clear();
}

void GFFField::clear() {
	if (_type == kTypeNone)
		return;

	// Free memory if we need to
	if      (_type == kTypeString)
		delete _value.typeString;
	else if (_type == kTypeLocString)
		delete _value.typeLocString;
	else if (_type == kTypeData)
		delete[] _value.typeData;

	_gffType = kGFFTypeNone;
	_type    = kTypeNone;

	_dataSize = 0;

	_label.clear();
}

GFFField::Type GFFField::getType() const {
	return _type;
}

const Common::UString &GFFField::getLabel() const {
	return _label;
}

char GFFField::getChar() const {
	if (_type != kTypeChar)
		throw kGFFFieldTypeError;

	return (char) _value.typeInt;
}

uint64 GFFField::getUint() const {
	if ((_type != kTypeUint) && (_type != kTypeSint))
		throw kGFFFieldTypeError;

	return (uint64) _value.typeInt;
}

int64 GFFField::getSint() const {
	if ((_type != kTypeUint) && (_type != kTypeSint))
		throw kGFFFieldTypeError;

	return (int64) _value.typeInt;
}

double GFFField::getDouble() const {
	if (_type != kTypeDouble)
		throw kGFFFieldTypeError;

	return _value.typeDouble;
}

const Common::UString &GFFField::getString() const {
	if (_type != kTypeString)
		throw kGFFFieldTypeError;

	return *_value.typeString;
}

const LocString &GFFField::getLocString() const {
	if (_type != kTypeLocString)
		throw kGFFFieldTypeError;

	return *_value.typeLocString;
}

uint32 GFFField::getDataSize() const {
	if (_type != kTypeData)
		throw kGFFFieldTypeError;

	return _dataSize;
}

const byte *GFFField::getData() const {
	if (_type != kTypeData)
		throw kGFFFieldTypeError;

	return (const byte *) *_value.typeData;
}

const float *GFFField::getVector() const {
	if (_type != kTypeVector)
		throw kGFFFieldTypeError;

	return _value.typeVector;
}

const uint32 GFFField::getStructIndex() const {
	if (_type != kTypeStruct)
		throw kGFFFieldTypeError;

	return _value.typeIndex;
}

const uint32 GFFField::getListIndex() const {
	if (_type != kTypeList)
		throw kGFFFieldTypeError;

	return _value.typeIndex;
}

void GFFField::read(Common::SeekableReadStream &gff, const GFFFile::Header &header) {
	clear();

	// Read the data

	_gffType = (GFFType) gff.readUint32LE();

	uint32 labelIndex = gff.readUint32LE();
	uint32 data       = gff.readUint32LE();

	// Supported type?
	_type = toType(_gffType);
	if (_type == kTypeNone)
		throw Common::Exception("Unknown field type %d", _gffType);

	// Sanity check
	if (labelIndex >= header.labelCount)
		throw Common::Exception("Label index out of range (%d/%d)", labelIndex, header.labelCount);

	// Read the label
	uint32 curPos = gff.seekTo(header.labelOffset + labelIndex * 16);
	_label.readASCII(gff, 16);
	gff.seekTo(curPos);

	// Fill in the correct data
	convertData(gff, header, data);
}

void GFFField::convertData(Common::SeekableReadStream &gff, const GFFFile::Header &header, uint32 data) {
	// Do the correct conversion/reading for each data type

	switch (_gffType) {
		case kGFFTypeChar:
			// Correct sign extension
			_value.typeInt = (uint64) ((char) data);
			break;

		case kGFFTypeByte:
			// Correct sign extension
			_value.typeInt = (uint64) ((byte) data);
			break;

		case kGFFTypeUint16:
			// Correct sign extension
			_value.typeInt = (uint64) ((uint16) data);
			break;

		case kGFFTypeUint32:
			// Correct sign extension
			_value.typeInt = (uint64) ((uint32) data);
			break;

		case kGFFTypeUint64:
			readUint64(gff, header, data);
			break;

		case kGFFTypeSint16:
			// Correct sign extension
			_value.typeInt = (uint64) ((int64) ((int16) ((uint16) data)));
			break;

		case kGFFTypeSint32:
			// Correct sign extension
			_value.typeInt = (uint64) ((int64) ((int32) ((uint32) data)));
			break;

		case kGFFTypeSint64:
			readSint64(gff, header, data);
			break;

		case kGFFTypeFloat:
			_value.typeDouble = (double) convertIEEEFloat(data);
			break;

		case kGFFTypeDouble:
			readDouble(gff, header, data);
			break;

		case kGFFTypeExoString:
			readExoString(gff, header, data);
			break;

		case kGFFTypeResRef:
			readResRef(gff, header, data);
			break;

		case kGFFTypeLocString:
			readLocString(gff, header, data);
			break;

		case kGFFTypeVoid:
			readVoid(gff, header, data);
			break;

		case kGFFTypeStruct:
			// Direct index into the struct array
			_value.typeIndex = data;
			break;

		case kGFFTypeList:
			// Byte index into the list area, all 32bit values.
			_value.typeIndex = data / 4;
			break;

		case kGFFTypeOrientation:
			warning("TODO: kGFFTypeOrientation");
			break;

		case kGFFTypeVector:
			readVector(gff, header, data);
			break;

		default:
			throw Common::Exception("Unknown field type %d", _gffType);
	}

}

inline void GFFField::seekGFFData(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data, uint32 &curPos) {

	if (data >= header.fieldDataCount)
		throw Common::Exception("Field data index out of range (%d/%d)", data, header.fieldDataCount);

	curPos = gff.pos();
	if (!gff.seek(header.fieldDataOffset + data))
		throw Common::Exception(Common::kSeekError);
}

inline void GFFField::readUint64(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	_value.typeInt = gff.readUint64LE();

	gff.seek(curPos);
}

inline void GFFField::readSint64(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	_value.typeInt = (uint64) gff.readSint64LE();

	gff.seek(curPos);
}

inline void GFFField::readDouble(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	_value.typeDouble = gff.readIEEEDoubleLE();

	gff.seek(curPos);
}

inline void GFFField::readExoString(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	uint32 length = gff.readUint32LE();

	_value.typeString = new Common::UString;

	_value.typeString->readASCII(gff, length);

	gff.seek(curPos);
}

inline void GFFField::readResRef(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	uint32 length = gff.readByte();

	_value.typeString = new Common::UString;

	_value.typeString->readASCII(gff, length);

	gff.seek(curPos);
}

inline void GFFField::readLocString(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	_value.typeLocString = new LocString();

	gff.skip(4); // Size in bytes
	_value.typeLocString->readLocString(gff);

	gff.seek(curPos);
}

inline void GFFField::readVoid(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	_dataSize = gff.readUint32LE();

	_value.typeData = new byte[_dataSize];

	if (gff.read(_value.typeData, _dataSize) != _dataSize) {
		delete[] _value.typeData;

		_value.typeData = 0;
		_dataSize = 0;

		throw Common::Exception(Common::kReadError);
	}

	gff.seek(curPos);
}

inline void GFFField::readVector(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	seekGFFData(gff, header, data, curPos);

	gff.seek(curPos);
}

inline GFFField::Type GFFField::toType(GFFType type) {
	switch (type) {
		case kGFFTypeChar:
			return kTypeChar;

		// This is all an unsigned integer value
		case kGFFTypeByte:
		case kGFFTypeUint16:
		case kGFFTypeUint32:
		case kGFFTypeUint64:
			return kTypeUint;

		// This is all a signed integer value
		case kGFFTypeSint16:
		case kGFFTypeSint32:
		case kGFFTypeSint64:
			return kTypeSint;

		// Float/Double type
		case kGFFTypeFloat:
		case kGFFTypeDouble:
			return kTypeDouble;

		// All strings
		case kGFFTypeExoString:
		case kGFFTypeResRef:
			return kTypeString;

		case kGFFTypeLocString:
			return kTypeLocString;

		case kGFFTypeVoid:
			return kTypeData;

		case kGFFTypeStruct:
			return kTypeStruct;

		case kGFFTypeList:
			return kTypeList;

		case kGFFTypeOrientation:
			return kTypeOrientation;

		case kGFFTypeVector:
			return kTypeVector;

		default:
			break;
	}

	return kTypeNone;
}

} // End of namespace Aurora

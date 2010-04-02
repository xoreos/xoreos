/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "common/stream.h"
#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/aurorafile.h"
#include "aurora/locstring.h"

static const uint32 kVersion32 = MKID_BE('V3.2');
static const uint32 kVersion33 = MKID_BE('V3.3'); // Found in The Witcher, different language table

namespace Aurora {

GFFFile::Header::Header() {
	clear();
}

void GFFFile::Header::clear() {
	id                 = 0;
	version            = 0;
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

bool GFFFile::Header::read(Common::SeekableReadStream &gff) {
	id      = gff.readUint32BE();
	version = gff.readUint32BE();

	if ((version != kVersion32) && (version != kVersion33)) {
		warning("GFFFile::Header::read(): Unsupported file version");
		return false;
	}

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

	return true;
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

bool GFFFile::load(Common::SeekableReadStream &gff) {
	if (!_header.read(gff))
		return false;

	// Read structs
	_structArray.resize(_header.structCount);
	for (uint32 i = 0; i < _header.structCount; i++) {
		gff.skip(4); // Programmer-defined ID

		uint32 data  = gff.readUint32LE();
		uint32 count = gff.readUint32LE();

		uint32 curPos = gff.pos();

		_structArray[i].resize(count);
		if (count > 1) {
			if (!readFields(gff, _structArray[i], data))
				return false;
		} else
			if (!readField(gff, _structArray[i][0], data))
				return false;

		gff.seek(curPos);
	}

	if (!gff.seek(_header.listIndicesOffset))
		return false;

	// Read list array
	uint32 listArrayCount = _header.listIndicesCount / 4;
	_listArray.resize(listArrayCount);
	for (uint32 i = 0; i < listArrayCount; i++)
		_listArray[i] = gff.readUint32LE();

	return true;
}

bool GFFFile::readField(Common::SeekableReadStream &gff, GFFField &field, uint32 fieldIndex) {
	// Sanity check
	if (fieldIndex > _header.fieldCount)
		return false;

	// Seek
	if (!gff.seek(_header.fieldOffset + fieldIndex * 12))
		return false;

	// Read
	if (!field.read(gff, _header))
		return false;

	return true;
}

bool GFFFile::readFields(Common::SeekableReadStream &gff, GFFStruct &strct, uint32 fieldIndicesIndex) {
	// Sanity check
	if (fieldIndicesIndex > _header.fieldIndicesCount)
		return false;

	// Seek
	if (!gff.seek(_header.fieldIndicesOffset + fieldIndicesIndex))
		return false;

	// Get the number of structs in the list
	uint32 count = strct.size();

	std::vector<uint32> indices;

	// Read all struct indices
	indices.resize(count);
	for (uint32 i = 0; i < count; i++)
		indices[i] = gff.readUint32LE();

	// Read the structs
	for (uint32 i = 0; i < count; i++)
		if (!readField(gff, strct[i], indices[i]))
			return false;

	return true;
}

GFFFile::StructIterator GFFFile::beginStruct(uint32 structID) const {
	return StructIterator();
}

GFFFile::StructIterator GFFFile::endStruct(uint32 structID) const {
	return StructIterator();
}

GFFFile::ListIterator GFFFile::beginList(uint32 listID) const {
	return ListIterator();
}

GFFFile::ListIterator GFFFile::endList(uint32 listID) const {
	return ListIterator();
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

const std::string &GFFField::getLabel() const {
	return _label;
}

char GFFField::getChar() const {
	return (char) _value.typeInt;
}

uint64 GFFField::getUint() const {
	return (uint64) _value.typeInt;
}

int64 GFFField::getSint() const {
	return (int64) _value.typeInt;
}

double GFFField::getDouble() const {
	return _value.typeDouble;
}

const std::string &GFFField::getString() const {
	return *_value.typeString;
}

const LocString &GFFField::getLocString() const {
	return *_value.typeLocString;
}

uint32 GFFField::getDataSize() const {
	return _dataSize;
}

const float *GFFField::getVector() const {
	return _value.typeVector;
}

const byte *GFFField::getData() const {
	return (const byte *) *_value.typeData;
}

const uint32 GFFField::getIndex() const {
	return _value.typeIndex;
}

bool GFFField::read(Common::SeekableReadStream &gff, const GFFFile::Header &header) {
	clear();

	// Read the data

	_gffType = (GFFType) gff.readUint32LE();

	uint32 labelIndex = gff.readUint32LE();
	uint32 data       = gff.readUint32LE();

	// Supported type?
	_type = toType(_gffType);
	if (_type == kTypeNone) {
		warning("GFFField::read(): Unknown field type %d", _gffType);
		return false;
	}

	// Sanity check
	if (labelIndex >= header.labelCount) {
		warning("GFFField::read(): Label index out of range (%d/%d)", labelIndex, header.labelCount);
		return false;
	}

	// Read the label
	_label = AuroraFile::readRawString(gff, 16, header.labelOffset + labelIndex * 16);

	// Fill in the correct data
	if (!convertData(gff, header, data))
		return false;

	return true;
}

bool GFFField::convertData(Common::SeekableReadStream &gff, const GFFFile::Header &header, uint32 data) {
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
			if (!readUint64(gff, header, data))
				return false;
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
			if (!readSint64(gff, header, data))
				return false;
			break;

		case kGFFTypeFloat:
			_value.typeDouble = (double) AuroraFile::convertFloat(data);
			break;

		case kGFFTypeDouble:
			if (!readDouble(gff, header, data))
				return false;
			break;

		case kGFFTypeExoString:
			if (!readExoString(gff, header, data))
				return false;
			break;

		case kGFFTypeResRef:
			if (!readResRef(gff, header, data))
				return false;
			break;

		case kGFFTypeLocString:
			if (!readLocString(gff, header, data))
				return false;
			break;

		case kGFFTypeVoid:
			if (!readVoid(gff, header, data))
				return false;
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
			if (!readVector(gff, header, data))
				return false;
			break;

		default:
			return false;
	}

	return true;
}

inline bool GFFField::seekGFFData(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data, uint32 &curPos) {

	if (data >= header.fieldDataCount)
		return false;

	curPos = gff.pos();
	if (!gff.seek(header.fieldDataOffset + data))
		return false;

	return true;
}

inline bool GFFField::readUint64(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	_value.typeInt = gff.readUint64LE();

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readSint64(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	_value.typeInt = (uint64) gff.readSint64LE();

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readDouble(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	_value.typeDouble = AuroraFile::readDouble(gff);

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readExoString(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	uint32 length = gff.readUint32LE();

	_value.typeString = new std::string;

	*_value.typeString = AuroraFile::readRawString(gff, length);

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readResRef(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	uint32 length = gff.readByte();

	_value.typeString = new std::string;

	*_value.typeString = AuroraFile::readRawString(gff, length);

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readLocString(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	_value.typeLocString = new LocString();

	gff.skip(4); // Size in bytes
	_value.typeLocString->readLocString(gff);

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readVoid(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	_dataSize = gff.readUint32LE();

	_value.typeData = new byte[_dataSize];

	if (gff.read(_value.typeData, _dataSize) != _dataSize) {
		delete[] _value.typeData;

		_value.typeData = 0;
		_dataSize = 0;

		return false;
	}

	gff.seek(curPos);
	return true;
}

inline bool GFFField::readVector(Common::SeekableReadStream &gff,
		const GFFFile::Header &header, uint32 data) {

	uint32 curPos;
	if (!seekGFFData(gff, header, data, curPos))
		return false;

	gff.seek(curPos);
	return true;
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

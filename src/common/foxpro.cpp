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
 *  A database in FoxPro format.
 */

#include "src/common/foxpro.h"
#include "src/common/error.h"
#include "src/common/encoding.h"
#include "src/common/stream.h"

// boost-date_time stuff
using boost::gregorian::date;
using boost::gregorian::day_clock;

namespace Common {

FoxPro::FoxPro() : _hasIndex(false), _hasMemo(false), _memoBlockSize(512) {
}

FoxPro::~FoxPro() {
	for (std::list<byte *>::iterator it = _pool.begin(); it != _pool.end(); ++it)
		delete[] *it;
	for (std::vector<byte *>::iterator it = _memos.begin(); it != _memos.end(); ++it)
		delete[] *it;
}

void FoxPro::load(SeekableReadStream *dbf, SeekableReadStream *cdx,
                  SeekableReadStream *fpt) {

	assert(dbf);

	// Read header
	uint32 recordSize, recordCount, firstRecordPos;
	loadHeader(*dbf, recordSize, recordCount, firstRecordPos);
	if (_hasIndex && !cdx)
		throw Exception("Index needed");
	if (_hasMemo && !fpt)
		throw Exception("Memo needed");

	// Read fields
	loadFields(*dbf, recordSize);
	if (_hasMemo && !fpt)
		throw Exception("Memo needed");

	// Read records
	dbf->seek(firstRecordPos);
	loadRecords(*dbf, recordSize, recordCount);

	// Read memos
	if (fpt)
		loadMemos(*fpt);

	// TODO: Read the compound index (CDX) file
}

void FoxPro::loadHeader(SeekableReadStream &dbf, uint32 &recordSize, uint32 &recordCount,
                        uint32 &firstRecordPos) {

	byte version = dbf.readByte();
	if (version != 0xF5)
		throw Exception("Unknown database version 0x%02X", version);

	int lastUpdateYear  = dbf.readByte() + 2000;
	int lastUpdateMonth = dbf.readByte();
	int lastUpdateDay   = dbf.readByte();

	_lastUpdate = date(lastUpdateYear, lastUpdateMonth, lastUpdateDay);

	recordCount    = dbf.readUint32LE();
	firstRecordPos = dbf.readUint16LE();
	recordSize     = dbf.readUint16LE();

	dbf.skip(16); // Reserved

	byte flags = dbf.readByte();

	_hasIndex = flags & 0x01;
	_hasMemo  = (flags & 0x02) != 0;

	if (flags & 0x04)
		throw Exception("DBC unsupported");
	if (flags & 0xF8)
		throw Exception("Unknown flags 0x%02X", flags);

	dbf.skip(1); // Codepage marker
	dbf.skip(2); // Reserved
}

void FoxPro::loadFields(SeekableReadStream &dbf, uint32 recordSize) {
	// Read all field descriptions, 0x0D is the end marker
	uint32 fieldsLength = 0;
	while (!dbf.eos() && !dbf.err() && (dbf.readByte() != 0x0D)) {
		Field field;

		dbf.seek(-1, SEEK_CUR);

		field.name = readStringFixed(dbf, kEncodingASCII, 11);

		field.type     = (Type) dbf.readByte();
		field.offset   = dbf.readUint32LE();
		field.size     = dbf.readByte();
		field.decimals = dbf.readByte();

		field.flags = dbf.readByte();

		field.autoIncNext = dbf.readUint32LE();
		field.autoIncStep = dbf.readByte();

		dbf.skip(8); // Reserved

		if (field.offset != (fieldsLength + 1))
			throw Exception("Field offset makes no sense (%d != %d)",
					field.offset, fieldsLength + 1);

		if (field.type == kTypeMemo)
			_hasMemo = true;

		fieldsLength += field.size;

		_fields.push_back(field);
	}

	if (dbf.eos() || dbf.err())
		throw Exception(kReadError);

	if (recordSize != (fieldsLength + 1))
		throw Exception("Length of all fields does not equal the record size");
}

void FoxPro::loadRecords(SeekableReadStream &dbf, uint32 recordSize, uint32 recordCount) {
	_pool.push_back(new byte[recordSize * recordCount]);
	byte *recordData = _pool.back();

	if (dbf.read(recordData, recordSize * recordCount) != (recordSize * recordCount))
		throw Exception(kReadError);

	if (dbf.readByte() != 0x1A)
		throw Exception("Record end marker missing");

	uint32 fieldCount = _fields.size();

	// Create the records array
	_records.resize(recordCount);
	for (uint32 i = 0; i < recordCount; i++) {
		Record &record = _records[i];
		const byte *data = recordData + i * recordSize;

		char status = *data++;
		if ((status != ' ') && (status != '*'))
			throw Exception("Unknown record status '%c'", status);

		record.deleted = status == '*';

		record.fields.resize(fieldCount);
		for (uint32 j = 0; j < fieldCount; j++) {
			record.fields[j] = data;
			data += _fields[j].size;
		}
	}
}

void FoxPro::loadMemos(SeekableReadStream &fpt) {
	fpt.skip(4); // Next free block
	fpt.skip(2); // Unused

	_memoBlockSize = fpt.readUint16BE();
	if (_memoBlockSize < 33)
		_memoBlockSize *= 1024;

	fpt.skip(504); // Unused

	while (!fpt.eos() && !fpt.err()) {
		_memos.push_back(new byte[_memoBlockSize]);
		byte *data = _memos.back();

		fpt.read(data, _memoBlockSize);
	}

	if (fpt.err())
		throw Exception(kReadError);
}

void FoxPro::save(WriteStream *dbf, WriteStream *cdx, WriteStream *fpt) const {
	assert(dbf);

	if (_hasIndex && !cdx)
		throw Exception("Index needed");
	if (_hasMemo && !fpt)
		throw Exception("Memo needed");

	if (_records.empty() || _fields.empty())
		throw Exception("No records / fields");

	saveHeader(*dbf);
	saveFields(*dbf);
	saveRecords(*dbf);
	if (!dbf->flush() || dbf->err())
		throw Exception(kWriteError);

	if (fpt) {
		saveMemos(*fpt);

		if (!fpt->flush() || fpt->err())
			throw Exception(kWriteError);
	}

	// TODO: Write the compound index (CDX) file

	if (cdx && (!cdx->flush() || cdx->err()))
		throw Exception(kWriteError);
}

void FoxPro::saveHeader(WriteStream &dbf) const {
	dbf.writeByte(0xF5); // Version

	dbf.writeByte(_lastUpdate.year() - 2000);
	dbf.writeByte(_lastUpdate.month());
	dbf.writeByte(_lastUpdate.day());

	dbf.writeUint32LE(_records.size());

	//                    Header + fields + field end marker
	uint16 firstRecordPos = 32 + _fields.size() * 32 + 1;
	dbf.writeUint16LE(firstRecordPos);

	uint16 recordSize = _fields.back().offset + _fields.back().size;
	dbf.writeUint16LE(recordSize);

	dbf.writeUint32LE(0x00000000); // Reserved
	dbf.writeUint32LE(0x00000000); // Reserved
	dbf.writeUint32LE(0x00000000); // Reserved
	dbf.writeUint32LE(0x00000000); // Reserved

	uint8 flags = (_hasIndex ? 0x01 : 0x00) | (_hasMemo ? 0x02 : 0x00);
	dbf.writeByte(flags);

	dbf.writeByte(0x00); // Codepage marker

	dbf.writeUint16LE(0x0000); // Reserved
}

void FoxPro::saveFields(WriteStream &dbf) const {
	for (uint i = 0; i < _fields.size(); i++) {
		const Field &field = _fields[i];

		int l = strlen(field.name.c_str());

		dbf.write(field.name.c_str(), MIN(10, l));
		dbf.writeByte(0x00);

		while ((10 - l++) > 0)
			dbf.writeByte(0x00);

		dbf.writeByte((byte) ((char) field.type));

		dbf.writeUint32LE(field.offset);

		dbf.writeByte(field.size);
		dbf.writeByte(field.decimals);
		dbf.writeByte(field.flags);

		dbf.writeUint32LE(field.autoIncNext);
		dbf.writeByte    (field.autoIncStep);

		dbf.writeUint32LE(0x00000000); // Reserved
		dbf.writeUint32LE(0x00000000); // Reserved
	}

	dbf.writeByte(0x0D); // Field end marker
}

void FoxPro::saveRecords(WriteStream &dbf) const {
	// Write the records
	for (uint i = 0; i < _records.size(); i++) {
		const Record &record = _records[i];

		dbf.writeByte(record.deleted ? '*' : ' ');

		for (uint j = 0; j < _fields.size(); j++)
			dbf.write(record.fields[j], _fields[j].size);
	}

	dbf.writeByte(0x1A); // Records end marker
}

void FoxPro::saveMemos(WriteStream &fpt) const {
	fpt.writeUint32BE(_memos.size() + 1); // Next free block
	fpt.writeUint16BE(0x0000);        // Reserved
	fpt.writeUint16BE(_memoBlockSize);

	// Reserved
	for (int i = 0; i < 126; i++)
		fpt.writeUint32BE(0x00000000);

	for (uint i = 0; i < _memos.size(); i++)
		fpt.write(_memos[i], _memoBlockSize);
}

date FoxPro::getLastUpdate() const {
	return _lastUpdate;
}

bool FoxPro::hasIndex() const {
	return _hasIndex;
}

bool FoxPro::hasMemo() const {
	return _hasMemo;
}

uint32 FoxPro::getFieldCount() const {
	return _fields.size();
}

uint32 FoxPro::getRecordCount() const {
	return _records.size();
}

const std::vector<FoxPro::Field> &FoxPro::getFields() const {
	return _fields;
}

const std::vector<FoxPro::Record> &FoxPro::getRecords() const {
	return _records;
}

UString FoxPro::getString(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];
	if (f.type != kTypeString)
		throw Exception("Field is not of string type ('%c')", f.type);

	MemoryReadStream stream(record.fields[field], f.size);

	UString str = readStringFixed(stream, kEncodingLatin9, f.size);

	// xBase fields are padded with spaces...
	str.trimRight();

	return str;
}

int32 FoxPro::getInt(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];

	int32 i = 0;
	if        (f.type == kTypeNumber) {

		if (!getInt(record.fields[field], f.size, i))
			i = 0;

	} else if (f.type == kTypeInteger) {

		if (f.size != 4)
			throw Exception("Integer field size != 4 (%d)", f.size);

		i = READ_LE_UINT32(record.fields[field]);

	} else
		throw Exception("Field is not of int type ('%c')", f.type);

	return i;
}

bool FoxPro::getBool(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];
	if (f.type != kTypeBool)
		throw Exception("Field is not of bool type ('%c')", f.type);

	if (f.size != 1)
		throw Exception("Bool field size != 1 (%d)", f.size);

	char c = (char) record.fields[field][0];

	if ((c == 't') || (c == 'T') || (c == 'y') || (c == 'Y') || (c == '1'))
		return true;

	return false;
}

double FoxPro::getDouble(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];

	char n[32];
	double d = 0.0;
	if        (f.type == kTypeNumber) {

		if (f.size > 31)
			throw Exception("Numerical field size > 31 (%d)", f.size);

		strncpy(n, (const char *) record.fields[field], f.size);
		n[f.size] = '\0';

		if (sscanf(n, "%lf", &d) != 1)
			d = 0.0;

	} else if (f.type == kTypeFloat) {

		if (f.size != 4)
			throw Exception("Float field size != 4 (%d)", f.size);

		d = convertIEEEFloat(READ_LE_UINT32(record.fields[field]));

	} else if (f.type == kTypeDouble) {

		if (f.size != 8)
			throw Exception("Double field size != 8 (%d)", f.size);

		d = convertIEEEDouble(READ_LE_UINT64(record.fields[field]));

	} else
		throw Exception("Field is not of double type ('%c')", f.type);

	return d;
}

date FoxPro::getDate(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];
	if (f.type != kTypeDate)
		throw Exception("Field is not of date type ('%c')", f.type);

	if (f.size != 8)
			throw Exception("Date field size != 8 (%d)", f.size);

	int year, month, day;
	if (sscanf((const char *) record.fields[field], "%4d%2d%2d", &year, &month, &day) != 3)
		throw Exception("Failed reading the date");

	return date(year, month, day);
}

SeekableReadStream *FoxPro::getMemo(const Record &record, uint32 field) const {
	assert(field < _fields.size());

	const Field &f = _fields[field];
	if (f.type != kTypeMemo)
		throw Exception("Field is not of memo type ('%c')", f.type);

	int32 i = 0;
	if (!getInt(record.fields[field], f.size, i) || (i < 1))
		return 0;

	uint32 block = ((uint32) i) - 1;

	if (block >= _memos.size())
		throw Exception("Memo block #%d >= memo block count %d", block, _memos.size());

	uint32 type = READ_BE_UINT32(_memos[block] + 0);
	uint32 size = READ_BE_UINT32(_memos[block] + 4);

	if ((type != 0x00) && (type != 0x01) && (type != 0x02))
		throw Exception("Memo type unknown (%d)", type);

	bool first = true;

	uint32 dataSize = size;

	// Read the data
	byte *data    = new byte[size];
	byte *dataPtr = data;
	while (size > 0) {
		if (block >= _memos.size()) {
			delete[] data;
			throw Exception("Memo block #%d >= memo block count %d", block, _memos.size());
		}

		uint32 n = MIN<uint32>(size, _memoBlockSize - (first ? 8 : 0));

		memcpy(dataPtr, _memos[block] + (first ? 8 : 0), n);

		dataPtr += n;
		size    -= n;
		block   += 1;

		first = false;
	}

	return new MemoryReadStream(data, dataSize, true);
}

void FoxPro::deleteRecord(uint32 record) {
	assert(record < _records.size());

	_records[record].deleted = true;

	updateUpdate();

	// TODO: Deleting a record should also mark any memo fields in that
	//       record as free. They should be reused when adding a memo
	//       field of equals or less size.
}

uint32 FoxPro::addFieldString(const UString &name, uint8 size) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeString;
	field.offset      = offset;
	field.size        = size;
	field.decimals    = 0;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	return _fields.size() - 1;
}

uint32 FoxPro::addFieldNumber(const UString &name, uint8 size, uint8 decimals) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeNumber;
	field.offset      = offset;
	field.size        = size;
	field.decimals    = decimals;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	return _fields.size() - 1;
}

uint32 FoxPro::addFieldInt(const UString &name) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeInteger;
	field.offset      = offset;
	field.size        = 4;
	field.decimals    = 0;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	return _fields.size() - 1;
}

uint32 FoxPro::addFieldBool(const UString &name) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeBool;
	field.offset      = offset;
	field.size        = 1;
	field.decimals    = 0;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	return _fields.size() - 1;
}

uint32 FoxPro::addFieldDate(const UString &name) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeDate;
	field.offset      = offset;
	field.size        = 8;
	field.decimals    = 0;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	return _fields.size() - 1;
}

uint32 FoxPro::addFieldMemo(const UString &name) {
	uint32 offset = 1;
	if (!_fields.empty())
		offset = _fields.back().offset + _fields.back().size;

	_fields.push_back(Field());

	Field &field = _fields.back();

	field.name        = name;
	field.type        = kTypeMemo;
	field.offset      = offset;
	field.size        = 10;
	field.decimals    = 0;
	field.flags       = 0;
	field.autoIncNext = 0;
	field.autoIncStep = 0;

	addField(field.size);
	updateUpdate();

	_hasMemo = true;

	return _fields.size() - 1;
}

void FoxPro::addField(uint8 size) {
	if (_records.empty())
		return;

	uint32 dataSize = size * _records.size();

	_pool.push_back(new byte[dataSize]);

	byte *data = _pool.back();

	for (std::vector<Record>::iterator it = _records.begin(); it != _records.end(); ++it) {
		it->fields.push_back(data);
		data += size;
	}
}

uint32 FoxPro::addRecord() {
	_records.push_back(Record());
	Record &record = _records.back();

	record.deleted = false;

	if (!_fields.empty()) {
		uint32 dataSize = _fields.back().offset + _fields.back().size;

		_pool.push_back(new byte[dataSize]);

		byte *data = _pool.back();

		record.fields.resize(_fields.size());
		for (uint i = 0; i < _fields.size(); i++) {
			record.fields[i] = data;
			data += _fields[i].size;
		}
	}

	updateUpdate();

	return _records.size() - 1;
}

void FoxPro::setString(uint32 record, uint32 field, const UString &value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	if (f.type != kTypeString)
		throw Exception("Field is not of string type ('%c')", f.type);

	char *data    = (char *) r.fields[field];
	char *dataEnd = (char *) r.fields[field] + f.size;

	strncpy(data, value.c_str(), f.size);

	data += strlen(value.c_str());

	while (data < dataEnd)
		*dataEnd++ = 0x20;

	updateUpdate();
}

void FoxPro::setInt(uint32 record, uint32 field, int32 value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	char *data = (char *) r.fields[field];

	if        (f.type == kTypeNumber) {

		if (f.decimals != 0)
			snprintf(data, f.size, "%*d", f.size, value);
		else
			snprintf(data, f.size, "%*.*f", f.size, f.decimals, (double) value);

	} else if (f.type == kTypeInteger) {

		if (f.size != 4)
			throw Exception("Integer field size != 4 (%d)", f.size);

		WRITE_LE_UINT32(data, value);

	} else
		throw Exception("Field is not of int type ('%c')", f.type);

	updateUpdate();
}

void FoxPro::setBool(uint32 record, uint32 field, bool value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	if (f.type != kTypeBool)
		throw Exception("Field is not of bool type ('%c')", f.type);

	if (f.size != 1)
		throw Exception("Bool field size != 1 (%d)", f.size);

	char *data = (char *) r.fields[field];

	data[0] = value ? 'T' : 'F';

	updateUpdate();
}

void FoxPro::setDouble(uint32 record, uint32 field, double value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	char *data = (char *) r.fields[field];

	if        (f.type == kTypeNumber) {

		if (f.decimals != 0)
			snprintf(data, f.size, "%*d", f.size, (int32) value);
		else
			snprintf(data, f.size, "%*.*f", f.size, f.decimals, value);

	} else if (f.type == kTypeFloat) {

		if (f.size != 4)
			throw Exception("Float field size != 4 (%d)", f.size);

		WRITE_LE_UINT32(data, convertIEEEFloat((float) value));

	} else if (f.type == kTypeDouble) {

		if (f.size != 8)
			throw Exception("Double field size != 8 (%d)", f.size);

		WRITE_LE_UINT64(data, convertIEEEDouble(value));

	} else
		throw Exception("Field is not of double type ('%c')", f.type);

	updateUpdate();
}

void FoxPro::setDate(uint32 record, uint32 field, const date &value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	if (f.type != kTypeDate)
		throw Exception("Field is not of date type ('%c')", f.type);

	char *data = (char *) r.fields[field];

	snprintf(data, 8, "%04d%02d%02d",
			(int) value.year(), (int) value.month(), (int) value.day());

	updateUpdate();
}

void FoxPro::setMemo(uint32 record, uint32 field, SeekableReadStream *value) {
	assert((record < _records.size()) && (field < _fields.size()));

	Record &r = _records[record];
	Field  &f = _fields[field];

	if (f.type != kTypeMemo)
		throw Exception("Field is not of memo type ('%c')", f.type);

	char *data = (char *) r.fields[field];

	if (!value) {
		memset(data, 0x20, f.size);
		updateUpdate();
		return;
	}

	value->seek(0);

	uint32 size = value->size();

	uint32 block = _memos.size();
	_memos.push_back(new byte[_memoBlockSize]);

	uint32 startBlock = block + 1;

	WRITE_BE_UINT32(_memos[block]    , 1);
	WRITE_BE_UINT32(_memos[block] + 4, size);

	bool first = true;
	while (size > 0) {
		uint32 n = MIN<uint32>(size, _memoBlockSize - (first ? 8 : 0));

		if (value->read(_memos[block] + (first ? 8 : 0), n) != n)
			throw Exception(kReadError);

		size  -= n;
		block += 1;

		if (size > 0)
			_memos.push_back(new byte[_memoBlockSize]);

		first = false;
	}

	if (f.decimals != 0)
		snprintf(data, f.size, "%*d", f.size, startBlock);
	else
		snprintf(data, f.size, "%*.*f", f.size, f.decimals, (double) startBlock);

	updateUpdate();
}

bool FoxPro::getInt(const byte *data, uint32 size, int32 &i) {
	char n[32];

	if (size > 31)
		throw Exception("Numerical field size > 31 (%d)", size);

	strncpy(n, (const char *) data, size);
	n[size] = '\0';

	return sscanf(n, "%d", &i) == 1;
}

void FoxPro::updateUpdate() {
	_lastUpdate = day_clock::universal_day();
}

} // End of namespace Common

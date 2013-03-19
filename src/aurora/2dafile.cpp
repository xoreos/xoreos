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

/** @file aurora/2dafile.cpp
 *  Handling BioWare's 2DAs (two-dimensional array).
 */

#include "common/util.h"
#include "common/strutil.h"
#include "common/stream.h"
#include "common/file.h"
#include "common/streamtokenizer.h"

#include "aurora/2dafile.h"
#include "aurora/error.h"

static const uint32 k2DAID     = MKTAG('2', 'D', 'A', ' ');
static const uint32 kVersion2a = MKTAG('V', '2', '.', '0');
static const uint32 kVersion2b = MKTAG('V', '2', '.', 'b');

namespace Aurora {

TwoDARow::TwoDARow(TwoDAFile &parent) : _parent(&parent) {
}

TwoDARow::~TwoDARow() {
}

const Common::UString &TwoDARow::getString(uint32 column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultString;

	return cell;
}

const Common::UString &TwoDARow::getString(const Common::UString &column) const {
	const Common::UString &cell = getCell(_parent->headerToColumn(column));
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultString;

	return cell;
}

const int32 TwoDARow::getInt(uint32 column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultInt;

	return _parent->parseInt(cell);
}

const int32 TwoDARow::getInt(const Common::UString &column) const {
	const Common::UString &cell = getCell(_parent->headerToColumn(column));
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultInt;

	return _parent->parseInt(cell);
}

const float TwoDARow::getFloat(uint32 column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultFloat;

	return _parent->parseFloat(cell);
}

const float TwoDARow::getFloat(const Common::UString &column) const {
	const Common::UString &cell = getCell(_parent->headerToColumn(column));
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultFloat;

	return _parent->parseFloat(cell);
}

static const Common::UString kEmpty;
const Common::UString &TwoDARow::getCell(uint32 n) const {
	if (n >= _data.size())
		return kEmpty;

	return _data[n];
}


TwoDAFile::TwoDAFile() : _defaultInt(0), _defaultFloat(0.0), _emptyRow(*this) {
}

TwoDAFile::~TwoDAFile() {
	clear();
}

void TwoDAFile::clear() {
	AuroraBase::clear();

	_headers.clear();

	for (std::vector<TwoDARow *>::iterator row = _rows.begin(); row != _rows.end(); ++row)
		delete *row;
	_rows.clear();

	_headerMap.clear();

	_defaultString.clear();
	_defaultInt   = 0;
	_defaultFloat = 0.0;
}

void TwoDAFile::load(Common::SeekableReadStream &twoda) {
	clear();

	readHeader(twoda);

	if (_id != k2DAID)
		throw Common::Exception("Not a 2DA file");

	if ((_version != kVersion2a) && (_version != kVersion2b))
		throw Common::Exception("Unsupported 2DA file version %08X", _version);

	Common::UString lineRest;
	lineRest.readLineASCII(twoda); // Skip the rest of the line

	try {

		if      (_version == kVersion2a)
			read2a(twoda);
		else if (_version == kVersion2b)
			read2b(twoda);

		// Create the map to quickly translate headers to column indices
		createHeaderMap();

		if (twoda.err())
			throw Common::Exception(Common::kReadError);

	} catch (Common::Exception &e) {
		e.add("Failed reading 2DA file");
		throw;
	}

}

void TwoDAFile::read2a(Common::SeekableReadStream &twoda) {
	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleIgnoreAll);

	tokenize.addSeparator(' ');
	tokenize.addSeparator('\t');
	tokenize.addQuote('\"');
	tokenize.addChunkEnd('\n');
	tokenize.addIgnore('\r');

	readDefault2a(twoda, tokenize);
	readHeaders2a(twoda, tokenize);
	readRows2a(twoda, tokenize);
}

void TwoDAFile::read2b(Common::SeekableReadStream &twoda) {
	readHeaders2b(twoda);
	skipRowNames2b(twoda);
	readRows2b(twoda);
}

void TwoDAFile::readDefault2a(Common::SeekableReadStream &twoda,
                              Common::StreamTokenizer &tokenize) {

	std::vector<Common::UString> defaultRow;
	tokenize.getTokens(twoda, defaultRow, 2);

	if (defaultRow[0] == "Default:")
		_defaultString = defaultRow[1];

	_defaultInt   = parseInt(_defaultString);
	_defaultFloat = parseFloat(_defaultString);

	tokenize.nextChunk(twoda);
}

void TwoDAFile::readHeaders2a(Common::SeekableReadStream &twoda,
                              Common::StreamTokenizer &tokenize) {

	tokenize.getTokens(twoda, _headers);

	tokenize.nextChunk(twoda);
}

void TwoDAFile::readRows2a(Common::SeekableReadStream &twoda,
                           Common::StreamTokenizer &tokenize) {

	uint32 columnCount = _headers.size();

	while (!twoda.eos()) {
		TwoDARow *row = new TwoDARow(*this);

		tokenize.skipToken(twoda);

		int count = tokenize.getTokens(twoda, row->_data, columnCount, columnCount);

		tokenize.nextChunk(twoda);

		if (count == 0) {
			// Ignore empty lines
			delete row;
			continue;
		}

		_rows.push_back(row);
	}
}

void TwoDAFile::readHeaders2b(Common::SeekableReadStream &twoda) {
	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\t');
	tokenize.addSeparator('\0');

	Common::UString header = tokenize.getToken(twoda);
	while (!header.empty()) {
		_headers.push_back(header);

		header = tokenize.getToken(twoda);
	}
}

void TwoDAFile::skipRowNames2b(Common::SeekableReadStream &twoda) {
	uint32 rowCount = twoda.readUint32LE();

	_rows.reserve(rowCount);
	for (uint32 i = 0; i < rowCount; i++)
		_rows.push_back(0);

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\t');
	tokenize.addSeparator('\0');

	tokenize.skipToken(twoda, rowCount);
}

void TwoDAFile::readRows2b(Common::SeekableReadStream &twoda) {
	uint32 columnCount = _headers.size();
	uint32 rowCount    = _rows.size();
	uint32 cellCount   = columnCount * rowCount;

	uint32 *offsets = new uint32[cellCount];

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\0');

	for (uint32 i = 0; i < cellCount; i++)
		offsets[i] = twoda.readUint16LE();

	twoda.skip(2); // Reserved

	uint32 dataOffset = twoda.pos();

	for (uint32 i = 0; i < rowCount; i++) {
		_rows[i] = new TwoDARow(*this);

		_rows[i]->_data.resize(columnCount);

		for (uint32 j = 0; j < columnCount; j++) {
			uint32 offset = dataOffset + offsets[i * columnCount + j];

			if (!twoda.seek(offset)) {
				delete[] offsets;
				throw Common::Exception(Common::kSeekError);
			}

			_rows[i]->_data[j] = tokenize.getToken(twoda);
			if (_rows[i]->_data[j].empty())
				_rows[i]->_data[j] = "****";
		}
	}

	delete[] offsets;
}

void TwoDAFile::createHeaderMap() {
	for (uint32 i = 0; i < _headers.size(); i++)
		_headerMap.insert(std::make_pair(_headers[i], i));
}

uint32 TwoDAFile::getRowCount() const {
	return _rows.size();
}

uint32 TwoDAFile::getColumnCount() const {
	return _headers.size();
}

const std::vector<Common::UString> &TwoDAFile::getHeaders() const {
	return _headers;
}

uint32 TwoDAFile::headerToColumn(const Common::UString &header) const {
	HeaderMap::const_iterator column = _headerMap.find(header);
	if (column == _headerMap.end())
		// No such header
		return kFieldIDInvalid;

	return column->second;
}

const TwoDARow &TwoDAFile::getRow(uint32 row) const {
	if ((row >= _rows.size()) || !_rows[row])
		// No such row
		return _emptyRow;

	return *_rows[row];
}

bool TwoDAFile::dumpASCII(const Common::UString &fileName) const {
	Common::DumpFile file;
	if (!file.open(fileName))
		return false;

	// Write header

	file.writeString("2DA V2.0\n");
	file.writeString(_defaultString); file.writeByte('\n');

	// Calculate column lengths

	std::vector<uint32> colLength;
	colLength.resize(_headers.size() + 1);

	const Common::UString maxRow = Common::UString::sprintf("%d", _rows.size() - 1);
	colLength[0] = maxRow.size();

	for (uint32 i = 0; i < _headers.size(); i++)
		colLength[i + 1] = _headers[i].size();

	for (uint32 i = 0; i < _rows.size(); i++)
		for (uint32 j = 0; j < _rows[i]->_data.size(); j++)
			colLength[j + 1] = MAX<uint32>(colLength[j + 1], _rows[i]->_data[j].size());

	// Write column headers

	file.writeString(Common::UString::sprintf("%-*s", colLength[0], ""));

	for (uint32 i = 0; i < _headers.size(); i++)
		file.writeString(Common::UString::sprintf(" %-*s", colLength[i + 1], _headers[i].c_str()));

	file.writeByte('\n');

	// Write array

	for (uint32 i = 0; i < _rows.size(); i++) {
		file.writeString(Common::UString::sprintf("%*d", colLength[0], i));

		for (uint32 j = 0; j < _rows[i]->_data.size(); j++)
			file.writeString(Common::UString::sprintf(" %-*s", colLength[j + 1], _rows[i]->_data[j].c_str()));

		file.writeByte('\n');
	}

	file.flush();
	file.close();

	return true;
}

int32 TwoDAFile::parseInt(const Common::UString &str) {
	if (str.empty())
		return 0;

	int v;
	if (!str.parse(v))
		return 0;

	return v;
}

float TwoDAFile::parseFloat(const Common::UString &str) {
	if (str.empty())
		return 0;

	float v;
	if (!str.parse(v))
		return 0.0;

	return v;
}

} // End of namespace Aurora

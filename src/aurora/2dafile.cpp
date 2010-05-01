/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/2dafile.cpp
 *  Handling BioWare's 2DAs (two-dimensional array).
 */

#include "common/stream.h"
#include "common/util.h"
#include "common/strutil.h"
#include "common/streamtokenizer.h"

#include "aurora/2dafile.h"
#include "aurora/error.h"

static const uint32 k2DAID     = MKID_BE('2DA ');
static const uint32 kVersion2a = MKID_BE('V2.0');
static const uint32 kVersion2b = MKID_BE('V2.b');

namespace Aurora {

TwoDAFile::TwoDAFile() {
	_tokenizeASCII = new Common::StreamTokenizer(Common::StreamTokenizer::kRuleIgnoreAll);

	_tokenizeASCII->addSeparator(' ');
	_tokenizeASCII->addQuote('\"');
	_tokenizeASCII->addChunkEnd('\n');
	_tokenizeASCII->addIgnore('\r');
}

TwoDAFile::~TwoDAFile() {
	delete _tokenizeASCII;
}

void TwoDAFile::clear() {
	AuroraBase::clear();

	_headers.clear();

	for (Array::iterator row = _array.begin(); row != _array.end(); ++row)
		delete *row;
	_array.clear();

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
		throw e;
	}

}

void TwoDAFile::read2a(Common::SeekableReadStream &twoda) {
	readDefault2a(twoda);
	readHeaders2a(twoda);
	readRows2a(twoda);
}

void TwoDAFile::read2b(Common::SeekableReadStream &twoda) {
	readHeaders2b(twoda);
	skipRowNames2b(twoda);
	readRows2b(twoda);
}

void TwoDAFile::readDefault2a(Common::SeekableReadStream &twoda) {
	std::vector<Common::UString> defaultRow;
	_tokenizeASCII->getTokens(twoda, defaultRow, 2);

	if (defaultRow[0] == "Default:")
		_defaultString = defaultRow[1];

	_defaultInt   = parseInt(_defaultString);
	_defaultFloat = parseFloat(_defaultString);

	_tokenizeASCII->nextChunk(twoda);
}

void TwoDAFile::readHeaders2a(Common::SeekableReadStream &twoda) {
	_tokenizeASCII->getTokens(twoda, _headers);

	_tokenizeASCII->nextChunk(twoda);
}

void TwoDAFile::readRows2a(Common::SeekableReadStream &twoda) {
	uint32 columnCount = _headers.size();

	while (!twoda.eos()) {
		Row *row = new Row;

		_tokenizeASCII->skipToken(twoda);

		int count = _tokenizeASCII->getTokens(twoda, *row, columnCount);

		_tokenizeASCII->nextChunk(twoda);

		if (count == 0)
			// Ignore empty lines
			continue;

		_array.push_back(row);
	}
}

void TwoDAFile::readHeaders2b(Common::SeekableReadStream &twoda) {
	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\t');
	tokenize.addChunkEnd('\0');

	tokenize.getTokens(twoda, _headers);
	tokenize.nextChunk(twoda);
}

void TwoDAFile::skipRowNames2b(Common::SeekableReadStream &twoda) {
	uint32 rowCount = twoda.readUint32LE();

	_array.reserve(rowCount);
	for (uint32 i = 0; i < rowCount; i++)
		_array.push_back(0);

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\t');

	Row rowNames;
	rowNames.reserve(rowCount);

	tokenize.getTokens(twoda, rowNames, rowCount, rowCount);
}

void TwoDAFile::readRows2b(Common::SeekableReadStream &twoda) {
	uint32 columnCount = _headers.size();
	uint32 rowCount    = _array.size();
	uint32 cellCount   = columnCount * rowCount;

	uint32 *offsets = new uint32[cellCount];

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\0');

	for (uint32 i = 0; i < cellCount; i++)
		offsets[i] = twoda.readUint16LE();

	twoda.skip(2); // Reserved

	uint32 dataOffset = twoda.pos();

	for (uint32 i = 0; i < rowCount; i++) {
		_array[i] = new Row;

		_array[i]->resize(columnCount);

		for (uint32 j = 0; j < columnCount; j++) {
			uint32 offset = dataOffset + offsets[i * columnCount + j];

			if (!twoda.seek(offset)) {
				delete[] offsets;
				throw Common::Exception(Common::kSeekError);
			}

			(*_array[i])[j] = tokenize.getToken(twoda);
		}
	}

	delete[] offsets;
}

void TwoDAFile::createHeaderMap() {
	for (uint32 i = 0; i < _headers.size(); i++)
		_headerMap.insert(std::make_pair(_headers[i], i));
}

uint32 TwoDAFile::rowCount() const {
	return _array.size();
}

uint32 TwoDAFile::columnCount() const {
	return _headers.size();
}

const TwoDAFile::Row &TwoDAFile::getHeaders() const {
	return _headers;
}

uint32 TwoDAFile::headerToColumn(const Common::UString &header) const {
	HeaderMap::const_iterator column = _headerMap.find(header);
	if (column == _headerMap.end())
		// No such header
		return kColumnInvalid;

	return column->second;
}

const TwoDAFile::Row *TwoDAFile::getRow(uint32 row) const {
	if (row >= _array.size())
		// No such row
		return 0;

	return _array[row];
}

const Common::UString *TwoDAFile::getCell(uint32 row, uint32 column) const {
	const Row *r = getRow(row);
	if (!r)
		// No such row
		return 0;

	if (column >= r->size())
		// No such column
		return 0;

	return &(*r)[column];
}

static const Common::UString kEmptyString;

const Common::UString &TwoDAFile::getCellString(uint32 row, uint32 column) const {
	const Common::UString *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultString;

	// Check if the cell is empty
	if (*cell == "****")
		return kEmptyString;

	return *cell;
}

const Common::UString &TwoDAFile::getCellString(uint32 row, const Common::UString &column) const {
	return getCellString(row, headerToColumn(column));
}

const int32 TwoDAFile::getCellInt(uint32 row, uint32 column) const {
	const Common::UString *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultInt;

	// Check if the cell is empty
	if (*cell == "****")
		return 0;

	return parseInt(*cell);
}

const int32 TwoDAFile::getCellInt(uint32 row, const Common::UString &column) const {
	return getCellInt(row, headerToColumn(column));
}

const float TwoDAFile::getCellFloat(uint32 row, uint32 column) const {
	const Common::UString *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultFloat;

	// Check if the cell is empty
	if (*cell == "****")
		return 0.0;

	return parseFloat(*cell);
}

const float TwoDAFile::getCellFloat(uint32 row, const Common::UString &column) const {
	return getCellFloat(row, headerToColumn(column));
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

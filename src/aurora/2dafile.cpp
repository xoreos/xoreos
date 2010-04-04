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

#include "boost/algorithm/string.hpp"
#include "boost/tokenizer.hpp"

#include "common/stream.h"
#include "common/util.h"
#include "common/strutil.h"

#include "aurora/2dafile.h"

static const uint32 k2DAID     = MKID_BE('2DA ');
static const uint32 kVersion2a = MKID_BE('V2.0');
static const uint32 kVersion2b = MKID_BE('V2.b');

namespace Aurora {

TwoDAFile::TwoDAFile() {
}

TwoDAFile::~TwoDAFile() {
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

bool TwoDAFile::load(Common::SeekableReadStream &twoda) {
	clear();

	readHeader(twoda);

	if (_id != k2DAID) {
		warning("TwoDAFile::load(): Not a 2DA file");
		return false;
	}

	if ((_version != kVersion2a) && (_version != kVersion2b)) {
		warning("TwoDAFile::load(): Unsupported file version");
		return false;
	}

	if (_version == kVersion2b) {
		warning("TODO: Binary version");
		return false;
	}

	twoda.readLine(); // Skip the rest of the line

	if      (_version == kVersion2a) {
		if (!read2a(twoda))
			return false;
	} else if (_version == kVersion2b) {
		if (!read2b(twoda))
			return false;
	}

	// Create the map to quickly translate headers to column indices
	createHeaderMap();

	return true;
}

bool TwoDAFile::read2a(Common::SeekableReadStream &twoda) {
	if (!readDefault2a(twoda))
		return false;
	if (!readHeaders2a(twoda))
		return false;
	if (!readRows2a(twoda))
		return false;

	return true;
}

bool TwoDAFile::read2b(Common::SeekableReadStream &twoda) {
	if (!readHeaders2b(twoda))
		return false;
	if (!skipRowNames2b(twoda))
		return false;
	if (!readRows2b(twoda))
		return false;

	return true;
}

bool TwoDAFile::readDefault2a(Common::SeekableReadStream &twoda) {
	std::string defaultList = twoda.readLine();

	Row defaultRow;
	defaultRow.reserve(2);

	split(defaultList, defaultRow);
	if ((defaultRow.size() >= 2) && (defaultRow[0] == "Default:"))
		_defaultString = defaultRow[1];

	_defaultInt   = parseInt(_defaultString);
	_defaultFloat = parseFloat(_defaultString);

	return true;
}

bool TwoDAFile::readHeaders2a(Common::SeekableReadStream &twoda) {
	std::string headerLine = twoda.readLine();
	split(headerLine, _headers);

	return true;
}

bool TwoDAFile::readRows2a(Common::SeekableReadStream &twoda) {
	uint32 columnCount = _headers.size();

	std::string line;
	while (!twoda.eos()) {
		Row *row = new Row;

		// We can expect that amount of fields
		row->reserve(columnCount);

		// Read the row
		line = twoda.readLine();
		split(line, *row, 1);

		_array.push_back(row);
	}

	return true;
}

bool TwoDAFile::readHeaders2b(Common::SeekableReadStream &twoda) {
	return true;
}

bool TwoDAFile::skipRowNames2b(Common::SeekableReadStream &twoda) {
	return true;
}

bool TwoDAFile::readRows2b(Common::SeekableReadStream &twoda) {
	return true;
}

void TwoDAFile::createHeaderMap() {
	for (uint32 i = 0; i < _headers.size(); i++)
		_headerMap.insert(std::make_pair(_headers[i], i));
}

void TwoDAFile::split(const std::string &str, Row &row, uint32 skip) {
	// Create a tokenizer over the string, splitting at spaces, but accept spaces within quotes
	boost::tokenizer<boost::escaped_list_separator<char> > tok(str, boost::escaped_list_separator<char>("", " ", "\""));

	for(boost::tokenizer<boost::escaped_list_separator<char> >::iterator it = tok.begin(); it != tok.end(); ++it) {
		if (it->empty())
			// Skip empty tokens
			continue;

		if (skip > 0) {
			// Skip the specified amount of real tokens
			skip--;
			continue;
		}

		row.push_back(*it);
	}
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

uint32 TwoDAFile::headerToColumn(const std::string &header) const {
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

const std::string *TwoDAFile::getCell(uint32 row, uint32 column) const {
	const Row *r = getRow(row);
	if (!r)
		// No such row
		return 0;

	if (column >= r->size())
		// No such column
		return 0;

	return &(*r)[column];
}

static const std::string kEmptyString;

const std::string &TwoDAFile::getCellString(uint32 row, uint32 column) const {
	const std::string *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultString;

	// Check if the cell is empty
	if (*cell == "****")
		return kEmptyString;

	return *cell;
}

const std::string &TwoDAFile::getCellString(uint32 row, const std::string &column) const {
	return getCellString(row, headerToColumn(column));
}

const int32 TwoDAFile::getCellInt(uint32 row, uint32 column) const {
	const std::string *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultInt;

	// Check if the cell is empty
	if (*cell == "****")
		return 0;

	return parseInt(*cell);
}

const int32 TwoDAFile::getCellInt(uint32 row, const std::string &column) const {
	return getCellInt(row, headerToColumn(column));
}

const float TwoDAFile::getCellFloat(uint32 row, uint32 column) const {
	const std::string *cell = getCell(row, column);
	if (!cell)
		// Cell does not exist, return default value
		return _defaultFloat;

	// Check if the cell is empty
	if (*cell == "****")
		return 0.0;

	return parseFloat(*cell);
}

const float TwoDAFile::getCellFloat(uint32 row, const std::string &column) const {
	return getCellFloat(row, headerToColumn(column));
}

int32 TwoDAFile::parseInt(const std::string &str) {
	if (str.empty())
		return 0;

	int v;
	if (!Common::stringConvert(str, v))
		return 0;

	return v;
}

float TwoDAFile::parseFloat(const std::string &str) {
	if (str.empty())
		return 0;

	float v;
	if (!Common::stringConvert(str, v))
		return 0.0;

	return v;
}

} // End of namespace Aurora

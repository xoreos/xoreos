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
 *  Handling BioWare's 2DAs (two-dimensional array).
 */

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"
#include "src/common/readstream.h"
#include "src/common/writefile.h"
#include "src/common/streamtokenizer.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/types.h"

static const uint32 k2DAID     = MKTAG('2', 'D', 'A', ' ');
static const uint32 k2DAIDTab  = MKTAG('2', 'D', 'A', '\t');
static const uint32 kVersion2a = MKTAG('V', '2', '.', '0');
static const uint32 kVersion2b = MKTAG('V', '2', '.', 'b');

namespace Aurora {

TwoDARow::TwoDARow(TwoDAFile &parent) : _parent(&parent) {
}

TwoDARow::~TwoDARow() {
}

const Common::UString &TwoDARow::getString(size_t column) const {
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

int32 TwoDARow::getInt(size_t column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultInt;

	return _parent->parseInt(cell);
}

int32 TwoDARow::getInt(const Common::UString &column) const {
	const Common::UString &cell = getCell(_parent->headerToColumn(column));
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultInt;

	return _parent->parseInt(cell);
}

float TwoDARow::getFloat(size_t column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultFloat;

	return _parent->parseFloat(cell);
}

float TwoDARow::getFloat(const Common::UString &column) const {
	const Common::UString &cell = getCell(_parent->headerToColumn(column));
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultFloat;

	return _parent->parseFloat(cell);
}

bool TwoDARow::empty(size_t column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return true;

	return false;
}

bool TwoDARow::empty(const Common::UString &column) const {
	return empty(_parent->headerToColumn(column));
}

static const Common::UString kEmpty;
const Common::UString &TwoDARow::getCell(size_t n) const {
	if (n >= _data.size())
		return kEmpty;

	return _data[n];
}


TwoDAFile::TwoDAFile(Common::SeekableReadStream &twoda) :
	_defaultInt(0), _defaultFloat(0.0f), _emptyRow(*this) {

	load(twoda);
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
	_defaultFloat = 0.0f;
}

void TwoDAFile::load(Common::SeekableReadStream &twoda) {
	readHeader(twoda);

	if ((_id != k2DAID) && (_id != k2DAIDTab))
		throw Common::Exception("Not a 2DA file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion2a) && (_version != kVersion2b))
		throw Common::Exception("Unsupported 2DA file version %s", Common::debugTag(_version).c_str());

	Common::UString lineRest = Common::readStringLine(twoda, Common::kEncodingASCII);

	try {

		if      (_version == kVersion2a)
			read2a(twoda);
		else if (_version == kVersion2b)
			read2b(twoda);

		// Create the map to quickly translate headers to column indices
		createHeaderMap();

	} catch (Common::Exception &e) {
		clear();

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

	if (defaultRow[0].equalsIgnoreCase("Default:"))
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

	size_t columnCount = _headers.size();

	while (!twoda.eos()) {
		TwoDARow *row = new TwoDARow(*this);

		tokenize.skipToken(twoda);

		size_t count = tokenize.getTokens(twoda, row->_data, columnCount, columnCount);

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
	size_t columnCount = _headers.size();
	size_t rowCount    = _rows.size();
	size_t cellCount   = columnCount * rowCount;

	uint32 *offsets = new uint32[cellCount];

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\0');

	for (size_t i = 0; i < cellCount; i++)
		offsets[i] = twoda.readUint16LE();

	twoda.skip(2); // Reserved

	size_t dataOffset = twoda.pos();

	for (size_t i = 0; i < rowCount; i++) {
		_rows[i] = new TwoDARow(*this);

		_rows[i]->_data.resize(columnCount);

		for (size_t j = 0; j < columnCount; j++) {
			size_t offset = dataOffset + offsets[i * columnCount + j];

			try {
				twoda.seek(offset);
			} catch (...) {
				delete[] offsets;
				throw;
			}

			_rows[i]->_data[j] = tokenize.getToken(twoda);
			if (_rows[i]->_data[j].empty())
				_rows[i]->_data[j] = "****";
		}
	}

	delete[] offsets;
}

void TwoDAFile::createHeaderMap() {
	for (size_t i = 0; i < _headers.size(); i++)
		_headerMap.insert(std::make_pair(_headers[i], i));
}

size_t TwoDAFile::getRowCount() const {
	return _rows.size();
}

size_t TwoDAFile::getColumnCount() const {
	return _headers.size();
}

const std::vector<Common::UString> &TwoDAFile::getHeaders() const {
	return _headers;
}

size_t TwoDAFile::headerToColumn(const Common::UString &header) const {
	HeaderMap::const_iterator column = _headerMap.find(header);
	if (column == _headerMap.end())
		// No such header
		return kFieldIDInvalid;

	return column->second;
}

const TwoDARow &TwoDAFile::getRow(size_t row) const {
	if ((row >= _rows.size()) || !_rows[row])
		// No such row
		return _emptyRow;

	return *_rows[row];
}

bool TwoDAFile::dumpASCII(const Common::UString &fileName) const {
	Common::WriteFile file;
	if (!file.open(fileName))
		return false;

	// Write header

	file.writeString("2DA V2.0\n");
	if (!_defaultString.empty())
		file.writeString(Common::UString::format("DEFAULT: %s", _defaultString.c_str()));
	file.writeByte('\n');

	// Calculate column lengths

	std::vector<size_t> colLength;
	colLength.resize(_headers.size() + 1, 0);

	const Common::UString maxRow = Common::UString::format("%d", (int)_rows.size() - 1);
	colLength[0] = maxRow.size();

	for (size_t i = 0; i < _headers.size(); i++)
		colLength[i + 1] = _headers[i].size();

	for (size_t i = 0; i < _rows.size(); i++) {
		for (size_t j = 0; j < _rows[i]->_data.size(); j++) {
			const bool   needQuote = _rows[i]->_data[j].contains(' ');
			const size_t length    = needQuote ? _rows[i]->_data[j].size() + 2 : _rows[i]->_data[j].size();

			colLength[j + 1] = MAX<size_t>(colLength[j + 1], length);
		}
	}

	// Write column headers

	file.writeString(Common::UString::format("%-*s", (int)colLength[0], ""));

	for (size_t i = 0; i < _headers.size(); i++)
		file.writeString(Common::UString::format(" %-*s", (int)colLength[i + 1], _headers[i].c_str()));

	file.writeByte('\n');

	// Write array

	for (size_t i = 0; i < _rows.size(); i++) {
		file.writeString(Common::UString::format("%*u", (int)colLength[0], (uint)i));

		for (size_t j = 0; j < _rows[i]->_data.size(); j++) {
			const bool needQuote = _rows[i]->_data[j].contains(' ');

			Common::UString cellString;
			if (needQuote)
				cellString = Common::UString::format("\"%s\"", _rows[i]->_data[j].c_str());
			else
				cellString = _rows[i]->_data[j];

			file.writeString(Common::UString::format(" %-*s", (int)colLength[j + 1], cellString.c_str()));

		}

		file.writeByte('\n');
	}

	file.flush();
	file.close();

	return true;
}

int32 TwoDAFile::parseInt(const Common::UString &str) {
	if (str.empty())
		return 0;

	int32 v = 0;

	try {
		Common::parseString(str, v);
	} catch (...) {
	}

	return v;
}

float TwoDAFile::parseFloat(const Common::UString &str) {
	if (str.empty())
		return 0;

	float v = 0.0f;

	try {
		Common::parseString(str, v);
	} catch (...) {
	}

	return v;
}

} // End of namespace Aurora

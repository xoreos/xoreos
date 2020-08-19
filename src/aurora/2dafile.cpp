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

#include <cassert>

#include <memory>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/strutil.h"
#include "src/common/encoding.h"
#include "src/common/readstream.h"
#include "src/common/writefile.h"
#include "src/common/streamtokenizer.h"

#include "src/aurora/types.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/gdafile.h"
#include "src/aurora/gdaheaders.h"
#include "src/aurora/gff4file.h"

static const uint32_t k2DAID     = MKTAG('2', 'D', 'A', ' ');
static const uint32_t k2DAIDTab  = MKTAG('2', 'D', 'A', '\t');
static const uint32_t kVersion2a = MKTAG('V', '2', '.', '0');
static const uint32_t kVersion2b = MKTAG('V', '2', '.', 'b');

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

int32_t TwoDARow::getInt(size_t column) const {
	const Common::UString &cell = getCell(column);
	if (cell.empty() || (cell == "****"))
		return _parent->_defaultInt;

	return _parent->parseInt(cell);
}

int32_t TwoDARow::getInt(const Common::UString &column) const {
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

TwoDAFile::TwoDAFile(const GDAFile &gda) :
	_defaultInt(0), _defaultFloat(0.0f), _emptyRow(*this) {

	load(gda);
}

TwoDAFile::~TwoDAFile() {
}

void TwoDAFile::load(Common::SeekableReadStream &twoda) {
	readHeader(twoda);

	if ((_id != k2DAID) && (_id != k2DAIDTab))
		throw Common::Exception("Not a 2DA file (%s)", Common::debugTag(_id).c_str());

	if ((_version != kVersion2a) && (_version != kVersion2b))
		throw Common::Exception("Unsupported 2DA file version %s", Common::debugTag(_version).c_str());

	// Ignore the rest of the line; it's garbage
	Common::readStringLine(twoda, Common::kEncodingASCII);

	try {

		if      (_version == kVersion2a)
			read2a(twoda); // ASCII
		else if (_version == kVersion2b)
			read2b(twoda); // Binary

		// Create the map to quickly translate headers to column indices
		createHeaderMap();

	} catch (Common::Exception &e) {
		e.add("Failed reading 2DA file");
		throw;
	}

}

void TwoDAFile::read2a(Common::SeekableReadStream &twoda) {
	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleIgnoreAll);

	// Spaces and tabs act to separate cells
	tokenize.addSeparator(' ');
	tokenize.addSeparator('\t');
	// We can quote spaces and tabs with "
	tokenize.addQuote('\"');
	// \n ends a whole row
	tokenize.addChunkEnd('\n');
	// We're ignoring \r
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

	/* ASCII 2DA files can have default values that are returned for cells
	 * that don't exist. They are specified in the second line, optionally
	 * preceded by "Default:".
	 */

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

	/* Read the column headers of an ASCII 2DA file. */

	while (!twoda.eos() && (tokenize.getTokens(twoda, _headers) == 0))
		tokenize.nextChunk(twoda);

	tokenize.nextChunk(twoda);
}

void TwoDAFile::readRows2a(Common::SeekableReadStream &twoda,
                           Common::StreamTokenizer &tokenize) {

	/* And now read the individual cells in the rows. */

	const size_t columnCount = _headers.size();

	while (!twoda.eos()) {
		std::unique_ptr<TwoDARow> row(new TwoDARow(*this));

		/* Skip the first token, which is the row index, possibly indented.
		 * The row index is implicit in the data and its use in the 2DA
		 * file is only meant as a guideline for people editing the file by
		 * hand. It might even be completely incorrect. */
		tokenize.findFirstToken(twoda);
		tokenize.skipToken(twoda);

		// Read all the cells in the row
		size_t count = tokenize.getTokens(twoda, row->_data, columnCount, columnCount, "****");

		// And move to the next line
		tokenize.nextChunk(twoda);

		// Ignore empty lines
		if (count == 0)
			continue;

		_rows.push_back(row.release());
	}
}

void TwoDAFile::readHeaders2b(Common::SeekableReadStream &twoda) {
	/* Read the column headers of a binary 2DA file. */

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	// Individual column headers a separated by either a tab or a NUL
	tokenize.addSeparator('\t');
	tokenize.addSeparator('\0');

	Common::UString header = tokenize.getToken(twoda);
	while (!header.empty()) {
		_headers.push_back(header);

		header = tokenize.getToken(twoda);
	}
}

void TwoDAFile::skipRowNames2b(Common::SeekableReadStream &twoda) {
	/* Next up are the row names / indices. Like for the ASCII 2DA files,
	 * the actual row indices are implicit in the data, so we're just
	 * ignoring them. The only information we care about is how many rows
	 * there are.
	 */

	const uint32_t rowCount = twoda.readUint32LE();
	_rows.resize(rowCount, 0);

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	// Individual row indices a separated by either a tab or a NUL
	tokenize.addSeparator('\t');
	tokenize.addSeparator('\0');

	tokenize.skipToken(twoda, rowCount);
}

void TwoDAFile::readRows2b(Common::SeekableReadStream &twoda) {
	/* And now read the cells. In binary 2DA files, each cell only
	 * stores a single 16-bit number, the offset into the data segment
	 * where the data for this cell can be found. Moreover, a single
	 * data offset can be used by several cells, deduplicating the
	 * cell data.
	 */

	const size_t columnCount = _headers.size();
	const size_t rowCount    = _rows.size();
	const size_t cellCount   = columnCount * rowCount;

	std::unique_ptr<uint32_t[]> offsets = std::make_unique<uint32_t[]>(cellCount);

	Common::StreamTokenizer tokenize(Common::StreamTokenizer::kRuleHeed);

	tokenize.addSeparator('\0');

	for (size_t i = 0; i < cellCount; i++)
		offsets[i] = twoda.readUint16LE();

	twoda.skip(2); // Size of the data segment in bytes

	const size_t dataOffset = twoda.pos();

	for (size_t i = 0; i < rowCount; i++) {
		_rows[i] = new TwoDARow(*this);

		_rows[i]->_data.resize(columnCount);

		for (size_t j = 0; j < columnCount; j++) {
			const size_t offset = dataOffset + offsets[i * columnCount + j];

			twoda.seek(offset);

			_rows[i]->_data[j] = tokenize.getToken(twoda);
			if (_rows[i]->_data[j].empty())
				_rows[i]->_data[j] = "****";
		}
	}
}

void TwoDAFile::createHeaderMap() {
	for (size_t i = 0; i < _headers.size(); i++)
		_headerMap.insert(std::make_pair(_headers[i], i));
}

void TwoDAFile::load(const GDAFile &gda) {
	try {

		const GDAFile::Headers &headers = gda.getHeaders();
		assert(headers.size() == gda.getColumnCount());

		_headers.resize(gda.getColumnCount());
		for (size_t i = 0; i < gda.getColumnCount(); i++) {
			const char *headerString = findGDAHeader(headers[i].hash);

			_headers[i] = headerString ? headerString : Common::UString::format("[%u]", headers[i].hash);
		}

		_rows.resize(gda.getRowCount(), 0);
		for (size_t i = 0; i < gda.getRowCount(); i++) {
			const GFF4Struct *row = gda.getRow(i);

			_rows[i] = new TwoDARow(*this);
			_rows[i]->_data.resize(gda.getColumnCount());

			for (size_t j = 0; j < gda.getColumnCount(); j++) {
				if (row) {
					switch (headers[j].type) {
						case GDAFile::kTypeString:
						case GDAFile::kTypeResource:
							_rows[i]->_data[j] = row->getString(headers[j].field);
							break;

						case GDAFile::kTypeInt:
							_rows[i]->_data[j] = Common::UString::format("%d", (int) row->getSint(headers[j].field));
							break;

						case GDAFile::kTypeFloat:
							_rows[i]->_data[j] = Common::UString::format("%f", row->getDouble(headers[j].field));
							break;

						case GDAFile::kTypeBool:
							_rows[i]->_data[j] = Common::UString::format("%u", (uint) row->getUint(headers[j].field));
							break;

						default:
							break;
					}
				}

				if (_rows[i]->_data[j].empty())
					_rows[i]->_data[j] = "****";

			}
		}

	} catch (Common::Exception &e) {
		e.add("Failed reading GDA file");
		throw;
	}

	createHeaderMap();
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

const TwoDARow &TwoDAFile::getRow(const Common::UString &header, const Common::UString &value) const {
	size_t columnIndex = headerToColumn(header);
	if (columnIndex == kFieldIDInvalid)
		return _emptyRow;

	for (std::vector<TwoDARow *>::const_iterator row = _rows.begin(); row != _rows.end(); ++row) {
		if ((*row)->getString(columnIndex).equalsIgnoreCase(value))
			return **row;
	}

	// No such row
	return _emptyRow;
}

void TwoDAFile::writeASCII(Common::WriteStream &out) const {
	// Write header

	out.writeString("2DA V2.0\n");
	if (!_defaultString.empty())
		out.writeString(Common::UString::format("DEFAULT: %s", _defaultString.c_str()));
	out.writeByte('\n');

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

	out.writeString(Common::UString::format("%-*s", (int)colLength[0], ""));

	for (size_t i = 0; i < _headers.size(); i++)
		out.writeString(Common::UString::format(" %-*s", (int)colLength[i + 1], _headers[i].c_str()));

	out.writeByte('\n');

	// Write array

	for (size_t i = 0; i < _rows.size(); i++) {
		out.writeString(Common::UString::format("%*u", (int)colLength[0], (uint)i));

		for (size_t j = 0; j < _rows[i]->_data.size(); j++) {
			const bool needQuote = _rows[i]->_data[j].contains(' ');

			Common::UString cellString;
			if (needQuote)
				cellString = Common::UString::format("\"%s\"", _rows[i]->_data[j].c_str());
			else
				cellString = _rows[i]->_data[j];

			out.writeString(Common::UString::format(" %-*s", (int)colLength[j + 1], cellString.c_str()));

		}

		out.writeByte('\n');
	}

	out.flush();
}

bool TwoDAFile::writeASCII(const Common::UString &fileName) const {
	Common::WriteFile file;
	if (!file.open(fileName))
		return false;

	writeASCII(file);
	file.close();

	return true;
}

void TwoDAFile::writeBinary(Common::WriteStream &out) const {
	const size_t columnCount = _headers.size();
	const size_t rowCount    = _rows.size();
	const size_t cellCount   = columnCount * rowCount;

	out.writeString("2DA V2.b\n");

	// Write the column headers

	for (std::vector<Common::UString>::const_iterator h = _headers.begin(); h != _headers.end(); ++h) {
		out.writeString(*h);
		out.writeByte('\t');
	}
	out.writeByte('\0');

	// Write the row indices

	out.writeUint32LE((uint32_t) rowCount);
	for (size_t i = 0; i < rowCount; i++) {
		out.writeString(Common::composeString(i));
		out.writeByte('\t');
	}

	/* Deduplicate cell data strings. Binary 2DA files don't store the
	 * data for each cell directly: instead, each cell contains an offset
	 * into a data array. This way, cells with the same data only need to
	 * to store this data once.
	 *
	 * The original binary 2DA files in KotOR/KotOR2 make extensive use
	 * of that, and we should do this as well.
	 *
	 * Basically, this involves going through each cell, and looking up
	 * if we already saved this particular piece of data. If not, save
	 * it, otherwise only remember the offset. There's no need to be
	 * particularly smart about it, so we're just doing it the naive
	 * O(n^2) way.
	 */

	std::vector<Common::UString> data;
	std::vector<size_t> offsets;

	data.reserve(cellCount);
	offsets.reserve(cellCount);

	size_t dataSize = 0;

	std::vector<size_t> cells;
	cells.reserve(cellCount);

	for (size_t i = 0; i < rowCount; i++) {
		assert(_rows[i]);

		for (size_t j = 0; j < columnCount; j++) {
			const Common::UString cell = _rows[i]->getString(j);

			// Do we already know about this cell data string?
			size_t foundCell = SIZE_MAX;
			for (size_t k = 0; k < data.size(); k++) {
				if (data[k] == cell) {
					foundCell = k;
					break;
				}
			}

			// If not, add it to the cell data array
			if (foundCell == SIZE_MAX) {
				foundCell = data.size();

				data.push_back(cell);
				offsets.push_back(dataSize);

				dataSize += data.back().size() + 1;

				if (dataSize > 65535)
					throw Common::Exception("TwoDAFile::writeBinary(): Cell data size overflow");
			}

			// Remember the offset to the cell data array
			cells.push_back(offsets[foundCell]);
		}
	}

	// Write cell data offsets
	for (std::vector<size_t>::const_iterator c = cells.begin(); c != cells.end(); ++c)
		out.writeUint16LE((uint16_t) *c);

	// Size of the all cell data strings
	out.writeUint16LE((uint16_t) dataSize);

	// Write cell data strings
	for (std::vector<Common::UString>::const_iterator d = data.begin(); d != data.end(); ++d) {
		out.writeString(*d);
		out.writeByte('\0');
	}
}

bool TwoDAFile::writeBinary(const Common::UString &fileName) const {
	Common::WriteFile file;
	if (!file.open(fileName))
		return false;

	writeBinary(file);
	file.close();

	return true;
}

void TwoDAFile::writeCSV(Common::WriteStream &out) const {
	// Write column headers

	for (size_t i = 0; i < _headers.size(); i++) {
		const bool needQuote = _headers[i].contains(',');
		if (needQuote)
			out.writeByte('"');

		out.writeString(_headers[i]);

		if (needQuote)
			out.writeByte('"');

		if (i < (_headers.size() - 1))
			out.writeByte(',');
	}

	out.writeByte('\n');

	// Write array

	for (size_t i = 0; i < _rows.size(); i++) {
		for (size_t j = 0; j < _rows[i]->_data.size(); j++) {
			const bool needQuote = _rows[i]->_data[j].contains(',');

			if (needQuote)
				out.writeByte('"');

			if (_rows[i]->_data[j] != "****")
				out.writeString(_rows[i]->_data[j]);

			if (needQuote)
				out.writeByte('"');

			if (j < (_rows[i]->_data.size() - 1))
				out.writeByte(',');
		}

		out.writeByte('\n');
	}

	out.flush();
}

bool TwoDAFile::writeCSV(const Common::UString &fileName) const {
	Common::WriteFile file;
	if (!file.open(fileName))
		return false;

	writeCSV(file);
	file.close();

	return true;
}

int32_t TwoDAFile::parseInt(const Common::UString &str) {
	if (str.empty())
		return 0;

	int32_t v = 0;

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

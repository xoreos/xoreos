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
 *  Handling BioWare's GDAs (2DAs, two-dimensional array, within V4.0 GFFs).
 */

/* See the GDA description on the Dragon Age toolset wiki
 * (<http://social.bioware.com/wiki/datoolset/index.php/GDA>).
 */

#include <cassert>

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/hash.h"
#include "src/common/strutil.h"

#include "src/aurora/gdafile.h"
#include "src/aurora/gff4file.h"

static const uint32 kG2DAID    = MKTAG('G', '2', 'D', 'A');
static const uint32 kVersion01 = MKTAG('V', '0', '.', '1');
static const uint32 kVersion02 = MKTAG('V', '0', '.', '2');

namespace Aurora {

const size_t GDAFile::kInvalidColumn;
const size_t GDAFile::kInvalidRow;

GDAFile::GDAFile(Common::SeekableReadStream *gda) : _columns(0), _rowCount(0) {
	assert(gda);

	load(gda);
}

GDAFile::~GDAFile() {
}

size_t GDAFile::getColumnCount() const {
	return _columns->size();
}

size_t GDAFile::getRowCount() const {
	return _rowCount;
}

const GDAFile::Headers &GDAFile::getHeaders() const {
	return _headers;
}

bool GDAFile::hasRow(size_t row) const {
	return getRow(row) != 0;
}

const GFF4Struct *GDAFile::getRow(size_t row) const {
	assert(_rowStarts.size() == _rows.size());

	/* To find the correct GFF4 for this row, we go through
	 * the list of row start indices in reverse, until we
	 * found one that's not bigger than the row we want.
	 */

	for (ptrdiff_t i = _rowStarts.size() - 1; i >= 0; i--) {
		if (_rowStarts[i] <= row) {
			row -= _rowStarts[i];

			if (row >= _rows[i]->size())
				return 0;

			return (*_rows[i])[row];
		}
	}

	return 0;
}

size_t GDAFile::findRow(uint32 id) const {
	size_t idColumn = findColumn("ID");
	if (idColumn == kInvalidColumn)
		return kInvalidRow;

	// Go through all rows of all GFF4s, and look for the ID

	size_t gff4 = 0;
	for (size_t i = 0, j = 0; i < _rowCount; i++, j++) {
		if (j >= _rows[gff4]->size()) {
			if (++gff4 >= _rows.size())
				break;

			j = 0;
		}

		if ((*_rows[gff4])[j] && ((*_rows[gff4])[j]->getUint(idColumn) == id))
			return i;
	}

	return kInvalidRow;
}

size_t GDAFile::findColumn(const Common::UString &name) const {
	ColumnNameMap::const_iterator c = _columnNameMap.find(name);
	if (c != _columnNameMap.end())
		return c->second;

	size_t column = findColumn(Common::hashStringCRC32(name.toLower(), Common::kEncodingUTF16LE));
	_columnNameMap[name] = column;

	return column;
}

size_t GDAFile::findColumn(uint32 hash) const {
	ColumnHashMap::const_iterator c = _columnHashMap.find(hash);
	if (c != _columnHashMap.end())
		return c->second;

	for (size_t i = 0; i < _columns->size(); i++) {
		if (!(*_columns)[i])
			continue;

		if ((*_columns)[i]->getUint(kGFF4G2DAColumnHash) == hash) {
			_columnHashMap[hash] = kGFF4G2DAColumn1 + i;

			return kGFF4G2DAColumn1 + i;
		}
	}

	_columnHashMap[hash] = kInvalidColumn;
	return kInvalidColumn;
}

const GFF4Struct *GDAFile::getRowColumn(size_t row, uint32 hash, size_t &column) const {
	const GFF4Struct *gdaRow = getRow(row);
	if (!gdaRow || ((column = findColumn(hash)) == kInvalidColumn))
		return 0;

	return gdaRow;
}

const GFF4Struct *GDAFile::getRowColumn(size_t row, const Common::UString &name, size_t &column) const {
	const GFF4Struct *gdaRow = getRow(row);
	if (!gdaRow || ((column = findColumn(name)) == kInvalidColumn))
		return 0;

	return gdaRow;
}

Common::UString GDAFile::getString(size_t row, uint32 columnHash, const Common::UString &def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnHash, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getString(gdaColumn, def);
}

Common::UString GDAFile::getString(size_t row, const Common::UString &columnName,
                                   const Common::UString &def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnName, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getString(gdaColumn, def);
}

int32 GDAFile::getInt(size_t row, uint32 columnHash, int32 def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnHash, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getSint(gdaColumn, def);
}

int32 GDAFile::getInt(size_t row, const Common::UString &columnName, int32 def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnName, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getSint(gdaColumn, def);
}

float GDAFile::getFloat(size_t row, uint32 columnHash, float def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnHash, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getDouble(gdaColumn, def);
}

float GDAFile::getFloat(size_t row, const Common::UString &columnName, float def) const {
	size_t gdaColumn;
	const GFF4Struct *gdaRow = getRowColumn(row, columnName, gdaColumn);
	if (!gdaRow)
		return def;

	return gdaRow->getDouble(gdaColumn, def);
}

GDAFile::Type GDAFile::identifyType(const Columns &columns, const Row &rows, size_t column) const {
	if (!columns || (column >= columns->size()) || !(*columns)[column])
		return kTypeEmpty;

	if ((*columns)[column]->hasField(kGFF4G2DAColumnType)) {
		const Type type = (Type) (*columns)[column]->getUint(kGFF4G2DAColumnType, -1);

		switch (type) {
			case kTypeEmpty:
			case kTypeString:
			case kTypeInt:
			case kTypeFloat:
			case kTypeBool:
			case kTypeResource:
				break;

			default:
				throw Common::Exception("Invalid GDA column type %d", (int) type);
		}

		return type;
	}

	if (!rows || rows->empty() || !(*rows)[0])
		return kTypeEmpty;

	GFF4Struct::FieldType fieldType = (*rows)[0]->getFieldType(kGFF4G2DAColumn1 + column);

	switch (fieldType) {
		case GFF4Struct::kFieldTypeString:
		case GFF4Struct::kFieldTypeASCIIString:
			return kTypeString;

		case GFF4Struct::kFieldTypeUint8:
		case GFF4Struct::kFieldTypeUint16:
		case GFF4Struct::kFieldTypeUint32:
		case GFF4Struct::kFieldTypeUint64:
		case GFF4Struct::kFieldTypeSint8:
		case GFF4Struct::kFieldTypeSint16:
		case GFF4Struct::kFieldTypeSint32:
		case GFF4Struct::kFieldTypeSint64:
			return kTypeInt;

		case GFF4Struct::kFieldTypeFloat32:
		case GFF4Struct::kFieldTypeFloat64:
			return kTypeFloat;

		default:
			break;
	}

	return kTypeEmpty;
}

void GDAFile::load(Common::SeekableReadStream *gda) {
	try {
		_gff4s.push_back(new GFF4File(gda, kG2DAID));

		const uint32 version = _gff4s.back()->getTypeVersion();
		if ((version != kVersion01) && (version != kVersion02))
			throw Common::Exception("Unsupported GDA file version %s", Common::debugTag(version).c_str());

		const GFF4Struct &top = _gff4s.back()->getTopLevel();

		_columns = &top.getList(kGFF4G2DAColumnList);
		_rows.push_back(&top.getList(kGFF4G2DARowList));

		_rowStarts.push_back(_rowCount);
		_rowCount += _rows.back()->size();

		_headers.resize(_columns->size());
		for (size_t i = 0; i < _columns->size(); i++) {
			if (!(*_columns)[i])
				continue;

			_headers[i].hash  = (uint32) (*_columns)[i]->getUint(kGFF4G2DAColumnHash);
			_headers[i].type  =          identifyType(_columns, _rows.back(), i);
			_headers[i].field = (uint32) kGFF4G2DAColumn1 + i;
		}

	} catch (Common::Exception &e) {
		e.add("Failed reading GDA file");
		throw;
	}
}

void GDAFile::add(Common::SeekableReadStream *gda) {
	try {
		_gff4s.push_back(new GFF4File(gda, kG2DAID));

		const uint32 version = _gff4s.back()->getTypeVersion();
		if ((version != kVersion01) && (version != kVersion02))
			throw Common::Exception("Unsupported GDA file version %s", Common::debugTag(version).c_str());

		const GFF4Struct &top = _gff4s.back()->getTopLevel();

		_rows.push_back(&top.getList(kGFF4G2DARowList));

		_rowStarts.push_back(_rowCount);
		_rowCount += _rows.back()->size();

		Columns columns = &top.getList(kGFF4G2DAColumnList);
		if (columns->size() != _columns->size())
			throw Common::Exception("Column counts don't match (%u vs. %u)",
			                        (uint)columns->size(), (uint)_columns->size());

		for (size_t i = 0; i < columns->size(); i++) {
			const uint32 hash1 = (uint32) (* columns)[i]->getUint(kGFF4G2DAColumnHash);
			const uint32 hash2 = (uint32) (*_columns)[i]->getUint(kGFF4G2DAColumnHash);

			const Type type1 = identifyType( columns, _rows.back(), i);
			const Type type2 = identifyType(_columns, _rows[0]    , i);

			if ((hash1 != hash2) || (type1 != type2))
				throw Common::Exception("Columns don't match (%u: %u+%d vs. %u+%d)", (uint) i,
				                        hash1, (int)type1, hash2, (int)type2);
		}

	} catch (Common::Exception &e) {
		e.add("Failed adding GDA file");
		throw;
	}
}

} // End of namespace Aurora

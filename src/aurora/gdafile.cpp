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

#include "src/aurora/gdafile.h"
#include "src/aurora/gff4file.h"

static const uint32 kG2DAID = MKTAG('G', '2', 'D', 'A');

namespace Aurora {

GDAFile::GDAFile(Common::SeekableReadStream *gda) : _columns(0), _rowCount(0) {
	load(gda);
}

GDAFile::~GDAFile() {
	clear();
}

size_t GDAFile::getColumnCount() const {
	return _columns->size();
}

size_t GDAFile::getRowCount() const {
	return _rowCount;
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

uint32 GDAFile::identifyType(const Columns &columns, const Row &rows, size_t column) const {
	if (!columns || (column >= columns->size()) || !(*columns)[column])
		return -1;

	if ((*columns)[column]->hasField(kGFF4G2DAColumnType))
		return (uint32) (*columns)[column]->getUint(kGFF4G2DAColumnType, -1);

	if (!rows || rows->empty() || !(*rows)[0])
		return -1;

	GFF4Struct::FieldType fieldType = (*rows)[0]->getFieldType(kGFF4G2DAColumn1 + column);

	switch (fieldType) {
		case GFF4Struct::kFieldTypeString:
			return 0;

		case GFF4Struct::kFieldTypeUint:
		case GFF4Struct::kFieldTypeSint:
			return 1;

		case GFF4Struct::kFieldTypeDouble:
			return 2;

		default:
			break;
	}

	return -1;
}

void GDAFile::load(Common::SeekableReadStream *gda) {
	try {
		_gff4s.push_back(new GFF4File(gda, kG2DAID));

		const GFF4Struct &top = _gff4s.back()->getTopLevel();

		_columns = &top.getList(kGFF4G2DAColumnList);
		_rows.push_back(&top.getList(kGFF4G2DARowList));

		_rowStarts.push_back(_rowCount);
		_rowCount += _rows.back()->size();

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading GDA file");
		throw;
	}
}

void GDAFile::add(Common::SeekableReadStream *gda) {
	try {
		_gff4s.push_back(new GFF4File(gda, kG2DAID));

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

			const uint32 type1 = identifyType( columns, _rows.back(), i);
			const uint32 type2 = identifyType(_columns, _rows[0]    , i);

			if ((hash1 != hash2) || (type1 != type2))
				throw Common::Exception("Columns don't match (%u: %u+%u vs. %u+%u)", (uint) i,
				                        hash1, type1, hash2, type2);
		}

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed adding GDA file");
		throw;
	}
}

void GDAFile::clear() {
	for (std::vector<GFF4File *>::iterator g = _gff4s.begin(); g != _gff4s.end(); ++g)
		delete *g;

	_gff4s.clear();

	_columns = 0;
	_rows.clear();

	_rowStarts.clear();

	_rowCount = 0;

	_columnHashMap.clear();
	_columnNameMap.clear();
}

} // End of namespace Aurora

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

#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/hash.h"

#include "src/aurora/gdafile.h"
#include "src/aurora/gff4file.h"

static const uint32 kG2DAID = MKTAG('G', '2', 'D', 'A');

namespace Aurora {

GDAFile::GDAFile(Common::SeekableReadStream *gda) : _gff4(0), _columns(0), _rows(0) {
	load(gda);
}

GDAFile::~GDAFile() {
	clear();
}

size_t GDAFile::getColumnCount() const {
	return _columns->size();
}

size_t GDAFile::getRowCount() const {
	return _rows->size();
}

bool GDAFile::hasRow(size_t row) const {
	return getRow(row) != 0;
}

const GFF4Struct *GDAFile::getRow(size_t row) const {
	if (row >= _rows->size())
		return 0;

	return (*_rows)[row];
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

void GDAFile::load(Common::SeekableReadStream *gda) {
	try {
		_gff4 = new GFF4File(gda, kG2DAID);

		const GFF4Struct &top = _gff4->getTopLevel();

		_columns = &top.getList(kGFF4G2DAColumnList);
		_rows    = &top.getList(kGFF4G2DARowList);

	} catch (Common::Exception &e) {
		clear();

		e.add("Failed reading GDA file");
		throw;
	}
}

void GDAFile::clear() {
	delete _gff4;
	_gff4 = 0;

	_columns = 0;
	_rows    = 0;
}

} // End of namespace Aurora

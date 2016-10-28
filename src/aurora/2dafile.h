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

#ifndef AURORA_2DAFILE_H
#define AURORA_2DAFILE_H

#include <vector>
#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/ptrvector.h"

#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
	class WriteStream;
	class StreamTokenizer;
}

namespace Aurora {

class TwoDAFile;
class GDAFile;

/** A row within a 2DA file.
 *
 *  Each row inside a 2DA file contains several cells with string
 *  data, identified by either their column index or column header
 *  string.
 *
 *  For convenience's sake, there are also methods to directly parse
 *  the cell strings into integer or floating point values.
 *
 *  See also class TwoDAFile.
 */
class TwoDARow : boost::noncopyable {
public:
	/** Return the contents of a cell as a string. */
	const Common::UString &getString(size_t column) const;
	/** Return the contents of a cell as a string. */
	const Common::UString &getString(const Common::UString &column) const;

	/** Return the contents of a cell as an int. */
	int32 getInt(size_t column) const;
	/** Return the contents of a cell as an int. */
	int32 getInt(const Common::UString &column) const;

	/** Return the contents of a cell as a float. */
	float getFloat(size_t column) const;
	/** Return the contents of a cell as a float. */
	float getFloat(const Common::UString &column) const;

	/** Check if the cell is empty. */
	bool empty(size_t column) const;
	/** Check if the cell is empty. */
	bool empty(const Common::UString &column) const;

private:
	TwoDAFile *_parent; ///< The parent 2DA.

	std::vector<Common::UString> _data;

	TwoDARow(TwoDAFile &parent);
	~TwoDARow();

	const Common::UString &getCell(size_t n) const;

	friend class TwoDAFile;

	template<typename T>
	friend void Common::DeallocatorDefault::destroy(T *);
};

/** Class to hold the two-dimensional array of a 2DA file.
 *
 *  A 2DA contains a two-dimensional array of string data, where
 *  each cell can be identified by the numerical index of its row
 *  and column. Moreover, each column has a textual "header", a
 *  string uniquely identifying the column by what it's used for.
 *
 *  The usual use-case is to first identify which row to use for
 *  a certain object, item, feat, etc., read this row out of the
 *  2DA, and then read each column cell in that row.
 *
 *  For example: the data file defining an item specifies a "Type"
 *  of 23, which is an index into the row 23 of the 2DA "items.2da".
 *  This 2DA contains the column "Model", "Icon" and "Price", so
 *  the cells in the row 23 contain the model, icon and price of
 *  the item we are looking for.
 *
 *  2DA files exist in two variants: ASCII and binary. The ASCII
 *  version is just a simple text file, formatted to represent a
 *  grid of data, with whitespace separating the cells. It can
 *  be read and modified with a simple text editor. The binary
 *  version cannot.
 *
 *  See also classes TwoDARow and TwoDARegistry.
 */
class TwoDAFile : boost::noncopyable, public AuroraFile {
public:
	TwoDAFile(Common::SeekableReadStream &twoda);
	TwoDAFile(const GDAFile &gda);
	~TwoDAFile();

	/** Return the number of rows in the array. */
	size_t getRowCount() const;

	/** Return the number of columns in the array. */
	size_t getColumnCount() const;

	/** Return the columns' headers. */
	const std::vector<Common::UString> &getHeaders() const;

	/** Translate a column header to a column index. */
	size_t headerToColumn(const Common::UString &header) const;

	/** Get a row. */
	const TwoDARow &getRow(size_t row) const;

	/** Get a row whose value in the column named header is the given string value. */
	const TwoDARow &getRow(const Common::UString &header, const Common::UString &value) const;

	// .--- 2DA file writers
	/** Write the 2DA data into an V2.0 ASCII 2DA. */
	void writeASCII(Common::WriteStream &out) const;
	/** Write the 2DA data into an V2.0 ASCII 2DA. */
	bool writeASCII(const Common::UString &fileName) const;

	/** Write the 2DA data into an V2.b binary 2DA. */
	void writeBinary(Common::WriteStream &out) const;
	/** Write the 2DA data into an V2.b binary 2DA. */
	bool writeBinary(const Common::UString &fileName) const;

	/** Write the 2DA data into a CSV stream. */
	void writeCSV(Common::WriteStream &out) const;
	/** Write the 2DA data into a CSV file. */
	bool writeCSV(const Common::UString &fileName) const;
	// '---

private:
	typedef std::map<Common::UString, size_t, Common::UString::iless> HeaderMap;

	Common::UString _defaultString; ///< The default string to return should a cell not exist.
	int32           _defaultInt;    ///< The default int to return should a cell not exist.
	float           _defaultFloat;  ///< The default float to return should a cell not exist.

	std::vector<Common::UString> _headers;
	HeaderMap _headerMap;

	TwoDARow _emptyRow;
	Common::PtrVector<TwoDARow> _rows;

	// Loading helpers
	void load(Common::SeekableReadStream &twoda);
	void read2a(Common::SeekableReadStream &twoda);
	void read2b(Common::SeekableReadStream &twoda);

	// ASCII loading helpers
	void readDefault2a(Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);
	void readHeaders2a(Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);
	void readRows2a   (Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);

	// Binary loading helpers
	void readHeaders2b (Common::SeekableReadStream &twoda);
	void skipRowNames2b(Common::SeekableReadStream &twoda);
	void readRows2b    (Common::SeekableReadStream &twoda);

	// GDA loading/conversion helpers
	void load(const GDAFile &gda);

	void createHeaderMap();

	static int32 parseInt(const Common::UString &str);
	static float parseFloat(const Common::UString &str);

	friend class TwoDARow;
};

} // End of namespace Aurora

#endif // AURORA_2DAFILE_H

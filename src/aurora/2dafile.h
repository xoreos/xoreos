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

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
	class StreamTokenizer;
}

namespace Aurora {

class TwoDAFile;

class TwoDARow {
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
};

/** Class to hold the two-dimensional array of a 2DA file. */
class TwoDAFile : public AuroraBase {
public:
	TwoDAFile(Common::SeekableReadStream &twoda);
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

	/** Dump the 2DA data into an V2.0 ASCII 2DA. */
	bool dumpASCII(const Common::UString &fileName) const;

private:
	typedef std::map<Common::UString, size_t, Common::UString::iless> HeaderMap;

	Common::UString _defaultString; ///< The default string to return should a cell not exist.
	int32           _defaultInt;    ///< The default int to return should a cell not exist.
	float           _defaultFloat;  ///< The default float to return should a cell not exist.

	std::vector<Common::UString> _headers;
	HeaderMap _headerMap;

	TwoDARow _emptyRow;
	std::vector<TwoDARow *> _rows;

	// Loading helpers
	void load(Common::SeekableReadStream &twoda);
	void read2a(Common::SeekableReadStream &twoda);
	void read2b(Common::SeekableReadStream &twoda);
	void clear();

	// ASCII loading helpers
	void readDefault2a(Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);
	void readHeaders2a(Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);
	void readRows2a   (Common::SeekableReadStream &twoda, Common::StreamTokenizer &tokenize);

	// Binary loading helpers
	void readHeaders2b (Common::SeekableReadStream &twoda);
	void skipRowNames2b(Common::SeekableReadStream &twoda);
	void readRows2b    (Common::SeekableReadStream &twoda);

	void createHeaderMap();

	static int32 parseInt(const Common::UString &str);
	static float parseFloat(const Common::UString &str);

	friend class TwoDARow;
};

} // End of namespace Aurora

#endif // AURORA_2DAFILE_H

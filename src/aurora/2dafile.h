/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/2dafile.h
 *  Handling BioWare's 2DAs (two-dimensional array).
 */

#ifndef AURORA_2DAFILE_H
#define AURORA_2DAFILE_H

#include <string>
#include <vector>
#include <map>

#include "common/types.h"
#include "common/streamtokenizer.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

/** Class to hold the two-dimensional array of a 2DA file. */
class TwoDAFile : public AuroraBase {
public:
	static const int kColumnInvalid = 0xFFFFFFFF;

	typedef std::vector<std::string> Row;

	TwoDAFile();
	~TwoDAFile();

	/** Clear the array. */
	void clear();

	/** Load a 2DA file.
	 *
	 *  @param twoda A stream of an 2DA file.
	 */
	void load(Common::SeekableReadStream &twoda);

	/** Return the number of rows in the array. */
	uint32 rowCount() const;

	/** Return the number of columns in the array. */
	uint32 columnCount() const;

	/** Return the columns' headers. */
	const Row &getHeaders() const;

	/** Translate a column header to a column index. */
	uint32 headerToColumn(const std::string &header) const;

	/** Get a complete row.
	 *
	 *  @param  row The row's index.
	 *  @return A pointer to the row, or 0 if the row does not exist.
	 */
	const Row *getRow(uint32 row) const;

	/** Return the contents of a cell as a string. */
	const std::string &getCellString(uint32 row, uint32 column) const;
	/** Return the contents of a cell as a string. */
	const std::string &getCellString(uint32 row, const std::string &column) const;

	/** Return the contents of a cell as an int. */
	const int32 getCellInt(uint32 row, uint32 column) const;
	/** Return the contents of a cell as an int. */
	const int32 getCellInt(uint32 row, const std::string &column) const;

	/** Return the contents of a cell as a float. */
	const float getCellFloat(uint32 row, uint32 column) const;
	/** Return the contents of a cell as a float. */
	const float getCellFloat(uint32 row, const std::string &column) const;

private:
	typedef std::vector<Row *> Array;

	typedef std::map<std::string, uint32> HeaderMap;

	std::string _defaultString; ///< The default string to return should a cell not exist.
	int32       _defaultInt;    ///< The default int to return should a cell not exist.
	float       _defaultFloat;  ///< The default float to return should a cell not exist.

	Row _headers; ///< The columns' headers.
	Array _array; ///< The array itself.

	/** Map to translate a column header to an index. */
	HeaderMap _headerMap;

	// For tokenizing the ASCII format
	Common::CharList _splitCharsA;
	Common::CharList _endCharsA;
	Common::CharList _quoteCharsA;
	Common::CharList _ignoreCharsA;

	// Loading helpers
	void read2a(Common::SeekableReadStream &twoda);
	void read2b(Common::SeekableReadStream &twoda);

	// ASCII loading helpers
	void readDefault2a(Common::SeekableReadStream &twoda);
	void readHeaders2a(Common::SeekableReadStream &twoda);
	void readRows2a(Common::SeekableReadStream &twoda);

	// Binary loading helpers
	void readHeaders2b(Common::SeekableReadStream &twoda);
	void skipRowNames2b(Common::SeekableReadStream &twoda);
	void readRows2b(Common::SeekableReadStream &twoda);

	void createHeaderMap();

	void tokenize(Common::SeekableReadStream &stream, Row &row,
	              const Common::CharList &splitChars, const Common::CharList &endChars,
	              const Common::CharList &quoteChars, const Common::CharList &ignoreChars,
	              uint32 skip = 0, int n = -1);

	const std::string *getCell(uint32 row, uint32 column) const;

	static int32 parseInt(const std::string &str);
	static float parseFloat(const std::string &str);
};

} // End of namespace Aurora

#endif // AURORA_2DAFILE_H

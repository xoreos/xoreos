/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file common/foxpro.h
 *  A database in FoxPro format.
 */

#ifndef COMMON_FOXPRO_H
#define COMMON_FOXPRO_H

#include <vector>
#include <list>

#include "boost/date_time/gregorian/gregorian_types.hpp"

#include "common/types.h"
#include "common/ustring.h"

namespace Common {

class SeekableReadStream;

class FoxPro {
public:
	/** A field type. */
	enum Type {
		kTypeDouble   = 0x42, // 'B'
		kTypeString   = 0x43, // 'C'
		kTypeDate     = 0x44, // 'D'
		kTypeFloat    = 0x46, // 'F'
		kTypeGeneral  = 0x47, // 'G'
		kTypeInteger  = 0x49, // 'I'
		kTypeBool     = 0x4C, // 'L'
		kTypeMemo     = 0x4D, // 'M'
		kTypeNumber   = 0x4E, // 'N'
		kTypePicture  = 0x50, // 'P'
		kTypeDataTime = 0x54, // 'T'
		kTypeCurrency = 0x59  // 'Y'
	};

	/** A field description. */
	struct Field {
		UString name;

		Type   type;
		uint32 offset;
		uint8  size;
		uint8  decimals;

		byte flags;

		uint32 autoIncNext;
		uint8  autoIncStep;
	};

	/** A record. */
	struct Record {
		bool deleted; ///< Has this record been deleted?
		std::vector<const byte *> fields; ///< Raw field data.
	};

	FoxPro();
	~FoxPro();

	void load(SeekableReadStream *dbf, SeekableReadStream *cdx = 0,
	          SeekableReadStream *fpt = 0);

	boost::gregorian::date getLastUpdate() const;

	bool hasIndex() const;
	bool hasMemo () const;

	uint32 getFieldCount () const;
	uint32 getRecordCount() const;

	const std::vector<Field>  &getFields () const;
	const std::vector<Record> &getRecords() const;

	UString getString(const Record &record, uint32 field) const;
	int32   getInt   (const Record &record, uint32 field) const;
	bool    getBool  (const Record &record, uint32 field) const;

	SeekableReadStream *getMemo(const Record &record, uint32 field) const;

private:
	boost::gregorian::date _lastUpdate;

	bool _hasIndex;
	bool _hasMemo;

	std::vector<Field>  _fields;
	std::vector<Record> _records;

	std::list<byte *> _pool;

	uint16 _memoBlockSize;
	std::vector<byte *> _memos;

	static bool getInt(const byte *data, uint32 size, int32 &i);
};

} // End of namespace Common

#endif // COMMON_FOXPRO_H

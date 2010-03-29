/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#ifndef AURORA_GFFFILE_H
#define AURORA_GFFFILE_H

#include <vector>

#include "common/types.h"

#include "aurora/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class LocString;

class GFFField {
public:
	enum Type {
		kTypeNone      = 0,
		kTypeChar      = 1,
		kTypeUint      = 2,
		kTypeSint      = 3,
		kTypeString    = 4,
		kTypeLocString = 5,
		kTypeData      = 6,
		kTypeStruct    = 7,
		kTypeList      = 8
	};

	typedef std::vector<GFFField *> GFFStruct;
	typedef std::vector<GFFStruct> GFFList;

	GFFField();
	~GFFField();

	void clear();

	Type getType() const;

	const std::string &getName() const;

	char   getChar() const;
	uint64 getUnsignedInteger() const;
	int64  getSignedInteger()   const;

	double getDouble() const;

	const std::string &getString()    const;
	const LocString   &getLocString() const;

	const GFFStruct &getStruct() const;
	const GFFList   &getList()   const;

private:
	enum GFFType {
		kGFFTypeNone      = - 1,
		kGFFTypeByte      =   0,
		kGFFTypeChar      =   1,
		kGFFTypeUint16    =   2,
		kGFFTypeInt16     =   3,
		kGFFTypeUint32    =   4,
		kGFFTypeInt32     =   5,
		kGFFTypeUint64    =   6,
		kGFFTypeInt64     =   7,
		kGFFTypeFloat     =   8,
		kGFFTypeDouble    =   9,
		kGFFTypeExoString =  10,
		kGFFTypeResRef    =  11, ///< String, max. 16 characters.
		kGFFTypeLocString =  12, ///< Localized string.
		kGFFTypeVoid      =  13, ///< Random data of variable length.
		kGFFTypeStruct    =  14, ///< Struct containing a number of fields.
		kGFFTypeList      =  15  ///< List containing a number of structs.
	};

	GFFType _gffType;
	   Type _type;

	std::string _name;

	union {
		uint64 typeInt;
		double typeDouble;;

		std::string *typeString;
		LocString   *typeLocString;

		GFFStruct *typeStruct;
		GFFList   *typeList;
	} _value;
};

} // End of namespace Aurora

#endif // AURORA_GFFFILE_H

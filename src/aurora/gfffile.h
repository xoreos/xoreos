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

struct GFFHeader {
	uint32 id;
	uint32 version;
	uint32 structOffset;
	uint32 structCount;
	uint32 fieldOffset;
	uint32 fieldCount;
	uint32 labelOffset;
	uint32 labelCount;
	uint32 fieldDataOffset;
	uint32 fieldDataCount;
	uint32 fieldIndicesOffset;
	uint32 fieldIndicesCount;
	uint32 listIndicesOffset;
	uint32 listIndicesCount;

	GFFHeader();

	void clear();

	bool read(Common::SeekableReadStream &gff);
};

class GFFField {
public:
	enum Type {
		kTypeNone        = 0,
		kTypeChar        = 1,
		kTypeUint        = 2,
		kTypeSint        = 3,
		kTypeDouble      = 4,
		kTypeString      = 5,
		kTypeLocString   = 6,
		kTypeData        = 7,
		kTypeStruct      = 8,
		kTypeList        = 9,
		kTypeOrientation = 10,
		kTypeVector      = 11
	};

	GFFField();
	~GFFField();

	void clear();

	Type getType() const;

	const std::string &getLabel() const;

	char   getChar() const;
	uint64 getUint() const;
	int64  getSint() const;

	double getDouble() const;

	const std::string &getString()    const;
	const LocString   &getLocString() const;

	uint32 getDataSize() const;
	const byte *getData() const;

	const float *getVector() const;

	const uint32 getIndex() const;

	bool read(Common::SeekableReadStream &gff, const GFFHeader &header);

private:
	enum GFFType {
		kGFFTypeNone        = - 1,
		kGFFTypeByte        =   0,
		kGFFTypeChar        =   1,
		kGFFTypeUint16      =   2,
		kGFFTypeSint16      =   3,
		kGFFTypeUint32      =   4,
		kGFFTypeSint32      =   5,
		kGFFTypeUint64      =   6,
		kGFFTypeSint64      =   7,
		kGFFTypeFloat       =   8,
		kGFFTypeDouble      =   9,
		kGFFTypeExoString   =  10,
		kGFFTypeResRef      =  11, ///< String, max. 16 characters.
		kGFFTypeLocString   =  12, ///< Localized string.
		kGFFTypeVoid        =  13, ///< Random data of variable length.
		kGFFTypeStruct      =  14, ///< Struct containing a number of fields.
		kGFFTypeList        =  15, ///< List containing a number of structs.
		kGFFTypeOrientation =  16, // TODO: New in KotOR
		kGFFTypeVector      =  17, ///< A vector of 3 floats
		kGFFTypeStrRef      =  18  // TODO: New in Jade Empire
	};

	GFFType _gffType;
	   Type _type;

	std::string _label;

	uint32 _dataSize;

	union {
		uint64 typeInt;
		double typeDouble;

		std::string *typeString;
		LocString   *typeLocString;

		byte *typeData;

		float typeVector[3];

		uint32 typeIndex;
	} _value;

	bool convertData(Common::SeekableReadStream &gff, const GFFHeader &header, uint32 data);

	inline bool readUint64   (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readSint64   (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readDouble   (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readExoString(Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readResRef   (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readLocString(Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readVoid     (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);
	inline bool readVector   (Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data);

	inline bool seekGFFData(Common::SeekableReadStream &gff,
			const GFFHeader &header, uint32 data, uint32 &curPos);

	static inline Type toType(GFFType type);
};

class GFFFile {
public:
	GFFFile();
	~GFFFile();

	void clear();

	bool load(Common::SeekableReadStream &gff);

private:
	typedef std::vector<GFFField>  GFFStruct;
	typedef std::vector<GFFStruct> GFFStructArray;

	typedef std::vector<uint32> GFFListArray;

	GFFHeader _header;

	GFFStructArray _structArray;
	GFFListArray   _listArray;

	bool readField(Common::SeekableReadStream &gff, GFFField &field, uint32 fieldIndex);
	bool readFields(Common::SeekableReadStream &gff, GFFStruct &strct, uint32 fieldIndicesIndex);
};

} // End of namespace Aurora

#endif // AURORA_GFFFILE_H

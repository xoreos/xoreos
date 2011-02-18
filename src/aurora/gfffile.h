/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gfffile.h
 *  Handling BioWare's GFFs (generic file format).
 */

#ifndef AURORA_GFFFILE_H
#define AURORA_GFFFILE_H

#include <vector>
#include <list>
#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class LocString;
class GFFStruct;

typedef std::list<GFFStruct *> GFFList;

class GFFFile : public AuroraBase {
public:
	GFFFile();
	~GFFFile();

	/** Clear all information. */
	void clear();

	/** Load a GFF file.
	 *
	 *  @param gff A gff of an GFF file.
	 */
	void load(Common::SeekableReadStream &gff);

	/** Returns the top-level struct. */
	const GFFStruct &getTopLevel() const;

private:
	/** A GFF header. */
	struct Header {
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

		Header();

		/** Clear the header. */
		void clear();

		/** Read the header out of a gff. */
		void read(Common::SeekableReadStream &gff);
	};

	typedef std::vector<GFFStruct *> StructArray;
	typedef std::vector<GFFList> ListArray;

	Header _header; ///< The GFF's header

	StructArray _structs; ///< Our structs.
	ListArray   _lists;   ///< Our lists.

	/** To convert list offsets found in GFF to real indices. */
	std::vector<uint32> _listOffsetToIndex;

	byte *_fieldData; ///< The extended field data.

	/** Returns the struct field data at this offset. */
	const byte *getFieldData(uint32 offset) const;

	/** Return a struct within the GFF. */
	const GFFStruct &getStruct(uint32 i) const;
	/** Return a list within the GFF. */
	const GFFList   &getList  (uint32 i) const;

	// Loading helpers
	void readStructs(Common::SeekableReadStream &gff);
	void readLists(Common::SeekableReadStream &gff);
	void readFieldData(Common::SeekableReadStream &gff);

	friend class GFFStruct;
};

/** A struct within a GFF. */
class GFFStruct {
public:
	uint getFieldCount() const;

	bool hasField(const Common::UString &field) const;

	char   getChar(const Common::UString &field, char   def = '\0') const;
	uint64 getUint(const Common::UString &field, uint64 def = 0   ) const;
	 int64 getSint(const Common::UString &field,  int64 def = 0   ) const;

	double getDouble(const Common::UString &field, double def = 0.0) const;

	Common::UString getString(const Common::UString &field,
	                          const Common::UString &def = "") const;

	void getLocString(const Common::UString &field, LocString &str) const;

	Common::SeekableReadStream *getData(const Common::UString &field) const;

	void getVector     (const Common::UString &field,
			float &x, float &y, float &z          ) const;
	void getOrientation(const Common::UString &field,
			float &a, float &b, float &c, float &d) const;

	void getVector     (const Common::UString &field,
			double &x, double &y, double &z           ) const;
	void getOrientation(const Common::UString &field,
			double &a, double &b, double &c, double &d) const;

	const GFFStruct &getStruct(const Common::UString &field) const;
	const GFFList   &getList  (const Common::UString &field) const;

private:
	/** The type of a GFF field. */
	enum FieldType {
		kFieldTypeNone        = - 1, ///< Invalid type.
		kFieldTypeByte        =   0, ///< A single byte.
		kFieldTypeChar        =   1, ///< A single character.
		kFieldTypeUint16      =   2, ///< Unsigned 16bit integer.
		kFieldTypeSint16      =   3, ///< Signed 16bit integer.
		kFieldTypeUint32      =   4, ///< Unsigned 32bit integer.
		kFieldTypeSint32      =   5, ///< Signed 32bit integer.
		kFieldTypeUint64      =   6, ///< Unsigned 64bit integer.
		kFieldTypeSint64      =   7, ///< Signed 64bit integer.
		kFieldTypeFloat       =   8, ///< IEEE float.
		kFieldTypeDouble      =   9, ///< IEEE double.
		kFieldTypeExoString   =  10, ///< String.
		kFieldTypeResRef      =  11, ///< String, max. 16 characters.
		kFieldTypeLocString   =  12, ///< Localized string.
		kFieldTypeVoid        =  13, ///< Random data of variable length.
		kFieldTypeStruct      =  14, ///< Struct containing a number of fields.
		kFieldTypeList        =  15, ///< List containing a number of structs.
		kFieldTypeOrientation =  16, ///< An object orientation.
		kFieldTypeVector      =  17, ///< A vector of 3 floats.
		kFieldTypeStrRef      =  18  // TODO: New in Jade Empire
	};

	/** A GFF field. */
	struct Field {
		FieldType type;     ///< Type of the field.
		uint32    data;     ///< Data of the field.
		bool      extended; ///< Does this field need extended data?

		Field();
		Field(FieldType t, uint32 d);
	};

	typedef std::map<Common::UString, Field> FieldMap;

	const GFFFile *_parent; ///< The parent GFF.

	uint32 _id; ///< The struct's ID.
	FieldMap _fields; ///< The fields, indexed by their label.

	GFFStruct(const GFFFile &parent, Common::SeekableReadStream &gff);
	~GFFStruct();

	/** Returns the field with this tag. */
	const Field *getField(const Common::UString &name) const;
	/** Returns the extended field data for this field. */
	const byte *getData(const Field &field) const;

	// Loading helpers
	void readField  (Common::SeekableReadStream &gff, uint32 index);
	void readFields (Common::SeekableReadStream &gff, uint32 index, uint32 count);
	void readIndices(Common::SeekableReadStream &gff,
	                 std::vector<uint32> &indices, uint32 count);
	Common::UString readLabel(Common::SeekableReadStream &gff, uint32 index);

	friend class GFFFile;
};

} // End of namespace Aurora

#endif // AURORA_GFFFILE_H

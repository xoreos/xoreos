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
 *  Handling version V3.2/V3.3 of BioWare's GFFs (generic file format).
 */

#ifndef AURORA_GFF3FILE_H
#define AURORA_GFF3FILE_H

#include <vector>
#include <map>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class LocString;
class GFF3Struct;

/** A GFF (generic file format) V3.2/V3.3 file, found in all Aurora games
 *  except Sonic Chronicles: The Dark Brotherhood. Even games that have
 *  V4.0/V4.1 GFFs additionally use V3.2/V3.3 files as well.
 */
class GFF3File : public AuroraBase {
public:
	GFF3File(Common::SeekableReadStream *gff3, uint32 id = 0xFFFFFFFF);
	GFF3File(const Common::UString &gff3, FileType type, uint32 id);
	~GFF3File();

	/** Return the GFF3's specific type. */
	uint32 getType() const;

	/** Returns the top-level struct. */
	const GFF3Struct &getTopLevel() const;


private:
	/** A GFF3 header. */
	struct Header {
		uint32 structOffset;       ///< Offset to the struct definitions.
		uint32 structCount;        ///< Number of structs.
		uint32 fieldOffset;        ///< Offset to the field definitions.
		uint32 fieldCount;         ///< Number of fields.
		uint32 labelOffset;        ///< Offset to the field labels.
		uint32 labelCount;         ///< Number of labels.
		uint32 fieldDataOffset;    ///< Offset to the field data.
		uint32 fieldDataCount;     ///< Number of field data fields.
		uint32 fieldIndicesOffset; ///< Offset to the field indices.
		uint32 fieldIndicesCount;  ///< Number of field indices.
		uint32 listIndicesOffset;  ///< Offset to the list indices.
		uint32 listIndicesCount;   ///< Number of list indices.

		Header();

		void read(Common::SeekableReadStream &gff3);
	};

	typedef std::vector<GFF3Struct *> StructArray;
	typedef std::vector<GFF3List> ListArray;


	Common::SeekableReadStream *_stream;

	Header _header; ///< The GFF's header

	StructArray _structs; ///< Our structs.
	ListArray   _lists;   ///< Our lists.

	/** To convert list offsets found in GFF to real indices. */
	std::vector<uint32> _listOffsetToIndex;


	// .--- Loading helpers
	void load(uint32 id);
	void loadHeader(uint32 id);
	void loadStructs();
	void loadLists();

	void clear();
	// '---

	// .--- Helper methods called by GFF3Struct
	/** Return the GFF stream. */
	Common::SeekableReadStream &getStream(uint32 offset) const;
	/** Return the GFF stream seeked to the start of the field data. */
	Common::SeekableReadStream &getFieldData() const;

	/** Return a struct within the GFF. */
	const GFF3Struct &getStruct(uint32 i) const;
	/** Return a list within the GFF. */
	const GFF3List   &getList  (uint32 i) const;
	// '---

	friend class GFF3Struct;
};

/** A struct within a GFF. */
class GFF3Struct {
public:
	/** Return the struct's ID. */
	uint32 getID() const;

	/** Return the number of fields in this struct. */
	size_t getFieldCount() const;
	/** Does this specific field exist? */
	bool hasField(const Common::UString &field) const;

	// .--- Read field values
	char   getChar(const Common::UString &field, char   def = '\0' ) const;
	uint64 getUint(const Common::UString &field, uint64 def = 0    ) const;
	 int64 getSint(const Common::UString &field,  int64 def = 0    ) const;
	bool   getBool(const Common::UString &field, bool   def = false) const;

	double getDouble(const Common::UString &field, double def = 0.0) const;

	Common::UString getString(const Common::UString &field,
	                          const Common::UString &def = "") const;

	void getLocString(const Common::UString &field, LocString &str) const;

	void getVector     (const Common::UString &field,
	                    float &x, float &y, float &z          ) const;
	void getOrientation(const Common::UString &field,
	                    float &a, float &b, float &c, float &d) const;

	void getVector     (const Common::UString &field,
	                    double &x, double &y, double &z           ) const;
	void getOrientation(const Common::UString &field,
	                    double &a, double &b, double &c, double &d) const;

	Common::SeekableReadStream *getData(const Common::UString &field) const;
	// '---

	// .--- Structs and lists of structs
	const GFF3Struct &getStruct(const Common::UString &field) const;
	const GFF3List   &getList  (const Common::UString &field) const;
	// '---

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

	/** A field in the GFF3 struct. */
	struct Field {
		FieldType type;     ///< Type of the field.
		uint32    data;     ///< Data of the field.
		bool      extended; ///< Does this field need extended data?

		Field();
		Field(FieldType t, uint32 d);
	};

	typedef std::map<Common::UString, Field> FieldMap;


	const GFF3File *_parent; ///< The parent GFF.

	uint32 _id;         ///< The struct's ID.
	uint32 _fieldIndex; ///< Field / Field indices index.
	uint32 _fieldCount; ///< Field count.

	FieldMap _fields; ///< The fields, indexed by their label.


	// .--- Loader
	GFF3Struct(const GFF3File &parent, uint32 offset);
	~GFF3Struct();

	void load(uint32 offset);

	void readField  (Common::SeekableReadStream &data, uint32 index);
	void readFields (Common::SeekableReadStream &data, uint32 index, uint32 count);
	void readIndices(Common::SeekableReadStream &data,
	                 std::vector<uint32> &indices, uint32 count) const;

	Common::UString readLabel(Common::SeekableReadStream &data, uint32 index) const;
	// '---

	// .--- Field and field data accessors
	/** Returns the field with this tag. */
	const Field *getField(const Common::UString &name) const;
	/** Returns the extended field data for this field. */
	Common::SeekableReadStream &getData(const Field &field) const;
	// '---

	friend class GFF3File;
};

} // End of namespace Aurora

#endif // AURORA_GFF3FILE_H

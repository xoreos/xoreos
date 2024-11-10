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
#include <memory>

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
 *
 *  GFF files store hierarchical data, similar in concept to XML. They are
 *  used whenever such data is useful: to, for example, hold area and object
 *  descriptions, module and campaign specifications or conversations. They
 *  consist of a top-level struct, with a collection of fields of various
 *  types, indexed by a human-readable string name. A field can then be
 *  another struct (which itself will be a collection of fields) or a
 *  list of structs, leading to a recursive, hierarchical structure.
 *
 *  GFF V3.2/V3.3 files come in a multitude of types (ARE, DLG, ...), each
 *  with its own 4-byte type ID ('ARE ', 'DLG ', ...). When specified in
 *  the GFF3File constructor, the loader will enforce that it matches, and
 *  throw an exception should it not. Conversely, an ID of 0xFFFFFFFF means
 *  that no such type ID enforcement should be done. In both cases, the type
 *  ID read from the file can get access through getType().
 *
 *  The GFF V3.2/V3.3 files found in the encrypted premium module archives
 *  of Neverwinter Nights are deliberately broken in various way. When the
 *  constructor parameter repairNWNPremium is set to true, GFF3File will
 *  detect such broken files and automatically repair them. When this
 *  parameter is set to false, no detection will take place, and these
 *  broken files will lead the loader to throw an exception.
 *
 *  There is no functional difference between GFF V3.2 and V3.3 files. GFF
 *  V3.3 files exclusively appear in The Witcher (and every GFF file there
 *  is of version V3.3), simply to denote that the language table used for
 *  LocStrings is different. Since xoreos has more flexible handling of
 *  language IDs anyway, this doesn't concern us.
 *
 *  See also: GFF4File in gff4file.h for the later V4.0/V4.1 versions of
 *  the GFF format.
 */
class GFF3File : public AuroraFile {
public:
	/** Take over this stream and read a GFF3 file out of it. */
	GFF3File(Common::SeekableReadStream *gff3, uint32_t id = 0xFFFFFFFF, bool repairNWNPremium = false);
	/** Request this resource from the ResourceManager and read a GFF3 file out of it. */
	GFF3File(const Common::UString &gff3, FileType type, uint32_t id = 0xFFFFFFFF, bool repairNWNPremium = false);
	virtual ~GFF3File();

	GFF3File(const GFF3File &) = delete;
	GFF3File &operator=(const GFF3File &) = delete;

	/** Return the GFF3's specific type. */
	uint32_t getType() const;

	/** Returns the top-level struct. */
	const GFF3Struct &getTopLevel() const;


private:
	/** A GFF3 header. */
	struct Header {
		uint32_t structOffset;       ///< Offset to the struct definitions.
		uint32_t structCount;        ///< Number of structs.
		uint32_t fieldOffset;        ///< Offset to the field definitions.
		uint32_t fieldCount;         ///< Number of fields.
		uint32_t labelOffset;        ///< Offset to the field labels.
		uint32_t labelCount;         ///< Number of labels.
		uint32_t fieldDataOffset;    ///< Offset to the field data.
		uint32_t fieldDataCount;     ///< Number of field data fields.
		uint32_t fieldIndicesOffset; ///< Offset to the field indices.
		uint32_t fieldIndicesCount;  ///< Number of field indices.
		uint32_t listIndicesOffset;  ///< Offset to the list indices.
		uint32_t listIndicesCount;   ///< Number of list indices.

		Header();

		void read(Common::SeekableReadStream &gff3);
	};

	typedef std::vector<std::unique_ptr<GFF3Struct>> StructArray;
	typedef std::vector<GFF3List> ListArray;


	std::unique_ptr<Common::SeekableReadStream> _stream;

	Header _header; ///< The GFF3's header.

	/** Should we try to read GFF3 files found in Neverwinter Nights premium modules? */
	bool   _repairNWNPremium;
	/** The correctional value for offsets to repair Neverwinter Nights premium modules. */
	uint32_t _offsetCorrection;

	StructArray _structs; ///< Our structs.
	ListArray   _lists;   ///< Our lists.

	/** To convert list offsets found in GFF3 to real indices. */
	std::vector<uint32_t> _listOffsetToIndex;


	// .--- Loading helpers
	void load(uint32_t id);
	void loadHeader(uint32_t id);
	void loadStructs();
	void loadLists();
	// '---

	// .--- Helper methods called by GFF3Struct
	/** Return the GFF3 stream. */
	Common::SeekableReadStream &getStream(uint32_t offset) const;
	/** Return the GFF3 stream seeked to the start of the field data. */
	Common::SeekableReadStream &getFieldData() const;

	/** Return a struct within the GFF3. */
	const GFF3Struct &getStruct(uint32_t i) const;
	/** Return a list within the GFF3. */
	const GFF3List   &getList  (uint32_t i) const;
	// '---

	friend class GFF3Struct;
};

/** A struct within a GFF3. */
class GFF3Struct {
public:
	/** The type of a GFF3 field. */
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
		kFieldTypeResRef      =  11, ///< Resource reference, string.
		kFieldTypeLocString   =  12, ///< Localized string.
		kFieldTypeVoid        =  13, ///< Random data of variable length.
		kFieldTypeStruct      =  14, ///< Struct containing a number of fields.
		kFieldTypeList        =  15, ///< List containing a number of structs.
		kFieldTypeOrientation =  16, ///< An object orientation.
		kFieldTypeVector      =  17, ///< A vector of 3 floats.
		kFieldTypeStrRef      =  18  ///< String reference, index into a talk table.
	};

	/** Return the struct's ID.
	 *
	 *  The ID is a (non-unique) number that's saved in the GFF3 file.
	 *  It's sometimes used to identify the higher-level meaning of a struct
	 *  within a GFF3.
	 *
	 *  The purpose of the ID in a GFF3 struct is comparable to the label in
	 *  a GFF4 struct.
	 */
	uint32_t getID() const;

	/** Return the number of fields in this struct. */
	size_t getFieldCount() const;
	/** Does this specific field exist? */
	bool hasField(const Common::UString &field) const;

	/** Return a list of all field names in this struct. */
	const std::vector<Common::UString> &getFieldNames() const;

	/** Return the type of this field, or kFieldTypeNone if such a field doesn't exist. */
	FieldType getFieldType(const Common::UString &field) const;


	// .--- Read field values
	char   getChar(const Common::UString &field, char   def = '\0' ) const;
	uint64_t getUint(const Common::UString &field, uint64_t def = 0    ) const;
	 int64_t getSint(const Common::UString &field,  int64_t def = 0    ) const;
	bool   getBool(const Common::UString &field, bool   def = false) const;

	double getDouble(const Common::UString &field, double def = 0.0) const;

	Common::UString getString(const Common::UString &field,
	                          const Common::UString &def = "") const;

	bool getLocString(const Common::UString &field, LocString &str) const;

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
	/** A field in the GFF3 struct. */
	struct Field {
		FieldType type;     ///< Type of the field.
		uint32_t  data;     ///< Data of the field.
		bool      extended; ///< Does this field need extended data?

		Field();
		Field(FieldType t, uint32_t d);
	};

	typedef std::map<Common::UString, Field> FieldMap;


	const GFF3File *_parent; ///< The parent GFF3.

	uint32_t _id;         ///< The struct's ID.
	uint32_t _fieldIndex; ///< Field / Field indices index.
	uint32_t _fieldCount; ///< Field count.

	FieldMap _fields; ///< The fields, indexed by their label.

	/** The names of all fields in this struct. */
	std::vector<Common::UString> _fieldNames;


	// .--- Loader
	GFF3Struct(const GFF3File &parent, uint32_t offset);

	void load(uint32_t offset);

	void readField  (Common::SeekableReadStream &data, uint32_t index);
	void readFields (Common::SeekableReadStream &data, uint32_t index, uint32_t count);
	void readIndices(Common::SeekableReadStream &data,
	                 std::vector<uint32_t> &indices, uint32_t count) const;

	Common::UString readLabel(Common::SeekableReadStream &data, uint32_t index) const;
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

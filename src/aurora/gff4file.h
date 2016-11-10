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
 *  Handling version V4.0/V4.1 of BioWare's GFFs (generic file format).
 */

#ifndef AURORA_GFF4FILE_H
#define AURORA_GFF4FILE_H

#include <vector>
#include <map>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/gff4fields.h"

namespace Common {
	class SeekableReadStream;
	class Matrix4x4;
}

namespace Aurora {

class GFF4Struct;

/** A GFF (generic file format) V4.0/V4.1 file, found in Dragon Age: Origins,
 *  Dragon Age 2 and Sonic Chronicles: The Dark Brotherhood.
 *
 *  Just like GFF V3.2/V3.3 files, GFF V4.0/V4.1 store hierarchical data,
 *  similar in concept to XML and hold, for example, module and campaign
 *  descriptions. Unlike version 3 of the format, version 4 is optimized
 *  for access speed, with fields indexed by a numerical value instead of
 *  a human-readable string. A collection of currently known field values
 *  and their meanings can be found in gff4fields.h.
 *
 *  Unlike GFF3, GFF4 has a new concept, the generic. A generic is a field
 *  of variable type. I.e. a field can contain any other type. A list of
 *  generics can even contain different types at different indices.
 *
 *  GFF V4.0/V4.1 files come in a multitude of types (ARE, DLG, ...), each
 *  with its own 4-byte type ID ('ARE ', 'DLG ', ...). When specified in
 *  the GFF4File constructor, the loader will enforce that it matches, and
 *  throw an exception should it not. Conversely, an ID of 0xFFFFFFFF means
 *  that no such type ID enforcement should be done. In both cases, the type
 *  ID read from the file can get access through getType().
 *
 *  GFF V4.1 files exclusively appear in Dragon Age 2 (but Dragon Age 2 also
 *  has GFF V4.0 files). The only difference between these versions is that
 *  V4.1 has support for a global string table and every string field can
 *  reference a string within this table, so that duplicated strings don't
 *  need to be stored multiple times.
 *
 *  Notes:
 *  - Generics and lists of generics are mapped to structs, with the field ID
 *    being the list element indices (or just 0 on non-list generics).
 *  - Strings are generally encoded in UTF-16LE. One exception is the TLK files
 *    in Sonic, which have strings in a language-specific encoding. For example,
 *    the English, French, Italian, German and Spanish (EFIGS) versions have
 *    the strings in TLK files encoded in Windows CP-1252.
 *
 *  See also: GFF3File in gff3file.h for the earlier V3.2/V3.3 versions of
 *  the GFF format.
 */
class GFF4File : boost::noncopyable, public AuroraFile {
public:
	/** Take over this stream and read a GFF4 file out of it. */
	GFF4File(Common::SeekableReadStream *gff4, uint32 type = 0xFFFFFFFF);
	/** Request this resource from the ResourceManager and read a GFF4 file out of it. */
	GFF4File(const Common::UString &gff4, FileType fileType, uint32 type = 0xFFFFFFFF);
	~GFF4File();

	/** Return the GFF4's specific type. */
	uint32 getType() const;
	/** Return the GFF4's specific type version. */
	uint32 getTypeVersion() const;
	/** Return the platform this GFF4 is for. */
	uint32 getPlatform() const;

	/** Returns the top-level struct. */
	const GFF4Struct &getTopLevel() const;


private:
	/** A GFF4 header. */
	struct Header {
		uint32 platformID;   ///< ID of the platform this GFF4 is for.
		uint32 type;         ///< The specific type this GFF4 describes.
		uint32 typeVersion;  ///< The version of the specific type this GFF4 describes.
		uint32 structCount;  ///< Number of struct templates in this GFF4.
		uint32 stringCount;  ///< Number of shared strings (V4.1 only).
		uint32 stringOffset; ///< Offset to shared strings (V4.1 only).
		uint32 dataOffset;   ///< Offset to the data portion.

		bool hasSharedStrings;

		void read(Common::SeekableReadStream &gff4, uint32 version);
	};

	/** A template of a struct, used when loading a struct. */
	struct StructTemplate {
		struct Field {
			uint32 label;
			uint16 type;
			uint16 flags;
			uint32 offset;
		};

		uint32 index;
		uint32 label;
		uint32 size;

		std::vector<Field> fields;
	};

	typedef std::vector<StructTemplate> StructTemplates;
	typedef std::vector<Common::UString> SharedStrings;
	typedef std::map<uint64, GFF4Struct *> StructMap;



	Common::ScopedPtr<Common::SeekableReadStream> _stream;

	/** This GFF4's header. */
	Header          _header;
	/** All struct templates in this GFF4. */
	StructTemplates _structTemplates;

	/** The shared strings used in V4.1. */
	SharedStrings _sharedStrings;

	/** All actual structs in this GFF4. */
	StructMap   _structs;
	/** The top-level struct. */
	GFF4Struct *_topLevelStruct;


	// .--- Loading helpers
	void load(uint32 type);
	void loadHeader(uint32 type);
	void loadStructs();
	void loadStrings();

	void clear();
	// '---

	// .--- Helper methods called by GFF4Struct
	void registerStruct(uint64 id, GFF4Struct *strct);
	void unregisterStruct(uint64 id);
	GFF4Struct *findStruct(uint64 id);

	Common::SeekableReadStream &getStream(uint32 offset) const;
	const StructTemplate &getStructTemplate(uint32 i) const;
	uint32 getDataOffset() const;

	bool hasSharedStrings() const;
	Common::UString getSharedString(uint32 i) const;
	// '---

	friend class GFF4Struct;
};

class GFF4Struct {
public:
	/** The type of a GFF4 field. */
	enum FieldType {
		kFieldTypeNone        = -    1, ///< Invalid type.
		kFieldTypeUint8       =      0, ///< Unsigned 8bit integer.
		kFieldTypeSint8       =      1, ///< Signed 8bit integer.
		kFieldTypeUint16      =      2, ///< Unsigned 16bit integer.
		kFieldTypeSint16      =      3, ///< Signed 16bit integer.
		kFieldTypeUint32      =      4, ///< Unsigned 32bit integer.
		kFieldTypeSint32      =      5, ///< Signed 32bit integer.
		kFieldTypeUint64      =      6, ///< Unsigned 64bit integer.
		kFieldTypeSint64      =      7, ///< Signed 64bit integer.
		kFieldTypeFloat32     =      8, ///< 32bit IEEE float.
		kFieldTypeFloat64     =      9, ///< 64bit IEEE float (double).
		kFieldTypeVector3f    =     10, ///< 3 IEEE floats, 3D vector.
		kFieldTypeVector4f    =     12, ///< 4 IEEE floats, 4D vector.
		kFieldTypeQuaternionf =     13, ///< 4 IEEE floats, Quaternion rotation.
		kFieldTypeString      =     14, ///< A string.
		kFieldTypeColor4f     =     15, ///< 4 IEEE floats, RGBA color.
		kFieldTypeMatrix4x4f  =     16, ///< 16 IEEE floats, 4x4 matrix in row-major order.
		kFieldTypeTlkString   =     17, ///< 2 unsigned 32bit integers, reference into the TLK table.
		kFieldTypeNDSFixed    =     18, ///< A 32bit fixed-point value, found in Sonic.
		kFieldTypeASCIIString =     20, ///< ASCII string, found in Sonic.
		kFieldTypeStruct      =  65534, ///< A struct.
		kFieldTypeGeneric     =  65535  ///< A "generic" field, able to hold any other type.
	};

	/** Return the struct's unique ID within the GFF4. */
	uint64 getID() const;
	/** Return the number of structs that refer to this struct. */
	uint32 getRefCount() const;

	/** Return the struct's label.
	 *
	 *  The label is a (non-unique) 4-byte value, most often consisting of
	 *  upper-case letters ([A-Z]), that can sometimes be used to identify
	 *  the higher-level meaning of a struct within a GFF4.
	 *
	 *  It is read as a big-endian 32-bit integer value. See the MKTAG()
	 *  macro, as defined in src/common/endianness.h, for generating values
	 *  to compare it against.
	 *
	 *  The purpose of the label in a GFF4 struct is comparable to the ID in
	 *  a GFF3 struct.
	 */
	uint32 getLabel() const;

	/** Return the number of fields in this struct. */
	size_t getFieldCount() const;
	/** Does this specific field exist? */
	bool hasField(uint32 field) const;

	/** Return a list of all field labels in this struct. */
	const std::vector<uint32> &getFieldLabels() const;

	/** Return the type of this field, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32 field) const;
	/** Return the type of this field and whether it's list, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32 field, bool &isList) const;

	/** Collectively return all field properties in one go.
	 *
	 *  @return true if the field exists, false otherwise.
	 */
	bool getFieldProperties(uint32 field, FieldType &type, uint32 &label, bool &isList) const;


	// .--- Single values
	uint64 getUint(uint32 field, uint64 def = 0    ) const;
	 int64 getSint(uint32 field,  int64 def = 0    ) const;
	bool   getBool(uint32 field, bool   def = false) const;

	double getDouble(uint32 field, double def = 0.0 ) const;
	float  getFloat (uint32 field, float  def = 0.0f) const;

	/** Return a field string, read from the given encoding. */
	Common::UString getString(uint32 field, Common::Encoding encoding, const Common::UString &def = "") const;

	/** Return a field string, read from the default UTF-16LE encoding. */
	Common::UString getString(uint32 field, const Common::UString &def = "") const;

	/** Return a talk string, which is a reference into the TalkTable and an optional direct string. */
	bool getTalkString(uint32 field, Common::Encoding encoding, uint32 &strRef, Common::UString &str) const;

	/** Return a talk string, which is a reference into the TalkTable and an optional direct string. */
	bool getTalkString(uint32 field, uint32 &strRef, Common::UString &str) const;

	bool getVector3(uint32 field, double &v1, double &v2, double &v3) const;
	bool getVector4(uint32 field, double &v1, double &v2, double &v3, double &v4) const;

	bool getMatrix4x4(uint32 field, double (&m)[16]) const;

	bool getVector3(uint32 field, float &v1, float &v2, float &v3) const;
	bool getVector4(uint32 field, float &v1, float &v2, float &v3, float &v4) const;

	bool getMatrix4x4(uint32 field, float (&m)[16]) const;

	/** Return a field vector or a matrix type as a std::vector of doubles. */
	bool getVectorMatrix(uint32 field, std::vector<double> &vectorMatrix) const;
	/** Return a field vector or a matrix type as a std::vector of doubles. */
	bool getVectorMatrix(uint32 field, std::vector<float > &vectorMatrix) const;

	bool getMatrix4x4(uint32 field, Common::Matrix4x4 &m) const;
	// '---

	// .--- Lists of values
	bool getUint(uint32 field, std::vector<uint64> &list) const;
	bool getSint(uint32 field, std::vector< int64> &list) const;
	bool getBool(uint32 field, std::vector<bool  > &list) const;

	bool getDouble(uint32 field, std::vector<double> &list) const;
	bool getFloat (uint32 field, std::vector<float > &list) const;

	/** Return field strings, read from the given encoding. */
	bool getString(uint32 field, Common::Encoding encoding, std::vector<Common::UString> &list) const;

	/** Return field strings, read from the default UTF-16LE encoding. */
	bool getString(uint32 field, std::vector<Common::UString> &list) const;

	/** Return field talk strings. */
	bool getTalkString(uint32 field, Common::Encoding encoding,
	                   std::vector<uint32> &strRefs, std::vector<Common::UString> &strs) const;

	/** Return field talk strings. */
	bool getTalkString(uint32 field,
	                   std::vector<uint32> &strRefs, std::vector<Common::UString> &strs) const;

	/** Return field vector or a matrix types as std::vectors of doubles. */
	bool getVectorMatrix(uint32 field, std::vector< std::vector<double> > &list) const;
	/** Return field vector or a matrix types as std::vectors of floats. */
	bool getVectorMatrix(uint32 field, std::vector< std::vector<float > > &list) const;

	bool getMatrix4x4(uint32 field, std::vector<Common::Matrix4x4> &list) const;
	// '---

	// .--- Structs and lists of structs
	const GFF4Struct *getStruct (uint32 field) const;
	const GFF4Struct *getGeneric(uint32 field) const;
	const GFF4List   &getList   (uint32 field) const;
	// '---

	// .--- Raw data
	/** Return the raw data of the field as a Seekable(Sub)ReadStream. Dangerous. */
	Common::SeekableReadStream *getData(uint32 field) const;
	// '---

private:
	/** A field in the GFF4 struct. */
	struct Field {
		uint32    label;  ///< A numerical label of the field.
		FieldType type;   ///< Type of the field.
		uint32    offset; ///< Offset into the GFF4 data.

		bool isList;      ///< Is this field a singular item or a list?
		bool isReference; ///< Is this field a reference (pointer) to another field?
		bool isGeneric;   ///< Is this field found in a generic?

		uint16   structIndex; ///< Index of the field's struct type (if kFieldTypeStruct).
		GFF4List structs;     ///< List of GFF4Struct (if kFieldTypeStruct).

		Field();
		Field(uint32 l, uint16 t, uint16 f, uint32 o, bool g = false);
		~Field();
	};

	typedef std::map<uint32, Field> FieldMap;


	const GFF4File *_parent;

	uint32 _label;

	uint64 _id;
	uint32 _refCount;

	size_t _fieldCount;

	FieldMap _fields;

	/** The labels of all fields in this struct. */
	std::vector<uint32> _fieldLabels;


	// .--- Loader
	/** Load a GFF4 struct. */
	GFF4Struct(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt);
	/** Load a GFF4 generic as a struct. */
	GFF4Struct(GFF4File &parent, const Field &genericParent);
	~GFF4Struct();

	void load(GFF4File &parent, uint32 offset, const GFF4File::StructTemplate &tmplt);
	void loadStructs(GFF4File &parent, Field &field);
	void loadGeneric(GFF4File &parent, Field &field);

	void load(GFF4File &parent, const Field &genericParent);

	static uint64 generateID(uint32 offset, const GFF4File::StructTemplate *tmplt = 0);
	// '---

	// .--- Field and field data accessors
	const Field *getField(uint32 field) const;

	uint32 getDataOffset(bool isReference, uint32 offset) const;
	uint32 getDataOffset(const Field &field) const;

	Common::SeekableReadStream *getData(const Field &field) const;
	Common::SeekableReadStream *getField(uint32 fieldID, const Field *&field) const;
	// '---

	// .--- Field reader helpers
	uint32 getListCount(Common::SeekableReadStream &data, const Field &field) const;
	uint32 getFieldSize(FieldType type) const;

	uint64 getUint(Common::SeekableReadStream &data, FieldType type) const;
	 int64 getSint(Common::SeekableReadStream &data, FieldType type) const;

	double getDouble(Common::SeekableReadStream &data, FieldType type) const;
	float  getFloat (Common::SeekableReadStream &data, FieldType type) const;

	Common::UString getString(Common::SeekableReadStream &data, Common::Encoding encoding) const;
	Common::UString getString(Common::SeekableReadStream &data, Common::Encoding encoding,
	                          uint32 offset) const;
	Common::UString getString(Common::SeekableReadStream &data, const Field &field,
	                          Common::Encoding encoding) const;

	uint32 getVectorMatrixLength(const Field &field, uint32 minLength, uint32 maxLength) const;
	// '---


	friend class GFF4File;
};

} // End of namespace Aurora

#endif // AURORA_GFF4FILE_H

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

#include "src/common/types.h"
#include "src/common/ustring.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/gff4fields.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class GFF4Struct;

/** A GFF (generic file format) V4.0/V4.1 file, found in Dragon Age: Origins,
 *  Dragon Age 2 and Sonic Chronicles: The Dark Brotherhood.
 *
 *  Notes:
 *  - Generics and lists of generics are mapped to structs, with the field ID
 *    being the list element indices (or just 0 on non-list generics).
 *  - Strings are generally encoded in UTF-16LE. One exception is the TLK files
 *    in Sonic, which have strings in a language-specific encoding. For example,
 *    the English, French, Italian, German and Spanish (EFIGS) versions have
 *    the strings in TLK files encoded in Windows CP-1252.
 */
class GFF4File : public AuroraBase {
public:
	GFF4File(Common::SeekableReadStream *gff4, uint32 type = 0xFFFFFFFF);
	GFF4File(const Common::UString &gff4, FileType fileType, uint32 type);
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
		uint32 structCount;  ///< Number of struct templates in thie GFF4.
		uint32 stringCount;  ///< Number of shared strings (V4.1 only)
		uint32 stringOffset; ///< Offset to shared strings (V4.1 only)
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



	Common::SeekableReadStream *_stream;

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
	/** The public field types, representing the abstract contents of the field. */
	enum FieldType {
		kFieldTypeNone = -1,
		kFieldTypeUint,
		kFieldTypeSint,
		kFieldTypeDouble,
		kFieldTypeString,
		kFieldTypeTalkString,
		kFieldTypeVector3,
		kFieldTypeVector4,
		kFieldTypeMatrix,
		kFieldTypeStruct,
		kFieldTypeGeneric
	};

	/** Return the struct's label. */
	uint32 getLabel() const;

	/** Return the number of fields in this struct. */
	size_t getFieldCount() const;
	/** Does this specific field exist? */
	bool hasField(uint32 field) const;

	/** Return the type of this field, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32 field) const;
	/** Return the type of this field and whether it's list, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32 field, bool &isList) const;


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
	// '---

	// .--- Lists of values
	bool getUint(uint32 field, std::vector<uint64> &list) const;
	bool getSint(uint32 field, std::vector< int64> &list) const;
	bool getBool(uint32 field, std::vector<bool  > &list) const;

	bool  getDouble(uint32 field, std::vector<double> &list) const;
	float getFloat (uint32 field, std::vector<float > &list) const;

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
	/** The internal field types, representing the actual contents of the field. */
	enum IFieldType {
		kIFieldTypeNone        = -    1, ///< Invalid type.
		kIFieldTypeUint8       =      0, ///< Unsigned 8bit integer.
		kIFieldTypeSint8       =      1, ///< Signed 8bit integer.
		kIFieldTypeUint16      =      2, ///< Unsigned 16bit integer.
		kIFieldTypeSint16      =      3, ///< Signed 16bit integer.
		kIFieldTypeUint32      =      4, ///< Unsigned 32bit integer.
		kIFieldTypeSint32      =      5, ///< Signed 32bit integer.
		kIFieldTypeUint64      =      6, ///< Unsigned 64bit integer.
		kIFieldTypeSint64      =      7, ///< Signed 64bit integer.
		kIFieldTypeFloat32     =      8, ///< 32bit IEEE float.
		kIFieldTypeFloat64     =      9, ///< 64bit IEEE float (double).
		kIFieldTypeVector3f    =     10, ///< 3 IEEE floats, 3D vector.
		kIFieldTypeVector4f    =     12, ///< 4 IEEE floats, 4D vector.
		kIFieldTypeQuaternionf =     13, ///< 4 IEEE floats, Quaternion rotation.
		kIFieldTypeString      =     14, ///< A string.
		kIFieldTypeColor4f     =     15, ///< 4 IEEE floats, RGBA color.
		kIFieldTypeMatrix4x4f  =     16, ///< 16 IEEE floats, 4x4 matrix in row-major order.
		kIFieldTypeTlkString   =     17, ///< 2 unsigned 32bit integers, reference into the TLK table.
		kIFieldTypeNDSFixed    =     18, ///< A 32bit fixed-point value, found in Sonic.
		kIFieldTypeASCIIString =     20, ///< ASCII string, found in Sonic.
		kIFieldTypeStruct      =  65534, ///< A struct.
		kIFieldTypeGeneric     =  65535  ///< A "generic" field, able to hold any other type.
	};

	/** A field in the GFF4 struct. */
	struct Field {
		uint32     label;  ///< A numerical label of the field.
		IFieldType type;   ///< Type of the field.
		uint32     offset; ///< Offset into the GFF4 data.

		bool isList;      ///< Is this field a singular item or a list?
		bool isReference; ///< Is this field a reference (pointer) to another field?
		bool isGeneric;   ///< Is this field found in a generic?

		uint16   structIndex; ///< Index of the field's struct type (if kIFieldTypeStruct).
		GFF4List structs;     ///< List of GFF4Struct (if kIFieldTypeStruct).

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
	FieldType convertFieldType(IFieldType type) const;

	uint32 getListCount(Common::SeekableReadStream &data, const Field &field) const;
	uint32 getFieldSize(IFieldType type) const;

	uint64 getUint(Common::SeekableReadStream &data, IFieldType type) const;
	 int64 getSint(Common::SeekableReadStream &data, IFieldType type) const;

	double getDouble(Common::SeekableReadStream &data, IFieldType type) const;
	float  getFloat (Common::SeekableReadStream &data, IFieldType type) const;

	Common::UString getString(Common::SeekableReadStream &data, Common::Encoding encoding) const;
	Common::UString getString(Common::SeekableReadStream &data, Common::Encoding encoding,
	                          uint32 offset) const;
	Common::UString getString(Common::SeekableReadStream &data, const Field &field,
	                          Common::Encoding encoding) const;

	uint32 getVectorMatrixLength(const Field &field, uint32 maxLength) const;
	// '---


	friend class GFF4File;
};

} // End of namespace Aurora

#endif // AURORA_GFF4FILE_H

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
#include <memory>

#include "external/glm/mat4x4.hpp"

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/endianness.h"
#include "src/common/ustring.h"
#include "src/common/encoding.h"

#include "src/aurora/types.h"
#include "src/aurora/aurorafile.h"
#include "src/aurora/gff4fields.h"

namespace Common {
	class SeekableReadStream;
	class SeekableSubReadStreamEndian;
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
 *  - Strings are generally encoded in UTF-16, with native endianness according
 *    to the platform ID. "PC" is little endian, while "PS3" and "X360" are
 *    big endian. One exception to that rule is the TLK files in Sonic, which
 *    have strings in a language-specific encoding. For example, the English,
 *    French, Italian, German and Spanish (EFIGS) versions have the strings
 *    in TLK files encoded in Windows CP-1252.
 *
 *  See also: GFF3File in gff3file.h for the earlier V3.2/V3.3 versions of
 *  the GFF format.
 */
class GFF4File : boost::noncopyable, public AuroraFile {
public:
	/** Read a GFF4 file out of the stream. */
	GFF4File(std::unique_ptr<Common::SeekableReadStream> gff4, uint32_t type = 0xFFFFFFFF);
	/** Take over this stream and read a GFF4 file out of it. */
	GFF4File(Common::SeekableReadStream *gff4, uint32_t type = 0xFFFFFFFF);
	/** Request this resource from the ResourceManager and read a GFF4 file out of it. */
	GFF4File(const Common::UString &gff4, FileType fileType, uint32_t type = 0xFFFFFFFF);
	~GFF4File();

	/** Return the GFF4's specific type. */
	uint32_t getType() const;
	/** Return the GFF4's specific type version. */
	uint32_t getTypeVersion() const;
	/** Return the platform this GFF4 is for. */
	uint32_t getPlatform() const;

	/** Is the GFF4's platform natively big endian? */
	bool isBigEndian() const;
	/** Return the native UTF-16 encoding according to the GFF4's platform. */
	Common::Encoding getNativeEncoding() const;

	/** Returns the top-level struct. */
	const GFF4Struct &getTopLevel() const;


private:
	enum Platform {
		kGFF4PlatformPC      = MKTAG('P', 'C', ' ', ' '),
		kGFF4PlatformPS3     = MKTAG('P', 'S', '3', ' '),
		kGFF4PlatformXbox360 = MKTAG('X', '3', '6', '0')
	};

	/** A GFF4 header. */
	struct Header {
		uint32_t platformID;   ///< ID of the platform this GFF4 is for.
		uint32_t type;         ///< The specific type this GFF4 describes.
		uint32_t typeVersion;  ///< The version of the specific type this GFF4 describes.
		uint32_t structCount;  ///< Number of struct templates in this GFF4.
		uint32_t stringCount;  ///< Number of shared strings (V4.1 only).
		uint32_t stringOffset; ///< Offset to shared strings (V4.1 only).
		uint32_t dataOffset;   ///< Offset to the data portion.

		bool hasSharedStrings;

		void read(Common::SeekableReadStream &gff4, uint32_t version);
		bool isBigEndian() const;
	};

	/** A template of a struct, used when loading a struct. */
	struct StructTemplate {
		struct Field {
			uint32_t label;
			uint16_t type;
			uint16_t flags;
			uint32_t offset;
		};

		uint32_t index;
		uint32_t label;
		uint32_t size;

		std::vector<Field> fields;
	};

	typedef std::vector<StructTemplate> StructTemplates;
	typedef std::vector<Common::UString> SharedStrings;
	typedef std::map<uint64_t, GFF4Struct *> StructMap;



	std::unique_ptr<Common::SeekableReadStream> _origStream;
	std::unique_ptr<Common::SeekableSubReadStreamEndian> _stream;

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
	void load(uint32_t type);
	void loadHeader(uint32_t type);
	void loadStructs();
	void loadStrings();

	void clear();
	// '---

	// .--- Helper methods called by GFF4Struct
	void registerStruct(uint64_t id, GFF4Struct *strct);
	void unregisterStruct(uint64_t id);
	GFF4Struct *findStruct(uint64_t id);

	Common::SeekableSubReadStreamEndian &getStream(uint32_t offset) const;
	const StructTemplate &getStructTemplate(uint32_t i) const;
	uint32_t getDataOffset() const;

	bool hasSharedStrings() const;
	Common::UString getSharedString(uint32_t i) const;
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
	uint64_t getID() const;
	/** Return the number of structs that refer to this struct. */
	uint32_t getRefCount() const;

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
	uint32_t getLabel() const;

	/** Return the number of fields in this struct. */
	size_t getFieldCount() const;
	/** Does this specific field exist? */
	bool hasField(uint32_t field) const;

	/** Return a list of all field labels in this struct. */
	const std::vector<uint32_t> &getFieldLabels() const;

	/** Return the type of this field, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32_t field) const;
	/** Return the type of this field and whether it's list, or kFieldTypeNone if it doesn't exist. */
	FieldType getFieldType(uint32_t field, bool &isList) const;

	/** Collectively return all field properties in one go.
	 *
	 *  @return true if the field exists, false otherwise.
	 */
	bool getFieldProperties(uint32_t field, FieldType &type, uint32_t &label, bool &isList) const;


	// .--- Single values
	uint64_t getUint(uint32_t field, uint64_t def = 0    ) const;
	 int64_t getSint(uint32_t field,  int64_t def = 0    ) const;
	bool     getBool(uint32_t field, bool     def = false) const;

	double getDouble(uint32_t field, double def = 0.0 ) const;
	float  getFloat (uint32_t field, float  def = 0.0f) const;

	/** Return a field string, read from the given encoding. */
	Common::UString getString(uint32_t field, Common::Encoding encoding, const Common::UString &def = "") const;

	/** Return a field string, read from the default UTF-16LE encoding. */
	Common::UString getString(uint32_t field, const Common::UString &def = "") const;

	/** Return a talk string, which is a reference into the TalkTable and an optional direct string. */
	bool getTalkString(uint32_t field, Common::Encoding encoding, uint32_t &strRef, Common::UString &str) const;

	/** Return a talk string, which is a reference into the TalkTable and an optional direct string. */
	bool getTalkString(uint32_t field, uint32_t &strRef, Common::UString &str) const;

	bool getVector3(uint32_t field, double &v1, double &v2, double &v3) const;
	bool getVector4(uint32_t field, double &v1, double &v2, double &v3, double &v4) const;

	bool getMatrix4x4(uint32_t field, double (&m)[16]) const;

	bool getVector3(uint32_t field, float &v1, float &v2, float &v3) const;
	bool getVector4(uint32_t field, float &v1, float &v2, float &v3, float &v4) const;

	bool getMatrix4x4(uint32_t field, float (&m)[16]) const;

	/** Return a field vector or a matrix type as a std::vector of doubles. */
	bool getVectorMatrix(uint32_t field, std::vector<double> &vectorMatrix) const;
	/** Return a field vector or a matrix type as a std::vector of doubles. */
	bool getVectorMatrix(uint32_t field, std::vector<float > &vectorMatrix) const;

	bool getMatrix4x4(uint32_t field, glm::mat4 &m) const;
	// '---

	// .--- Lists of values
	bool getUint(uint32_t field, std::vector<uint64_t> &list) const;
	bool getSint(uint32_t field, std::vector< int64_t> &list) const;
	bool getBool(uint32_t field, std::vector<bool  > &list) const;

	bool getDouble(uint32_t field, std::vector<double> &list) const;
	bool getFloat (uint32_t field, std::vector<float > &list) const;

	/** Return field strings, read from the given encoding. */
	bool getString(uint32_t field, Common::Encoding encoding, std::vector<Common::UString> &list) const;

	/** Return field strings, read from the default UTF-16LE encoding. */
	bool getString(uint32_t field, std::vector<Common::UString> &list) const;

	/** Return field talk strings. */
	bool getTalkString(uint32_t field, Common::Encoding encoding,
	                   std::vector<uint32_t> &strRefs, std::vector<Common::UString> &strs) const;

	/** Return field talk strings. */
	bool getTalkString(uint32_t field,
	                   std::vector<uint32_t> &strRefs, std::vector<Common::UString> &strs) const;

	/** Return field vector or a matrix types as std::vectors of doubles. */
	bool getVectorMatrix(uint32_t field, std::vector< std::vector<double> > &list) const;
	/** Return field vector or a matrix types as std::vectors of floats. */
	bool getVectorMatrix(uint32_t field, std::vector< std::vector<float > > &list) const;

	bool getMatrix4x4(uint32_t field, std::vector<glm::mat4> &list) const;
	// '---

	// .--- Structs and lists of structs
	const GFF4Struct *getStruct (uint32_t field) const;
	const GFF4Struct *getGeneric(uint32_t field) const;
	const GFF4List   &getList   (uint32_t field) const;
	// '---

	// .--- Raw data
	/** Return the raw data of the field as a Seekable(Sub)ReadStream. Dangerous. */
	Common::SeekableReadStream *getData(uint32_t field) const;
	// '---

private:
	/** A field in the GFF4 struct. */
	struct Field {
		uint32_t  label { 0 };             ///< A numerical label of the field.
		FieldType type { kFieldTypeNone }; ///< Type of the field.
		uint32_t  offset { 0xFFFFFFFF };   ///< Offset into the GFF4 data.

		bool isList { false };      ///< Is this field a singular item or a list?
		bool isReference { false }; ///< Is this field a reference (pointer) to another field?
		bool isGeneric { false };   ///< Is this field found in a generic?

		uint16_t structIndex { 0 }; ///< Index of the field's struct type (if kFieldTypeStruct).
		GFF4List structs;           ///< List of GFF4Struct (if kFieldTypeStruct).

		Field() = default;
		Field(const Field &) = default;
		Field(uint32_t l, uint16_t t, uint16_t f, uint32_t o, bool g = false);
		~Field() = default;

		Field &operator=(const Field &) = default;
	};

	typedef std::map<uint32_t, Field> FieldMap;


	const GFF4File *_parent;

	uint32_t _label;

	uint64_t _id;
	uint32_t _refCount;

	size_t _fieldCount;

	FieldMap _fields;

	/** The labels of all fields in this struct. */
	std::vector<uint32_t> _fieldLabels;


	// .--- Loader
	/** Load a GFF4 struct. */
	GFF4Struct(GFF4File &parent, uint32_t offset, const GFF4File::StructTemplate &tmplt);
	/** Load a GFF4 generic as a struct. */
	GFF4Struct(GFF4File &parent, const Field &genericParent);
	~GFF4Struct();

	void load(GFF4File &parent, uint32_t offset, const GFF4File::StructTemplate &tmplt);
	void loadStructs(GFF4File &parent, Field &field);
	void loadGeneric(GFF4File &parent, Field &field);

	void load(GFF4File &parent, const Field &genericParent);

	static uint64_t generateID(uint32_t offset, const GFF4File::StructTemplate *tmplt = 0);
	// '---

	// .--- Field and field data accessors
	const Field *getField(uint32_t field) const;

	uint32_t getDataOffset(bool isReference, uint32_t offset) const;
	uint32_t getDataOffset(const Field &field) const;

	Common::SeekableSubReadStreamEndian *getData(const Field &field) const;
	Common::SeekableSubReadStreamEndian *getField(uint32_t fieldID, const Field *&field) const;
	// '---

	// .--- Field reader helpers
	uint32_t getListCount(Common::SeekableSubReadStreamEndian &data, const Field &field) const;
	uint32_t getFieldSize(FieldType type) const;

	uint64_t getUint(Common::SeekableSubReadStreamEndian &data, FieldType type) const;
	 int64_t getSint(Common::SeekableSubReadStreamEndian &data, FieldType type) const;

	double getDouble(Common::SeekableSubReadStreamEndian &data, FieldType type) const;
	float  getFloat (Common::SeekableSubReadStreamEndian &data, FieldType type) const;

	Common::UString getString(Common::SeekableSubReadStreamEndian &data, Common::Encoding encoding) const;
	Common::UString getString(Common::SeekableSubReadStreamEndian &data, Common::Encoding encoding,
	                          uint32_t offset) const;
	Common::UString getString(Common::SeekableSubReadStreamEndian &data, const Field &field,
	                          Common::Encoding encoding) const;

	uint32_t getVectorMatrixLength(const Field &field, uint32_t minLength, uint32_t maxLength) const;
	// '---


	friend class GFF4File;
};

} // End of namespace Aurora

#endif // AURORA_GFF4FILE_H

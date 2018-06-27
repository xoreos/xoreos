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
 *  Writer for writing version 3.x of biowares gff (general file format).
 */

#ifndef AURORA_GFF3WRITER_H
#define AURORA_GFF3WRITER_H

#include <list>

#include <glm/glm.hpp>

#include <boost/shared_ptr.hpp>

#include "src/aurora/gff3file.h"
#include "src/aurora/locstring.h"

namespace Aurora {

class GFF3WriterStruct;
class GFF3WriterList;

typedef boost::shared_ptr<GFF3WriterStruct> GFF3WriterStructPtr;
typedef boost::shared_ptr<GFF3WriterList> GFF3WriterListPtr;

class GFF3Writer {
public:
	// TODO: Add a constructor consuming a GFF3File object.
	GFF3Writer(uint32 id, uint32 version = MKTAG('V', '3', '.', '2'));

	/** Get the toplevel struct. */
	GFF3WriterStructPtr getTopLevelStruct();

	/** Write the GFF3 to stream. */
	void write(Common::WriteStream &stream);

private:
	/** An implementation for a field. */
	struct Field : boost::noncopyable {
		GFF3Struct::FieldType type;
		uint32 labelIndex;

		uint32 uint32Value;
		uint64 uint64Value;
		int32 int32Value;
		int64 int64Value;
		float floatValue;
		double doubleValue;
		glm::vec4 vectorValue;
		Common::UString stringValue;
		LocString locStringValue;
		Common::ScopedArray<byte> voidData;
		uint32 voidSize;

		Field() {
		}
	};

	typedef boost::shared_ptr<Field> FieldPtr;

	uint32 _id, _version;

	std::vector<GFF3WriterStructPtr> _structs;
	std::vector<GFF3WriterListPtr> _lists;

	std::vector<Common::UString> _labels;
	std::vector<FieldPtr> _fields;

	friend class GFF3WriterList;
	friend class GFF3WriterStruct;

	/** Adds a label to the writer and returns the corresponding index. */
	uint32 addLabel(const Common::UString &label);
	/** Get the actual size of the field. */
	static uint32 getFieldDataSize(FieldPtr field);

	size_t createField(GFF3Struct::FieldType type, const Common::UString &label);
};

/**
 * Every GFF3 list can contain structs.
 */
class GFF3WriterList : boost::noncopyable {
public:
	GFF3WriterList(GFF3Writer *parent);

	/** Add a new struct to the list. */
	GFF3WriterStructPtr addStruct(const Common::UString &label);

	size_t getSize() const;

private:
	friend class GFF3Writer;
	friend class GFF3WriterStruct;

	GFF3Writer *_parent;
	std::vector<size_t> _strcts;
};

/**
 * A GFF3 struct can contain every other value including other structs.
 */
class GFF3WriterStruct : boost::noncopyable {
public:
	GFF3WriterStruct(GFF3Writer *parent, uint32 id = 0xFFFFFFFF);

	/** Get Id of the struct. */
	uint32 getId() const;
	/** Get the count of fields. */
	size_t getFieldCount() const;

	/** Create a new struct. */
	GFF3WriterStructPtr addStruct(const Common::UString &label);
	/** Create a new list. */
	GFF3WriterListPtr addList(const Common::UString &label);

	/** Add a new byte. */
	void addByte(const Common::UString &label, byte value);
	/** Add a new char. */
	void addChar(const Common::UString &label, char value);
	/** Add a new float. */
	void addFloat(const Common::UString &label, float value);
	/** Add a new double. */
	void addDouble(const Common::UString &label, double value);
	/** Add a new uint16. */
	void addUint16(const Common::UString &label, uint16 value);
	/** Add a new uint32. */
	void addUint32(const Common::UString &label, uint32 value);
	/** Add a new uint64. */
	void addUint64(const Common::UString &label, uint64 value);
	/** Add a new sint16. */
	void addSint16(const Common::UString &label, int16 value);
	/** Add a new sint32. */
	void addSint32(const Common::UString &label, int32 value);
	/** Add a new sint64. */
	void addSint64(const Common::UString &label, int64 value);
	/** Add a new ExoString. */
	void addExoString(const Common::UString &label, const Common::UString &value);
	/** Add a new String reference. */
	void addStrRef(const Common::UString &label, uint32 value);
	/** Add a new Resource reference. */
	void addResRef(const Common::UString &label, const Common::UString &value);
	/** Add new void data. Data will be copied. */
	void addVoid(const Common::UString &label, const byte *data, uint32 size);
	/** Add a new Vector. */
	void addVector(const Common::UString &label, glm::vec3 value);
	/** Add a new Orientation. */
	void addOrientation(const Common::UString &label, glm::vec4 value);
	/** Add a new LocString. */
	void addLocString(const Common::UString &label, const LocString &value);

private:
	GFF3Writer::FieldPtr createField(GFF3Struct::FieldType type, const Common::UString &label);

	uint32 _id;
	GFF3Writer *_parent;
	std::vector<size_t> _fieldIndices;

	friend class GFF3Writer;
	friend class GFF3WriterList;
};

} // End of namespace Aurora

#endif // AURORA_GFF3WRITER_H

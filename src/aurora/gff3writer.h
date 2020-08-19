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
 *  Writer for writing version V3.2/V3.3 of BioWare's GFFs (generic file format).
 */

#ifndef AURORA_GFF3WRITER_H
#define AURORA_GFF3WRITER_H

#include <list>
#include <functional>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/variant.hpp>

#ifdef BOOST_COMP_CLANG
	#define GLM_LANG_STL11_FORCED // Fix clang glm c++11 bug
#endif // BOOST_COMP_CLANG
#include "external/glm/glm.hpp"
#include "external/glm/gtx/hash.hpp"

#include "src/common/readstream.h"
#include "src/common/memwritestream.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/locstring.h"

namespace Aurora {

class GFF3WriterStruct;
class GFF3WriterList;

typedef boost::shared_ptr<GFF3WriterStruct> GFF3WriterStructPtr;
typedef boost::shared_ptr<GFF3WriterList> GFF3WriterListPtr;

class GFF3Writer : boost::noncopyable {
public:
	// TODO: Add a constructor consuming a GFF3File object.
	GFF3Writer(uint32_t id, uint32_t version = MKTAG('V', '3', '.', '2'));

	/** Get the top-level struct. */
	GFF3WriterStructPtr getTopLevel();

	/** Write the GFF3 to stream. */
	void write(Common::WriteStream &stream);

private:
	struct Vector4 {
		glm::vec4 vec;

		Vector4(glm::vec4 vector) : vec(vector) {
		}

		bool operator==(const Vector4 &v)  const {
			return vec == v.vec;
		}

		bool operator<(const Vector4 &v) const {
			std::hash<glm::vec4> h;
			return h(vec) < h(v.vec);
		}
	};

	/** A special struct type for representing void data. */
	struct VoidData {
		std::unique_ptr<Common::SeekableReadStream> data;

		VoidData() { }

		VoidData(Common::SeekableReadStream *stream);

		VoidData(const VoidData &voidData) { *this = voidData; }

		VoidData &operator=(const VoidData &rhs);

		bool operator==(const VoidData &rhs) const {
			return data.get() == rhs.data.get();
		}

		bool operator<(const VoidData &rhs) const {
			return std::less<Common::SeekableReadStream*>{}(data.get(), rhs.data.get());
		}
	};

	/** A variant containing all possible types of GFF data. */
	typedef boost::variant<
		uint32_t,
		uint64_t,
		int32_t,
		int64_t,
		float,
		double,
		Vector4,
		Common::UString,
		LocString,
		VoidData
	> ValueData;

	class ValueDataLess : public boost::static_visitor<bool> {
	public:
		template<typename T, typename U> bool operator()(const T &UNUSED(v1), const U &UNUSED(v2)) const { return false; }

		template<typename T> bool operator()(const T &v1, const T &v2) const { return v1 < v2; }
	};

	/** A value holds a type and data. */
	struct Value {
		GFF3Struct::FieldType type;
		ValueData data;
		bool isRaw { false };

		/** Equality operator for std::find. */
		bool operator==(const Value &rhs) const {
			return type == rhs.type &&
			       data == rhs.data;
		}

		/** Relational operator for map find. */
		bool operator<(const Value &rhs) const {
			if (type != rhs.type) {
				return type < rhs.type;
			} else if (data.which() != rhs.data.which()) {
				return data.which() < rhs.data.which();
			} else {
				return boost::apply_visitor(ValueDataLess(), data, rhs.data);
			}
		}
	};

	/** An implementation for a field. */
	struct Field : boost::noncopyable {
		uint32_t labelIndex;
		Value value;
	};

	typedef boost::shared_ptr<Field> FieldPtr;

	uint32_t _id;
	uint32_t _version;

	std::vector<GFF3WriterStructPtr> _structs;
	std::vector<GFF3WriterListPtr> _lists;

	std::vector<Common::UString> _labels;
	std::vector<FieldPtr> _fields;

	friend class GFF3WriterList;
	friend class GFF3WriterStruct;

	/** Adds a label to the writer and returns the corresponding index. */
	uint32_t addLabel(const Common::UString &label);
	/** Get the actual size of the field. */
	static uint32_t getFieldDataSize(Value field);

	size_t createField(GFF3Struct::FieldType type, const Common::UString &label);
};

/** A GFF3 list containing GFF3 structs. */
class GFF3WriterList : boost::noncopyable {
public:
	GFF3WriterList(GFF3Writer *parent);

	/** Add a new struct to the list. */
	GFF3WriterStructPtr addStruct(const Common::UString &label);
	/** Add a new struct to the list. */
	GFF3WriterStructPtr addStruct(const Common::UString &label, uint32_t id);

	size_t getSize() const;

private:
	friend class GFF3Writer;
	friend class GFF3WriterStruct;

	GFF3Writer *_parent;
	std::vector<size_t> _strcts;
};

/** A GFF3 struct containing GFF3 fields.
 *
 *  A field can be of any type, including list and struct.
 */
class GFF3WriterStruct : boost::noncopyable {
public:
	GFF3WriterStruct(GFF3Writer *parent, uint32_t id = 0xFFFFFFFF);

	/** Get ID of the struct. */
	uint32_t getID() const;
	/** Get the count of fields. */
	size_t getFieldCount() const;

	/** Create a new struct. */
	GFF3WriterStructPtr addStruct(const Common::UString &label);
	/** Create a new struct. */
	GFF3WriterStructPtr addStruct(const Common::UString &label, uint32_t id);
	/** Create a new list. */
	GFF3WriterListPtr addList(const Common::UString &label);

	/** Add a new byte. */
	void addByte(const Common::UString &label, uint8_t value);
	/** Add a new char. */
	void addChar(const Common::UString &label, int8_t value);
	/** Add a new float. */
	void addFloat(const Common::UString &label, float value);
	/** Add a new double. */
	void addDouble(const Common::UString &label, double value);
	/** Add a new uint16_t. */
	void addUint16(const Common::UString &label, uint16_t value);
	/** Add a new uint32_t. */
	void addUint32(const Common::UString &label, uint32_t value);
	/** Add a new uint64_t. */
	void addUint64(const Common::UString &label, uint64_t value);
	/** Add a new sint16. */
	void addSint16(const Common::UString &label, int16_t value);
	/** Add a new sint32. */
	void addSint32(const Common::UString &label, int32_t value);
	/** Add a new sint64. */
	void addSint64(const Common::UString &label, int64_t value);
	/** Add a new ExoString. */
	void addExoString(const Common::UString &label, const Common::UString &value);
	/** Add a new ExoString. */
	void addExoString(const Common::UString &label, Common::SeekableReadStream *value);
	/** Add a new String reference. */
	void addStrRef(const Common::UString &label, uint32_t value);
	/** Add a new Resource reference. */
	void addResRef(const Common::UString &label, const Common::UString &value);
	/** Add a new Resource reference. */
	void addResRef(const Common::UString &label, Common::SeekableReadStream *value);
	/** Add new void data. Data will be copied. */
	void addVoid(const Common::UString &label, Common::SeekableReadStream *value);
	/** Add a new Vector. */
	void addVector(const Common::UString &label, glm::vec3 value);
	/** Add a new Orientation. */
	void addOrientation(const Common::UString &label, glm::vec4 value);
	/** Add a new LocString. */
	void addLocString(const Common::UString &label, const LocString &value);

private:
	GFF3Writer::FieldPtr createField(GFF3Struct::FieldType type, const Common::UString &label);

	uint32_t _id;
	GFF3Writer *_parent;
	std::vector<size_t> _fieldIndices;

	friend class GFF3Writer;
	friend class GFF3WriterList;
};

} // End of namespace Aurora

#endif // AURORA_GFF3WRITER_H

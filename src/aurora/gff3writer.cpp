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

#include <algorithm>

#include <boost/make_shared.hpp>

#include "src/common/writestream.h"
#include "src/common/memreadstream.h"

#include "src/aurora/gff3writer.h"

namespace Aurora {

GFF3Writer::VoidData::VoidData(Common::SeekableReadStream *stream) {
	data.reset(stream);
	data->seek(0);
}

GFF3Writer::VoidData &GFF3Writer::VoidData::operator=(const VoidData &rhs) {
	data.reset();

	if (!rhs.data)
		return *this;

	data.reset(rhs.data->readStream(rhs.data->size()));
	rhs.data->seek(0);

	return *this;
}


GFF3Writer::GFF3Writer(uint32_t id, uint32_t version) : _id(id), _version(version) {
	_structs.push_back(boost::make_shared<GFF3WriterStruct>(this));
}

GFF3WriterStructPtr GFF3Writer::getTopLevel() {
	return _structs[0];
}

void GFF3Writer::write(Common::WriteStream &stream) {
	// Extract all individual values of the fields.
	std::list<Value> individualValues;
	for (const auto &field : _fields) {
		if (std::find(individualValues.begin(), individualValues.end(), field->value) == individualValues.end())
			individualValues.push_back(field->value);
	}

	stream.writeUint32BE(_id);
	stream.writeUint32BE(_version);

	uint32_t structOffset = 56; // ID + version + header
	uint32_t structCount = static_cast<uint32_t>(_structs.size());

	uint32_t fieldOffset = structOffset + structCount * 12;
	uint32_t fieldCount = static_cast<uint32_t>(_fields.size());

	uint32_t labelOffset = fieldOffset + fieldCount * 12;
	uint32_t labelCount = static_cast<uint32_t>(_labels.size());

	uint32_t fieldDataOffset = labelOffset + labelCount * 16;
	uint32_t fieldDataCount = 0;

	// Count the total size of field data
	for (const auto &value : individualValues) {
		fieldDataCount += getFieldDataSize(value);
	}

	uint32_t fieldIndicesOffset = fieldDataOffset + fieldDataCount;
	uint32_t fieldIndicesCount = 0;

	// Count all fields of structs with more than one field
	for (size_t i = 0; i < _structs.size(); ++i) {
		const GFF3WriterStructPtr strct = _structs[i];
		if (strct->getFieldCount() <= 1)
			continue;

		fieldIndicesCount += strct->getFieldCount() * 4;
	}

	uint32_t listIndicesOffset = fieldIndicesOffset + fieldIndicesCount;
	uint32_t listIndicesCount = 0;

	// Count all lists elements plus their size as int
	for (size_t i = 0; i < _lists.size(); ++i) {
		listIndicesCount += (_lists[i]->getSize() + 1) * 4;
	}

	// Write the header
	stream.writeUint32LE(structOffset);
	stream.writeUint32LE(structCount);
	stream.writeUint32LE(fieldOffset);
	stream.writeUint32LE(fieldCount);
	stream.writeUint32LE(labelOffset);
	stream.writeUint32LE(labelCount);
	stream.writeUint32LE(fieldDataOffset);
	stream.writeUint32LE(fieldDataCount);
	stream.writeUint32LE(fieldIndicesOffset);
	stream.writeUint32LE(fieldIndicesCount);
	stream.writeUint32LE(listIndicesOffset);
	stream.writeUint32LE(listIndicesCount);

	// Write structs data
	size_t structFieldIndicesIndex = 0;
	for (size_t i = 0; i < _structs.size(); ++i) {
		GFF3WriterStructPtr strct = _structs[i];

		// Struct ID
		stream.writeUint32LE(strct->getID());

		// Field index
		if (strct->getFieldCount() > 1) {
			stream.writeUint32LE(structFieldIndicesIndex * 4);
			structFieldIndicesIndex += strct->getFieldCount();
		} else {
			if (strct->getFieldCount() != 0)
				stream.writeUint32LE(strct->_fieldIndices[0]);
			else
				stream.writeUint32LE(0);
		}

		// Field count
		stream.writeUint32LE(strct->getFieldCount());
	}

	// Write fields
	size_t fieldDataIndex = 0;
	size_t listDataIndex = 0;

	// Maps for checking for already existing values
	std::map<Value, size_t> valueIndices;

	for (size_t i = 0; i < _fields.size(); ++i) {
		FieldPtr field = _fields[i];
		stream.writeUint32LE(field->value.type);
		stream.writeUint32LE(field->labelIndex);

		/* Determine if this field has simple values (less equal 32 bit) which are written in the field
		 * or complex values, bigger than 32bit like strings written in the field data section. */
		const bool simple =
			field->value.type == GFF3Struct::kFieldTypeByte ||
			field->value.type == GFF3Struct::kFieldTypeChar ||
			field->value.type == GFF3Struct::kFieldTypeUint16 ||
			field->value.type == GFF3Struct::kFieldTypeUint32 ||
			field->value.type == GFF3Struct::kFieldTypeStruct ||
			field->value.type == GFF3Struct::kFieldTypeSint16 ||
			field->value.type == GFF3Struct::kFieldTypeSint32 ||
			field->value.type == GFF3Struct::kFieldTypeFloat ||
			field->value.type == GFF3Struct::kFieldTypeList;

		if (simple) {
			// If the values are simple (less equal 4 bytes) write them to the field
			switch (field->value.type) {
				case GFF3Struct::kFieldTypeByte:
				case GFF3Struct::kFieldTypeUint16:
				case GFF3Struct::kFieldTypeUint32:
				case GFF3Struct::kFieldTypeStruct:
					stream.writeUint32LE(boost::get<uint32_t>(field->value.data));
					break;
				case GFF3Struct::kFieldTypeList:
					stream.writeUint32LE(listDataIndex * 4);
					listDataIndex += 1 + _lists[boost::get<uint32_t>(field->value.data)]->getSize();
					break;
				case GFF3Struct::kFieldTypeChar:
				case GFF3Struct::kFieldTypeSint16:
				case GFF3Struct::kFieldTypeSint32:
					stream.writeSint32LE(boost::get<int32_t>(field->value.data));
					break;
				case GFF3Struct::kFieldTypeFloat:
					stream.writeIEEEFloatLE(boost::get<float>(field->value.data));
					break;
				default:
					throw Common::Exception("Invalid Field type");
			}
		} else {
			// If the values are complex (greater then 4 bytes) write the index to the field data

			size_t index = 0;
			if (valueIndices.find(field->value) == valueIndices.end()) {
				index = fieldDataIndex;
				fieldDataIndex += getFieldDataSize(field->value);
				valueIndices[field->value] = index;
			} else {
				index = valueIndices[field->value];
			}

			stream.writeUint32LE(index);
		}
	}

	// Write labels
	for (size_t i = 0; i < _labels.size(); ++i) {
		const Common::UString &label = _labels[i];
		stream.write(label.c_str(), MIN<size_t>(label.size(), 16));
		stream.writeZeros(16 - MIN<size_t>(label.size(), 16));
	}

	// Write field data
	for (const auto &value : individualValues) {
		switch (value.type) {
			case GFF3Struct::kFieldTypeUint64:
				stream.writeUint64LE(boost::get<uint64_t>(value.data));
				break;
			case GFF3Struct::kFieldTypeSint64:
				stream.writeSint64LE(boost::get<int64_t>(value.data));
				break;
			case GFF3Struct::kFieldTypeDouble:
				stream.writeIEEEDoubleLE(boost::get<double>(value.data));
				break;
			case GFF3Struct::kFieldTypeStrRef:
				stream.writeUint32LE(4);
				stream.writeUint32LE(boost::get<uint32_t>(value.data));
				break;
			case GFF3Struct::kFieldTypeResRef:
				if (value.isRaw) {
					const VoidData &voidData = boost::get<VoidData>(value.data);

					stream.writeByte(MIN<size_t>(255, static_cast<uint32_t>(voidData.data->size())));
					stream.writeStream(*voidData.data, 255);
					voidData.data->seek(0);

				} else {
					const Common::UString &string = boost::get<Common::UString>(value.data);

					stream.writeByte(MIN<size_t>(255, string.size()));
					stream.write(string.c_str(), MIN<size_t>(string.size(), 255));
				}

				break;
			case GFF3Struct::kFieldTypeExoString:
				if (value.isRaw) {
					const VoidData &voidData = boost::get<VoidData>(value.data);

					stream.writeUint32LE(static_cast<uint32_t>(voidData.data->size()));
					stream.writeStream(*voidData.data);
					voidData.data->seek(0);

				} else {
					const Common::UString &string = boost::get<Common::UString>(value.data);

					stream.writeUint32LE(static_cast<uint32_t>(string.size()));
					stream.writeString(string);
				}
				break;
			case GFF3Struct::kFieldTypeLocString:
				stream.writeUint32LE(boost::get<LocString>(value.data).getWrittenSize() + 8);
				stream.writeUint32LE(boost::get<LocString>(value.data).getID());
				stream.writeUint32LE(boost::get<LocString>(value.data).getNumStrings());
				boost::get<LocString>(value.data).writeLocString(stream);
				break;
			case GFF3Struct::kFieldTypeVoid:
				stream.writeUint32LE(static_cast<uint32_t>(boost::get<VoidData>(value.data).data->size()));
				stream.writeStream(*boost::get<VoidData>(value.data).data);
				boost::get<VoidData>(value.data).data->seek(0);
				break;
			case GFF3Struct::kFieldTypeVector:
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.x);
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.y);
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.z);
				break;
			case GFF3Struct::kFieldTypeOrientation:
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.x);
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.y);
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.z);
				stream.writeIEEEFloatLE(boost::get<Vector4>(value.data).vec.w);
				break;
			default:
				break;
		}
	}

	// Write field indices of every struct with more than one field
	for (size_t i = 0; i < _structs.size(); ++i) {
		GFF3WriterStructPtr strct = _structs[i];
		if (strct->getFieldCount() <= 1)
			continue;

		for (size_t j = 0; j < strct->getFieldCount(); ++j) {
			stream.writeUint32LE(strct->_fieldIndices[j]);
		}
	}

	// Write list indices
	for (size_t i = 0; i < _lists.size(); ++i) {
		GFF3WriterListPtr list = _lists[i];
		stream.writeUint32LE(list->getSize());
		for (size_t j = 0; j < list->_strcts.size(); ++j) {
			stream.writeUint32LE(list->_strcts[j]);
		}
	}
}

uint32_t GFF3Writer::addLabel(const Common::UString &label) {
	std::vector<Common::UString>::iterator iter = std::find(_labels.begin(), _labels.end(), label);
	if (iter != _labels.end()) {
		return static_cast<uint32_t>(std::distance(_labels.begin(), iter));
	} else {
		_labels.push_back(label);
		return static_cast<uint32_t>(_labels.size() - 1);
	}
}

uint32_t GFF3Writer::getFieldDataSize(Value value) {
	switch (value.type) {
		case GFF3Struct::kFieldTypeUint64:
		case GFF3Struct::kFieldTypeSint64:
		case GFF3Struct::kFieldTypeStrRef:
		case GFF3Struct::kFieldTypeDouble:
			return 8;
		case GFF3Struct::kFieldTypeExoString:
			if (value.isRaw)
				return 4 + boost::get<VoidData>(value.data).data->size();
			else
				return 4 + boost::get<Common::UString>(value.data).size();
		case GFF3Struct::kFieldTypeLocString:
			return 12 + boost::get<LocString>(value.data).getWrittenSize();
		case GFF3Struct::kFieldTypeResRef:
			if (value.isRaw)
				return 1 + MIN<size_t>(255, boost::get<VoidData>(value.data).data->size());
			else
				return 1 + MIN<size_t>(255, boost::get<Common::UString>(value.data).size());
		case GFF3Struct::kFieldTypeVector:
			return 12;
		case GFF3Struct::kFieldTypeOrientation:
			return 16;
		case GFF3Struct::kFieldTypeVoid:
			return 4 + boost::get<VoidData>(value.data).data->size();
		default:
			return 0;
	}
}

size_t GFF3Writer::createField(GFF3Struct::FieldType type, const Common::UString &label) {
	// Create a field index
	size_t index = _fields.size();

	// Create field
	GFF3Writer::FieldPtr field = boost::make_shared<Field>();
	field->value.type = type;
	field->labelIndex = addLabel(label);
	_fields.push_back(field);

	return index;
}

GFF3WriterStructPtr GFF3WriterList::addStruct(const Common::UString &label) {
	return addStruct(label, static_cast<uint32_t>(_parent->_structs.size()) - 1);
}

GFF3WriterStructPtr GFF3WriterList::addStruct(const Common::UString &label, uint32_t id) {
	// Create the structure pointer
	GFF3WriterStructPtr strct(
			boost::make_shared<GFF3WriterStruct>(_parent, id));

	// Create a field index
	_strcts.push_back(_parent->_structs.size());
	GFF3Writer::FieldPtr field = boost::make_shared<GFF3Writer::Field>();
	field->value.type = GFF3Struct::kFieldTypeStruct;
	field->value.data = static_cast<uint32_t>(_parent->_structs.size());

	// Add the label
	field->labelIndex = _parent->addLabel(label);

	// Insert the newly created struct into the struct vector
	_parent->_structs.push_back(strct);

	// Insert the struct to the field vector
	_parent->_fields.push_back(field);

	return strct;
}

size_t GFF3WriterList::getSize() const {
	return _strcts.size();
}

GFF3WriterList::GFF3WriterList(GFF3Writer *parent) : _parent(parent) {
}

uint32_t GFF3WriterStruct::getID() const {
	return _id;
}

size_t GFF3WriterStruct::getFieldCount() const {
	return _fieldIndices.size();
}

GFF3WriterStructPtr GFF3WriterStruct::addStruct(const Common::UString &label) {
	return addStruct(label, static_cast<uint32_t>(_parent->_structs.size()) - 1);
}

GFF3WriterStructPtr GFF3WriterStruct::addStruct(const Common::UString &label, uint32_t id) {
	// Create the structure pointer
	GFF3WriterStructPtr strct(
			boost::make_shared<GFF3WriterStruct>(_parent, id));

	// Create a field index
	_fieldIndices.push_back(_parent->_fields.size());
	GFF3Writer::FieldPtr field = boost::make_shared<GFF3Writer::Field>();
	field->value.type = GFF3Struct::kFieldTypeStruct;
	field->value.data = static_cast<uint32_t>(_parent->_structs.size());

	// Add the label
	field->labelIndex = _parent->addLabel(label);

	// Insert the newly created struct into the struct vector
	_parent->_structs.push_back(strct);

	// Insert the struct to the field vector
	_parent->_fields.push_back(field);

	return strct;
}

GFF3WriterListPtr GFF3WriterStruct::addList(const Common::UString &label) {
	// Create the list pointer
	GFF3WriterListPtr strct(boost::make_shared<GFF3WriterList>(_parent));

	// Create a field index
	_fieldIndices.push_back(_parent->_fields.size());
	GFF3Writer::FieldPtr field = boost::make_shared<GFF3Writer::Field>();
	field->value.type = GFF3Struct::kFieldTypeList;
	field->value.data = static_cast<uint32_t>(_parent->_lists.size());

	// Add the label
	field->labelIndex = _parent->addLabel(label);

	// Insert the newly created list into the lists vector
	_parent->_lists.push_back(strct);

	// Insert the list to the field vector
	_parent->_fields.push_back(field);

	return strct;
}

void GFF3WriterStruct::addByte(const Common::UString &label, uint8_t value) {
	createField(GFF3Struct::kFieldTypeByte, label)->value.data = static_cast<uint32_t>(value);
}

void GFF3WriterStruct::addChar(const Common::UString &label, int8_t value) {
	createField(GFF3Struct::kFieldTypeChar, label)->value.data = static_cast<int32_t>(value);
}

void GFF3WriterStruct::addFloat(const Common::UString &label, float value) {
	createField(GFF3Struct::kFieldTypeFloat, label)->value.data = value;
}

void GFF3WriterStruct::addDouble(const Common::UString &label, double value) {
	createField(GFF3Struct::kFieldTypeDouble, label)->value.data = value;
}

void GFF3WriterStruct::addUint16(const Common::UString &label, uint16_t value) {
	createField(GFF3Struct::kFieldTypeUint16, label)->value.data = static_cast<uint32_t>(value);
}

void GFF3WriterStruct::addUint32(const Common::UString &label, uint32_t value) {
	createField(GFF3Struct::kFieldTypeUint32, label)->value.data = value;
}

void GFF3WriterStruct::addUint64(const Common::UString &label, uint64_t value) {
	createField(GFF3Struct::kFieldTypeUint64, label)->value.data = value;
}

void GFF3WriterStruct::addSint16(const Common::UString &label, int16_t value) {
	createField(GFF3Struct::kFieldTypeSint16, label)->value.data = static_cast<int32_t>(value);
}

void GFF3WriterStruct::addSint32(const Common::UString &label, int32_t value) {
	createField(GFF3Struct::kFieldTypeSint32, label)->value.data = value;
}

void GFF3WriterStruct::addSint64(const Common::UString &label, int64_t value) {
	createField(GFF3Struct::kFieldTypeSint64, label)->value.data = value;
}

void GFF3WriterStruct::addExoString(const Common::UString &label, const Common::UString &value) {
	createField(GFF3Struct::kFieldTypeExoString, label)->value.data = value;
}

void GFF3WriterStruct::addExoString(const Common::UString &label, Common::SeekableReadStream *value) {
	GFF3Writer::FieldPtr field = createField(GFF3Struct::kFieldTypeExoString, label);

	field->value.data = GFF3Writer::VoidData(value);
	field->value.isRaw = true;
}

void GFF3WriterStruct::addStrRef(const Common::UString &label, uint32_t value) {
	createField(GFF3Struct::kFieldTypeStrRef, label)->value.data = value;
}

void GFF3WriterStruct::addResRef(const Common::UString &label, const Common::UString &value) {
	createField(GFF3Struct::kFieldTypeResRef, label)->value.data = value;
}

void GFF3WriterStruct::addResRef(const Common::UString &label, Common::SeekableReadStream *value) {
	GFF3Writer::FieldPtr field = createField(GFF3Struct::kFieldTypeResRef, label);

	field->value.data = GFF3Writer::VoidData(value);
	field->value.isRaw = true;
}

void GFF3WriterStruct::addVoid(const Common::UString &label, Common::SeekableReadStream *value) {
	GFF3Writer::FieldPtr field = createField(GFF3Struct::kFieldTypeVoid, label);

	field->value.data = GFF3Writer::VoidData(value);
	field->value.isRaw = true;
}

void GFF3WriterStruct::addVector(const Common::UString &label, glm::vec3 value) {
	createField(GFF3Struct::kFieldTypeVector, label)->value.data = glm::vec4(value, 0.0f);
}

void GFF3WriterStruct::addOrientation(const Common::UString &label, glm::vec4 value) {
	createField(GFF3Struct::kFieldTypeOrientation, label)->value.data = value;
}

void GFF3WriterStruct::addLocString(const Common::UString &label, const LocString &value) {
	createField(GFF3Struct::kFieldTypeLocString, label)->value.data = value;
}

GFF3Writer::FieldPtr GFF3WriterStruct::createField(GFF3Struct::FieldType type, const Common::UString &label) {
	size_t index = _parent->createField(type, label);
	_fieldIndices.push_back(index);
	return _parent->_fields.back();
}

GFF3WriterStruct::GFF3WriterStruct(GFF3Writer *parent, uint32_t id) : _id(id), _parent(parent) {
}

} // End of namespace Aurora

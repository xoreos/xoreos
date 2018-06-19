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

GFF3Writer::GFF3Writer(uint32 id, uint32 version) : _id(id), _version(version) {
	_structs.push_back(boost::make_shared<GFF3WriterStruct>(this));
}

GFF3WriterStructPtr GFF3Writer::getTopLevel() {
	return _structs[0];
}

void GFF3Writer::write(Common::WriteStream &stream) {
	stream.writeUint32BE(_id);
	stream.writeUint32BE(_version);

	uint32 structOffset = 56; // ID + version + header
	uint32 structCount = static_cast<uint32>(_structs.size());

	uint32 fieldOffset = structOffset + structCount * 12;
	uint32 fieldCount = static_cast<uint32>(_fields.size());

	uint32 labelOffset = fieldOffset + fieldCount * 12;
	uint32 labelCount = static_cast<uint32>(_labels.size());

	uint32 fieldDataOffset = labelOffset + labelCount * 16;
	uint32 fieldDataCount = 0;

	// Count the total size of field data
	for (size_t i = 0; i < _fields.size(); ++i)
		fieldDataCount += getFieldDataSize(_fields[i]);

	uint32 fieldIndicesOffset = fieldDataOffset + fieldDataCount;
	uint32 fieldIndicesCount = 0;

	// Count all fields of structs with more than one field
	for (size_t i = 0; i < _structs.size(); ++i) {
		const GFF3WriterStructPtr strct = _structs[i];
		if (strct->getFieldCount() <= 1)
			continue;

		fieldIndicesCount += strct->getFieldCount() * 4;
	}

	uint32 listIndicesOffset = fieldIndicesOffset + fieldIndicesCount;
	uint32 listIndicesCount = 0;

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
	for (size_t i = 0; i < _fields.size(); ++i) {
		FieldPtr field = _fields[i];
		stream.writeUint32LE(field->type);
		stream.writeUint32LE(field->labelIndex);

		/* Determine if this field has simple values (less equal 32 bit) which are written in the field
		 * or complex values, bigger than 32bit like strings written in the field data section. */
		const bool simple =
			field->type == GFF3Struct::kFieldTypeByte ||
			field->type == GFF3Struct::kFieldTypeChar ||
			field->type == GFF3Struct::kFieldTypeUint16 ||
			field->type == GFF3Struct::kFieldTypeUint32 ||
			field->type == GFF3Struct::kFieldTypeStruct ||
			field->type == GFF3Struct::kFieldTypeSint16 ||
			field->type == GFF3Struct::kFieldTypeSint32 ||
			field->type == GFF3Struct::kFieldTypeFloat ||
			field->type == GFF3Struct::kFieldTypeList;

		if (simple) {
			// If the values are simple (less equal 4 bytes) write them to the field
			switch (field->type) {
				case GFF3Struct::kFieldTypeByte:
				case GFF3Struct::kFieldTypeUint16:
				case GFF3Struct::kFieldTypeUint32:
				case GFF3Struct::kFieldTypeStruct:
					stream.writeUint32LE(field->uint32Value);
					break;
				case GFF3Struct::kFieldTypeList:
					stream.writeUint32LE(listDataIndex * 4);
					listDataIndex += 1 + _lists[field->uint32Value]->getSize();
					break;
				case GFF3Struct::kFieldTypeChar:
				case GFF3Struct::kFieldTypeSint16:
				case GFF3Struct::kFieldTypeSint32:
					stream.writeSint32LE(field->int32Value);
					break;
				case GFF3Struct::kFieldTypeFloat:
					stream.writeIEEEFloatLE(field->floatValue);
					break;
				default:
					throw Common::Exception("Invalid Field type");
			}
		} else {
			// If the values are complex (greater then 4 bytes) write the index to the field data
			stream.writeUint32LE(fieldDataIndex);
			fieldDataIndex += getFieldDataSize(field);
		}
	}

	// Write labels
	for (size_t i = 0; i < _labels.size(); ++i) {
		const Common::UString &label = _labels[i];
		stream.write(label.c_str(), MIN<size_t>(label.size(), 16));
		stream.writeZeros(16 - MIN<size_t>(label.size(), 16));
	}

	// Write field data
	for (size_t i = 0; i < _fields.size(); ++i) {
		FieldPtr field = _fields[i];
		switch (field->type) {
			case GFF3Struct::kFieldTypeUint64:
				stream.writeUint64LE(field->uint64Value);
				break;
			case GFF3Struct::kFieldTypeSint64:
				stream.writeSint64LE(field->int64Value);
				break;
			case GFF3Struct::kFieldTypeDouble:
				stream.writeIEEEDoubleLE(field->doubleValue);
				break;
			case GFF3Struct::kFieldTypeStrRef:
				stream.writeUint32LE(4);
				stream.writeUint32LE(field->uint32Value);
				break;
			case GFF3Struct::kFieldTypeResRef:
				stream.writeByte(MIN<byte>(16, field->stringValue.size()));
				stream.write(field->stringValue.c_str(), MIN<size_t>(field->stringValue.size(), 16));
				break;
			case GFF3Struct::kFieldTypeExoString:
				stream.writeUint32LE(static_cast<uint32>(field->stringValue.size()));
				stream.writeString(field->stringValue);
				break;
			case GFF3Struct::kFieldTypeLocString:
				stream.writeUint32LE(field->locStringValue.getWrittenSize() + 8);
				stream.writeUint32LE(field->locStringValue.getID());
				stream.writeUint32LE(field->locStringValue.getNumStrings());
				field->locStringValue.writeLocString(stream);
				break;
			case GFF3Struct::kFieldTypeVoid:
				stream.writeUint32LE(static_cast<uint32>(field->voidSize));
				stream.write(field->voidData.get(), field->voidSize);
				break;
			case GFF3Struct::kFieldTypeVector:
				stream.writeIEEEFloatLE(field->vectorValue.x);
				stream.writeIEEEFloatLE(field->vectorValue.y);
				stream.writeIEEEFloatLE(field->vectorValue.z);
				break;
			case GFF3Struct::kFieldTypeOrientation:
				stream.writeIEEEFloatLE(field->vectorValue.x);
				stream.writeIEEEFloatLE(field->vectorValue.y);
				stream.writeIEEEFloatLE(field->vectorValue.z);
				stream.writeIEEEFloatLE(field->vectorValue.w);
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

uint32 GFF3Writer::addLabel(const Common::UString &label) {
	std::vector<Common::UString>::iterator iter = std::find(_labels.begin(), _labels.end(), label);
	if (iter != _labels.end()) {
		return static_cast<uint32>(std::distance(_labels.begin(), iter));
	} else {
		_labels.push_back(label);
		return static_cast<uint32>(_labels.size() - 1);
	}
}

uint32 GFF3Writer::getFieldDataSize(FieldPtr field) {
	switch (field->type) {
		case GFF3Struct::kFieldTypeUint64:
		case GFF3Struct::kFieldTypeSint64:
		case GFF3Struct::kFieldTypeStrRef:
		case GFF3Struct::kFieldTypeDouble:
			return 8;
		case GFF3Struct::kFieldTypeExoString:
			return 4 + field->stringValue.size();
		case GFF3Struct::kFieldTypeLocString:
			return 12 + field->locStringValue.getWrittenSize();
		case GFF3Struct::kFieldTypeResRef:
			return 1 + field->stringValue.size();
		case GFF3Struct::kFieldTypeVector:
			return 12;
		case GFF3Struct::kFieldTypeOrientation:
			return 16;
		case GFF3Struct::kFieldTypeVoid:
			return 4 + field->voidSize;
		default:
			return 0;
	}
}

size_t GFF3Writer::createField(GFF3Struct::FieldType type, const Common::UString &label) {
	// Create a field index
	size_t index = _fields.size();

	// Create field
	GFF3Writer::FieldPtr field = boost::make_shared<Field>();
	field->type = type;
	field->labelIndex = addLabel(label);
	_fields.push_back(field);

	return index;
}

GFF3WriterStructPtr GFF3WriterList::addStruct(const Common::UString &label) {
	// Create the structure pointer
	GFF3WriterStructPtr strct(
			boost::make_shared<GFF3WriterStruct>(_parent, static_cast<uint32>(_parent->_structs.size()) - 1));

	// Create a field index
	_strcts.push_back(_parent->_structs.size());
	GFF3Writer::FieldPtr field = boost::make_shared<GFF3Writer::Field>();
	field->type = GFF3Struct::kFieldTypeStruct;
	field->uint32Value = static_cast<uint32>(_parent->_structs.size());

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

uint32 GFF3WriterStruct::getID() const {
	return _id;
}

size_t GFF3WriterStruct::getFieldCount() const {
	return _fieldIndices.size();
}

GFF3WriterStructPtr GFF3WriterStruct::addStruct(const Common::UString &label) {
	// Create the structure pointer
	GFF3WriterStructPtr strct(
			boost::make_shared<GFF3WriterStruct>(_parent, static_cast<uint32>(_parent->_structs.size()) - 1));

	// Create a field index
	_fieldIndices.push_back(_parent->_fields.size());
	GFF3Writer::FieldPtr field = boost::make_shared<GFF3Writer::Field>();
	field->type = GFF3Struct::kFieldTypeStruct;
	field->uint32Value = static_cast<uint32>(_parent->_structs.size());

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
	field->type = GFF3Struct::kFieldTypeList;
	field->uint32Value = static_cast<uint32>(_parent->_lists.size());

	// Add the label
	field->labelIndex = _parent->addLabel(label);

	// Insert the newly created list into the lists vector
	_parent->_lists.push_back(strct);

	// Insert the list to the field vector
	_parent->_fields.push_back(field);

	return strct;
}

void GFF3WriterStruct::addByte(const Common::UString &label, byte value) {
	createField(GFF3Struct::kFieldTypeByte, label)->uint32Value = value;
}

void GFF3WriterStruct::addChar(const Common::UString &label, char value) {
	createField(GFF3Struct::kFieldTypeChar, label)->int32Value = value;
}

void GFF3WriterStruct::addFloat(const Common::UString &label, float value) {
	createField(GFF3Struct::kFieldTypeFloat, label)->floatValue = value;
}

void GFF3WriterStruct::addDouble(const Common::UString &label, double value) {
	createField(GFF3Struct::kFieldTypeDouble, label)->doubleValue = value;
}

void GFF3WriterStruct::addUint16(const Common::UString &label, uint16 value) {
	createField(GFF3Struct::kFieldTypeUint16, label)->uint32Value = value;
}

void GFF3WriterStruct::addUint32(const Common::UString &label, uint32 value) {
	createField(GFF3Struct::kFieldTypeUint32, label)->uint32Value = value;
}

void GFF3WriterStruct::addUint64(const Common::UString &label, uint64 value) {
	createField(GFF3Struct::kFieldTypeUint64, label)->uint64Value = value;
}

void GFF3WriterStruct::addSint16(const Common::UString &label, int16 value) {
	createField(GFF3Struct::kFieldTypeSint16, label)->int32Value = value;
}

void GFF3WriterStruct::addSint32(const Common::UString &label, int32 value) {
	createField(GFF3Struct::kFieldTypeSint32, label)->int32Value = value;
}

void GFF3WriterStruct::addSint64(const Common::UString &label, int64 value) {
	createField(GFF3Struct::kFieldTypeSint64, label)->int64Value = value;
}

void GFF3WriterStruct::addExoString(const Common::UString &label, const Common::UString &value) {
	createField(GFF3Struct::kFieldTypeExoString, label)->stringValue = value;
}

void GFF3WriterStruct::addStrRef(const Common::UString &label, uint32 value) {
	createField(GFF3Struct::kFieldTypeStrRef, label)->uint32Value = value;
}

void GFF3WriterStruct::addResRef(const Common::UString &label, const Common::UString &value) {
	createField(GFF3Struct::kFieldTypeResRef, label)->stringValue = value;
}

void GFF3WriterStruct::addVoid(const Common::UString &label, const byte *data, uint32 size) {
	GFF3Writer::FieldPtr field = createField(GFF3Struct::kFieldTypeVoid, label);
	field->voidData.reset(new byte[size]);
	field->voidSize = size;
	memcpy(field->voidData.get(), data, size);
}

void GFF3WriterStruct::addVector(const Common::UString &label, glm::vec3 value) {
	createField(GFF3Struct::kFieldTypeVector, label)->vectorValue = glm::vec4(value, 0.0f);
}

void GFF3WriterStruct::addOrientation(const Common::UString &label, glm::vec4 value) {
	createField(GFF3Struct::kFieldTypeOrientation, label)->vectorValue = value;
}

void GFF3WriterStruct::addLocString(const Common::UString &label, const LocString &value) {
	createField(GFF3Struct::kFieldTypeLocString, label)->locStringValue = value;
}

GFF3Writer::FieldPtr GFF3WriterStruct::createField(GFF3Struct::FieldType type, const Common::UString &label) {
	size_t index = _parent->createField(type, label);
	_fieldIndices.push_back(index);
	return _parent->_fields.back();
}

GFF3WriterStruct::GFF3WriterStruct(GFF3Writer *parent, uint32 id) : _id(id), _parent(parent) {
}

} // End of namespace Aurora

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file aurora/gffstructs.cpp
 *  %Common structures found in BioWare's GFF files.
 */

#include <cstring>

#include "common/util.h"

#include "aurora/gffstructs.h"
#include "aurora/error.h"

namespace Aurora {

GFFLocation::GFFLocation(uint32 area, const double *position, const double *orientation) {
	clear();

	_area = area;

	if (position)
		setPosition(position);
	if (orientation)
		setOrientation(orientation);
}

void GFFLocation::clear() {
	_area = kObjectIDInvalid;

	clearPosition();
	clearOrientation();
}

void GFFLocation::clearPosition() {
	_position[0] = 0.0;
	_position[1] = 0.0;
	_position[2] = 0.0;
}

void GFFLocation::clearOrientation() {
	_orientation[0] = 0.0;
	_orientation[1] = 0.0;
	_orientation[2] = 0.0;
}

uint32 GFFLocation::getArea() const {
	return _area;
}

const double *GFFLocation::getPosition() const {
	return _position;
}

const double *GFFLocation::getOrientation() const {
	return _orientation;
}

void GFFLocation::setArea(uint32 area) {
	_area = area;
}

void GFFLocation::setPosition(const double *position) {
	std::memcpy(_position, position, 3 * sizeof(double));
}

void GFFLocation::setOrientation(const double *orientation) {
	std::memcpy(_orientation, orientation, 3 * sizeof(double));
}

void GFFLocation::read(const GFFFile::StructRange &range) {
	for (GFFFile::StructIterator it = range.first; it != range.second; ++it) {
		if      (it->getLabel() == "Area")
			_area           = it->getUint();
		else if (it->getLabel() == "PositionX")
			_position[0]    = it->getDouble();
		else if (it->getLabel() == "PositionY")
			_position[1]    = it->getDouble();
		else if (it->getLabel() == "PositionZ")
			_position[2]    = it->getDouble();
		else if (it->getLabel() == "OrientationX")
			_orientation[0] = it->getDouble();
		else if (it->getLabel() == "OrientationY")
			_orientation[1] = it->getDouble();
		else if (it->getLabel() == "OrientationZ")
			_orientation[2] = it->getDouble();
	}
}


GFFVariable::GFFVariable() {
	_type = kTypeNone;
}

GFFVariable::GFFVariable(const GFFVariable &var) {
	_type = kTypeNone;

	*this = var;
}

GFFVariable::~GFFVariable() {
	clear();
}

GFFVariable &GFFVariable::operator=(const GFFVariable &var) {
	clear();

	_type  = var._type;
	_value = var._value;

	if      (_type == kTypeString)
		_value.typeString = new Common::UString(*_value.typeString);
	else if (_type == kTypeLocation)
		_value.typeLocation = new GFFLocation(*_value.typeLocation);

	return *this;
}

void GFFVariable::clear() {
	if      (_type == kTypeString)
		delete _value.typeString;
	else if (_type == kTypeLocation)
		delete _value.typeLocation;

	_type = kTypeNone;
}

GFFVariable::Type GFFVariable::getType() const {
	return _type;
}

int32 GFFVariable::getInt() const {
	if (_type != kTypeInt)
		throw kGFFFieldTypeError;

	return _value.typeInt;
}

float GFFVariable::getFloat() const {
	if (_type != kTypeFloat)
		throw kGFFFieldTypeError;

	return _value.typeFloat;
}

uint32 GFFVariable::getObjectID() const {
	if (_type != kTypeObjectID)
		throw kGFFFieldTypeError;

	return _value.typeObjectID;
}

const Common::UString &GFFVariable::getString() const {
	if (_type != kTypeString)
		throw kGFFFieldTypeError;

	return *_value.typeString;
}

const GFFLocation &GFFVariable::getLocation() const {
	if (_type != kTypeLocation)
		throw kGFFFieldTypeError;

	return *_value.typeLocation;
}

GFFLocation &GFFVariable::getLocation() {
	if (_type != kTypeLocation)
		throw kGFFFieldTypeError;

	return *_value.typeLocation;
}

void GFFVariable::setInt(int32 v) {
	clear();

	_type = kTypeInt;
	_value.typeInt = v;
}

void GFFVariable::setFloat(float v) {
	clear();

	_type = kTypeFloat;
	_value.typeFloat = v;
}

void GFFVariable::setObjectID(uint32 v) {
	clear();

	_type = kTypeObjectID;
	_value.typeObjectID = v;
}

void GFFVariable::setString(const Common::UString &v) {
	clear();

	_type = kTypeString;
	_value.typeString = new Common::UString(v);
}

void GFFVariable::setLocation(const GFFLocation &v) {
	clear();

	_type = kTypeLocation;
	_value.typeLocation = new GFFLocation(v);
}

void GFFVariable::read(const GFFFile::StructRange &range, Common::UString &name) {
	clear();

	Type type = kTypeNone;

	for (GFFFile::StructIterator it = range.first; it != range.second; ++it) {
		if      (it->getLabel() == "Name")
			name = it->getString();
		else if (it->getLabel() == "Type")
			type = (Type) it->getUint();
	}

	if ((type <= kTypeNone) || (type > kTypeLocation))
		throw Common::Exception("GFFVariable::read(): Unknown variable type %d", _type);

	for (GFFFile::StructIterator it = range.first; it != range.second; ++it) {
		if (it->getLabel() == "Value") {
			if        (type == kTypeInt) {
				setInt(it->getSint());
			} else if (type == kTypeFloat) {
				setFloat(it->getDouble());
			} else if (type == kTypeString) {
				setString(it->getString());
			} else if (type == kTypeObjectID) {
				setObjectID(it->getUint());
			} else if (type == kTypeLocation) {
				_type = kTypeLocation;
				_value.typeLocation = new GFFLocation();
				_value.typeLocation->read(it.structRange(it->getStructIndex()));
			}
		}
	}

}


GFFVarTable::GFFVarTable() {
}

GFFVarTable::~GFFVarTable() {
	clear();
}

void GFFVarTable::clear() {
	for (VarMap::iterator it = _variables.begin(); it != _variables.end(); ++it)
		delete it->second;

	_variables.clear();
}

bool GFFVarTable::has(const Common::UString &name) const {
	return _variables.find(name) != _variables.end();
}

const GFFVariable *GFFVarTable::get(const Common::UString &name) const {
	VarMap::const_iterator it = _variables.find(name);
	if (it == _variables.end())
		return 0;

	return it->second;
}

GFFVariable *GFFVarTable::get(const Common::UString &name) {
	VarMap::iterator it = _variables.find(name);
	if (it == _variables.end())
		return 0;

	return it->second;
}

void GFFVarTable::set(const Common::UString &name, const GFFVariable &variable) {
	_variables.insert(std::make_pair(name, new GFFVariable(variable)));
}

void GFFVarTable::read(const GFFFile::ListRange &range) {
	for (GFFFile::ListIterator it = range.first; it != range.second; ++it) {
		GFFVariable *variable = new GFFVariable;

		Common::UString name;
		try {
			variable->read(*it, name);
		} catch(...) {
			delete variable;
			throw;
		}

		_variables.insert(std::make_pair(name, variable));
	}
}

} // End of namespace Aurora

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

#include "common/stream.h"

#include "aurora/gfffile.h"
#include "aurora/locstring.h"

namespace Aurora {

GFFField::GFFField() {
	_gffType = kGFFTypeNone;
	_type    = kTypeNone;
}

GFFField::~GFFField() {
	clear();
}

void GFFField::clear() {
	if (_type == kTypeNone)
		return;

	if      (_type == kTypeString)
		delete _value.typeString;
	else if (_type == kTypeLocString)
		delete _value.typeLocString;
	else if (_type == kTypeStruct) {

		if (_value.typeStruct)
			for (GFFStruct::iterator it = _value.typeStruct->begin(); it != _value.typeStruct->end(); ++it)
				delete *it;
		delete _value.typeStruct;

	} else if (_type == kTypeList) {

		if (_value.typeList)
			for (GFFList::iterator it1 = _value.typeList->begin(); it1 != _value.typeList->end(); ++it1)
				for (GFFStruct::iterator it2 = it1->begin(); it2 != it1->end(); ++it2)
					delete *it2;
		delete _value.typeList;

	}

	_gffType = kGFFTypeNone;
	_type    = kTypeNone;

	_name.clear();
}

GFFField::Type GFFField::getType() const {
	return _type;
}

const std::string &GFFField::getName() const {
	return _name;
}

char GFFField::getChar() const {
	return (char) _value.typeInt;
}

uint64 GFFField::getUnsignedInteger() const {
	return (uint64) _value.typeInt;
}

int64 GFFField::getSignedInteger() const {
	return (int64) _value.typeInt;
}

double GFFField::getDouble() const {
	return _value.typeDouble;
}

const std::string &GFFField::getString() const {
	return *_value.typeString;
}

const LocString &GFFField::getLocString() const {
	return *_value.typeLocString;
}

const GFFField::GFFStruct &GFFField::getStruct() const {
	return *_value.typeStruct;
}

const GFFField::GFFList &GFFField::getList() const {
	return *_value.typeList;
}

} // End of namespace Aurora

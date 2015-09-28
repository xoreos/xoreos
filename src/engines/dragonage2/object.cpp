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
 *  An object in a Dragon Age II area.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"

#include "src/engines/dragonage2/object.h"

namespace Engines {

namespace DragonAge2 {

static const uint32 kVARSID = MKTAG('V', 'A', 'R', 'S');

using ::Aurora::GFF3File;
using ::Aurora::GFF3Struct;
using ::Aurora::GFF3List;

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;

Object::Object(ObjectType type) : _type(type), _static(true), _usable(false) {
	_position[0] = 0.0f;
	_position[1] = 0.0f;
	_position[2] = 0.0f;

	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;
}

Object::~Object() {
}

ObjectType Object::getType() const {
	return _type;
}

void Object::show() {
}

void Object::hide() {
}

const std::list<uint32> &Object::getIDs() const {
	return _ids;
}

const Common::UString &Object::getResRef() const {
	return _resRef;
}

const Aurora::LocString &Object::getName() const {
	return _name;
}

const Aurora::LocString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getNonLocalizedName() const {
	return _nonLocalizedName;
}

void Object::setNonLocalizedName(const Common::UString &name) {
	_nonLocalizedName = name;
}

const Common::UString &Object::getConversation() const {
	return _conversation;
}

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z, float &angle) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];

	angle = _orientation[3];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z, float angle) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = angle;
}

void Object::syncPosition() {
	float x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);
}

void Object::syncOrientation() {
	float x, y, z, angle;

	getOrientation(x, y, z, angle);
	setOrientation(x, y, z, angle);
}

void Object::enter() {
}

void Object::leave() {
}

void Object::highlight(bool UNUSED(enabled)) {
}

bool Object::click(Object *UNUSED(triggerer)) {
	return true;
}

void Object::readVarTable(const GFF3List &varTable) {
	for (GFF3List::const_iterator v = varTable.begin(); v != varTable.end(); ++v) {
		const Common::UString name  = (*v)->getString ("Name");
		const uint8           type  = (*v)->getUint   ("Type");

		if (name.empty())
			continue;

		switch (type) {
			case  0:
			case  4:
				setVariable(name, Aurora::NWScript::Variable());
				break;

			case  1:
				setVariable(name, (int32) (*v)->getSint("Value"));
				break;

			case  2:
				setVariable(name, (float) (*v)->getDouble("Value"));
				break;

			case  3:
			case 12:
				setVariable(name, (*v)->getString("Value"));
				break;

			default:
				throw Common::Exception("Unknown variable type %u (\"%s\")", type, name.c_str());
		}
	}
}

void Object::readVarTable(const Aurora::GFF3Struct &gff) {
	if (gff.hasField("VarTable"))
		readVarTable(gff.getList("VarTable"));
}

void Object::readVarTable(const GFF4List &varTable) {
	for (GFF4List::const_iterator v = varTable.begin(); v != varTable.end(); ++v) {
		if (!*v || ((*v)->getLabel() != kVARSID))
			continue;

		const Common::UString name  = (*v)->getString (kGFF4ScriptVarTableName);
		const uint8           type  = (*v)->getUint   (kGFF4ScriptVarTableType);
		const GFF4Struct     *value = (*v)->getGeneric(kGFF4ScriptVarTableValue);

		if (name.empty() || (type == 0) || !value || !value->hasField(0))
			continue;

		switch (type) {
			case  4:
				setVariable(name, Aurora::NWScript::Variable());
				break;

			case  1:
				setVariable(name, (int32) value->getSint(0));
				break;

			case  2:
				setVariable(name, (float) value->getDouble(0));
				break;

			case  3:
			case 12:
				setVariable(name, value->getString(0));
				break;

			default:
				throw Common::Exception("Unknown variable type %u (\"%s\")", type, name.c_str());
		}
	}
}

void Object::readVarTable(const Aurora::GFF4Struct &gff) {
	if (gff.hasField(kGFF4ScriptVarTable))
		readVarTable(gff.getList(kGFF4ScriptVarTable));
}

} // End of namespace DragonAge2

} // End of namespace Engines

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
 *  NWN script functions.
 */

#include "src/common/util.h"

#include "src/aurora/language.h"

#include "src/aurora/nwscript/variable.h"
#include "src/aurora/nwscript/functionman.h"

#include "src/graphics/graphics.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/nwn.h"
#include "src/engines/nwn/module.h"
#include "src/engines/nwn/area.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/waypoint.h"
#include "src/engines/nwn/door.h"
#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/location.h"

#include "src/engines/nwn/script/functions.h"

using Aurora::kObjectIDInvalid;

namespace Engines {

namespace NWN {

ObjectDistanceSort::ObjectDistanceSort(const Object &target) {
	target.getPosition(xt, yt, zt);
}

bool ObjectDistanceSort::operator()(Object *a, Object *b) {
	return getDistance(*a) < getDistance(*b);
}

float ObjectDistanceSort::getDistance(Object &a) {
	float x, y, z;
	a.getPosition(x, y, z);

	return ABS(x - xt) + ABS(y - yt) + ABS(z - zt);
}


ScriptFunctions::Defaults::Defaults() {
	int0             = new Aurora::NWScript::Variable(0);
	int1             = new Aurora::NWScript::Variable(1);
	int100           = new Aurora::NWScript::Variable(100);
	int18            = new Aurora::NWScript::Variable(18);
	int9             = new Aurora::NWScript::Variable(9);
	intm1            = new Aurora::NWScript::Variable(-1);
	intMale          = new Aurora::NWScript::Variable((int) Aurora::kLanguageGenderMale);
	intObjectTypeAll = new Aurora::NWScript::Variable(kObjectTypeAll);

	intACAll            = new Aurora::NWScript::Variable(0); // TODO
	intACDodge          = new Aurora::NWScript::Variable(0); // TODO
	intAlignAll         = new Aurora::NWScript::Variable(0); // TODO
	intAttackMisc       = new Aurora::NWScript::Variable(0); // TODO
	intCameraSnap       = new Aurora::NWScript::Variable(0); // TODO
	intClassInvalid     = new Aurora::NWScript::Variable(0); // TODO
	intCreatureLevel    = new Aurora::NWScript::Variable(0); // TODO
	intDamageMagical    = new Aurora::NWScript::Variable(0); // TODO
	intDamageNormal     = new Aurora::NWScript::Variable(0); // TODO
	intFactionHostile   = new Aurora::NWScript::Variable(0); // TODO
	intFirstGenericMale = new Aurora::NWScript::Variable(0); // TODO
	intIPAmmo           = new Aurora::NWScript::Variable(0); // TODO
	intMetaMagicAny     = new Aurora::NWScript::Variable(0); // TODO
	intMissNormal       = new Aurora::NWScript::Variable(0); // TODO
	intObjectCreature   = new Aurora::NWScript::Variable(0); // TODO
	intPackageInvalid   = new Aurora::NWScript::Variable(0); // TODO
	intPersZoneActive   = new Aurora::NWScript::Variable(0); // TODO
	intProjPathDefault  = new Aurora::NWScript::Variable(0); // TODO
	intSaveAll          = new Aurora::NWScript::Variable(0); // TODO
	intSaveNone         = new Aurora::NWScript::Variable(0); // TODO
	intSpellsGeneral    = new Aurora::NWScript::Variable(0); // TODO
	intSpellsAll        = new Aurora::NWScript::Variable(0); // TODO
	intTalk             = new Aurora::NWScript::Variable(0); // TODO
	intVFXNone          = new Aurora::NWScript::Variable(0); // TODO

	float0_0   = new Aurora::NWScript::Variable(0.0f);
	float1_0   = new Aurora::NWScript::Variable(1.0f);
	float2_0   = new Aurora::NWScript::Variable(2.0f);
	float30_0  = new Aurora::NWScript::Variable(30.f);
	float40_0  = new Aurora::NWScript::Variable(40.f);
	float180_0 = new Aurora::NWScript::Variable(180.f);
	floatm1_0  = new Aurora::NWScript::Variable(-1.0f);

	floatMedium = new Aurora::NWScript::Variable(0.0f); // TODO

	object0     = new Aurora::NWScript::Variable((Aurora::NWScript::Object *) 0);
	stringEmpty = new Aurora::NWScript::Variable("");
	vector0     = new Aurora::NWScript::Variable(Aurora::NWScript::kTypeVector);
}

ScriptFunctions::Defaults::~Defaults() {
	delete int0;
	delete int1;
	delete int100;
	delete int18;
	delete int9;
	delete intm1;
	delete intMale;
	delete intObjectTypeAll;

	delete intACAll;
	delete intACDodge;
	delete intAlignAll;
	delete intAttackMisc;
	delete intCameraSnap;
	delete intClassInvalid;
	delete intCreatureLevel;
	delete intDamageMagical;
	delete intDamageNormal;
	delete intFactionHostile;
	delete intFirstGenericMale;
	delete intIPAmmo;
	delete intMetaMagicAny;
	delete intMissNormal;
	delete intObjectCreature;
	delete intPackageInvalid;
	delete intPersZoneActive;
	delete intProjPathDefault;
	delete intSaveAll;
	delete intSaveNone;
	delete intSpellsGeneral;
	delete intSpellsAll;
	delete intTalk;
	delete intVFXNone;

	delete float0_0;
	delete float1_0;
	delete float2_0;
	delete float30_0;
	delete float40_0;
	delete float180_0;
	delete floatm1_0;

	delete floatMedium;

	delete object0;
	delete stringEmpty;
	delete vector0;
}


ScriptFunctions::ScriptFunctions(NWNEngine &engine) : _engine(&engine) {
	registerFunctions();
}

ScriptFunctions::~ScriptFunctions() {
	FunctionMan.clear();
}

Common::UString ScriptFunctions::floatToString(float f, int width, int decimals) {
	return Common::UString::format("%*.*f", width, decimals, f);
}

void ScriptFunctions::registerFunctions() {
	Defaults defaults;

	registerFunctions000(defaults);
	registerFunctions100(defaults);
	registerFunctions200(defaults);
	registerFunctions300(defaults);
	registerFunctions400(defaults);
	registerFunctions500(defaults);
	registerFunctions600(defaults);
	registerFunctions700(defaults);
	registerFunctions800(defaults);
}

int32 ScriptFunctions::random(int min, int max, int32 n) {
	if (n < 1)
		n = 1;

	int32 r = 0;

	while (n-- > 0)
		r += std::rand() % (max - min + 1) + min;

	return r;
}

Aurora::NWScript::Object *ScriptFunctions::getPC() {
	if (!_engine->getModule())
		return 0;

	return (Aurora::NWScript::Object *) _engine->getModule()->getPC();
}

Common::UString ScriptFunctions::gTag(const Aurora::NWScript::Object *o) {
	if (o)
		return Common::UString::format("\"%s\"", o->getTag().c_str());

	return "0";
}

Object *ScriptFunctions::convertObject(Aurora::NWScript::Object *o) {
	Object *object = dynamic_cast<Object *>(o);
	if (!object || (object->getID() == kObjectIDInvalid))
		return 0;

	return object;
}

Waypoint *ScriptFunctions::convertWaypoint(Aurora::NWScript::Object *o) {
	Waypoint *waypoint = dynamic_cast<Waypoint *>(o);
	if (!waypoint || (waypoint->getID() == kObjectIDInvalid))
		return 0;

	return waypoint;
}

Situated *ScriptFunctions::convertSituated(Aurora::NWScript::Object *o) {
	Situated *situated = dynamic_cast<Situated *>(o);
	if (!situated || (situated->getID() == kObjectIDInvalid))
		return 0;

	return situated;
}

Door *ScriptFunctions::convertDoor(Aurora::NWScript::Object *o) {
	Door *door = dynamic_cast<Door *>(o);
	if (!door || (door->getID() == kObjectIDInvalid))
		return 0;

	return door;
}

Creature *ScriptFunctions::convertCreature(Aurora::NWScript::Object *o) {
	Creature *creature = dynamic_cast<Creature *>(o);
	if (!creature || (creature->getID() == kObjectIDInvalid))
		return 0;

	return creature;
}

Creature *ScriptFunctions::convertPC(Aurora::NWScript::Object *o) {
	Creature *pc = dynamic_cast<Creature *>(o);
	if (!pc || (pc->getID() == kObjectIDInvalid) || !pc->isPC())
		return 0;

	return pc;
}

Area *ScriptFunctions::convertArea(Aurora::NWScript::Object *o) {
	return dynamic_cast<Area *>(o);
}

Module *ScriptFunctions::convertModule(Aurora::NWScript::Object *o) {
	return dynamic_cast<Module *>(o);
}

Location *ScriptFunctions::convertLocation(Aurora::NWScript::EngineType *e) {
	return dynamic_cast<Location *>(e);
}

void ScriptFunctions::jumpTo(Object *object, Area *area, float x, float y, float z) {
	// Sanity check
	if (!object->getArea() || !area) {
		warning("ScriptFunctions::jumpTo(): No area?!? (%d, %d)",
		        object->getArea() != 0, area != 0);
		return;
	}

	GfxMan.lockFrame();

	// Are we moving between areas?
	if (object->getArea() != area) {
		const Common::UString &areaFrom = object->getArea()->getResRef();
		const Common::UString &areaTo   = area->getResRef();

		warning("TODO: ScriptFunctions::jumpTo(): Moving from \"%s\" to \"%s\"",
		        areaFrom.c_str(), areaTo.c_str());

		Object *pc = convertObject(getPC());
		if (pc) {
			const Common::UString &pcArea = pc->getArea()->getResRef();

			if (areaFrom == pcArea) {
				// Moving away from the currently visible area.

				object->hide();
				object->unloadModel();

			} else if (areaTo == pcArea) {
				// Moving into the currently visible area.

				object->loadModel();
				object->show();
			}

		}

		object->setArea(area);
	}

	// Update position
	object->setPosition(x, y, z);

	GfxMan.unlockFrame();

	if (object == getPC() && _engine->getModule())
		_engine->getModule()->movedPC();
}

} // End of namespace NWN

} // End of namespace Engines

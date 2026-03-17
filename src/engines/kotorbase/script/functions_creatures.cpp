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
 *  Star Wars: Knights of the Old Republic engine functions managing creatures.
 */

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotorbase/types.h"
#include "src/engines/kotorbase/object.h"
#include "src/engines/kotorbase/creature.h"
#include "src/engines/kotorbase/objectcontainer.h"
#include "src/engines/kotorbase/effect.h"

#include "src/engines/kotorbase/script/functions.h"

namespace Engines {

namespace KotORBase {

void Functions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getGender(): Object is not a creature");

	ctx.getReturn() = creature->getGender();
}

void Functions::getLevelByClass(Aurora::NWScript::FunctionContext &ctx) {
	Class creatureClass = Class(ctx.getParams()[0].getInt());
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getLevelByClass(): Object is not a creature");

	ctx.getReturn() = creature->getLevel(creatureClass);
}

void Functions::getLevelByPosition(Aurora::NWScript::FunctionContext &ctx) {
	int position = ctx.getParams()[0].getInt();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getLevelByPosition(): Object is not a creature");

	ctx.getReturn() = creature->getLevelByPosition(position - 1);
}

void Functions::getClassByPosition(Aurora::NWScript::FunctionContext &ctx) {
	int position = ctx.getParams()[0].getInt();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getClassByPosition(): Object is not a creature");

	ctx.getReturn() = creature->getClassByPosition(position - 1);
}

void Functions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	Race race;
	if (!creature)
		race = kRaceInvalid;
	else
		race = creature->getRace();

	ctx.getReturn() = race;
}

void Functions::getSubRace(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getSubRace(): Object is not a creature");

	ctx.getReturn() = creature->getSubRace();
}

void Functions::getHasSkill(Aurora::NWScript::FunctionContext &ctx) {
	int nSkill = ctx.getParams()[0].getInt();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getHasSkill(): Object is not a creature");

	ctx.getReturn() = creature->getSkillRank(KotORBase::Skill(nSkill)) > 0;
}

void Functions::getSkillRank(Aurora::NWScript::FunctionContext &ctx) {
	int nSkill = ctx.getParams()[0].getInt();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getHasSkill(): Object is not a creature");

	ctx.getReturn() = creature->getSkillRank(KotORBase::Skill(nSkill));
}

void Functions::getAbilityScore(Aurora::NWScript::FunctionContext &ctx) {
	int nAbilityType = ctx.getParams()[0].getInt();
	Aurora::NWScript::Object *object = ctx.getParams()[1].getObject();

	Creature *creature = ObjectContainer::toCreature(object);

	if (!creature)
		throw Common::Exception("Functions::getHasSkill(): Object is not a creature");

	ctx.getReturn() = creature->getAbilityScore(KotORBase::Ability(nAbilityType));
}

void Functions::getIsDead(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	ctx.getReturn() = creature ? (int32_t)creature->isDead() : 0;
}

void Functions::getHitDice(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	ctx.getReturn() = creature ? creature->getHitDice() : 0;
}

void Functions::getAC(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	ctx.getReturn() = creature ? creature->getAC() : 10;
}

void Functions::getAttackTarget(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!creature)
		return;

	ctx.getReturn() = creature->getAttackTarget();
}

void Functions::getIsInCombat(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = ObjectContainer::toCreature(getParamObject(ctx, 0));
	ctx.getReturn() = creature ? (int32_t)creature->isInCombat() : 0;
}

void Functions::getLastHostileActor(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = static_cast<Aurora::NWScript::Object *>(nullptr);

	// Parameter 0 defaults to OBJECT_SELF
	Aurora::NWScript::Object *rawParam = ctx.getParams()[0].getObject();
	Creature *creature = ObjectContainer::toCreature(rawParam ? rawParam : ctx.getCaller());
	if (!creature)
		return;

	ctx.getReturn() = creature->getLastHostileActor();
}

void Functions::effectHeal(Aurora::NWScript::FunctionContext &ctx) {
	int amount = ctx.getParams()[0].getInt();
	ctx.getReturn() = new Effect(kEffectHeal, amount);
}

void Functions::effectDamage(Aurora::NWScript::FunctionContext &ctx) {
	int amount = ctx.getParams()[0].getInt();
	int damageType = ctx.getParams()[1].getInt();
	// param 2 (damage power) is not used in the basic implementation
	ctx.getReturn() = new Effect(kEffectDamage, amount, damageType);
}

void Functions::applyEffectToObject(Aurora::NWScript::FunctionContext &ctx) {
	// int nDurationType, effect eEffect, object oTarget, float fDuration=0.0
	const Effect *effect = dynamic_cast<const Effect *>(ctx.getParams()[1].getEngineType());
	Object *target = ObjectContainer::toObject(ctx.getParams()[2].getObject());

	if (!effect || !target)
		return;

	int current = target->getCurrentHitPoints();

	if (effect->getType() == kEffectHeal) {
		int healed = current + effect->getAmount();
		target->setCurrentHitPoints(healed);
	} else if (effect->getType() == kEffectDamage) {
		int damaged = current - effect->getAmount();
		target->setCurrentHitPoints(damaged);

		// Check for death on creatures
		Creature *creature = ObjectContainer::toCreature(target);
		if (creature)
			creature->handleDeath();
	}
}

} // End of namespace KotORBase

} // End of namespace Engines

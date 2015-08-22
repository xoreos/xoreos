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
 *  Neverwinter Nights engine functions messing with creatures.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

void Functions::getAbilityScore(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: nBaseAbilityScore

	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Ability   ability  = (Ability) ctx.getParams()[1].getInt();

	ctx.getReturn() = creature ? (int32) creature->getAbility(ability) : 0;
}

void Functions::getSkillRank(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->getSkillRank(ctx.getParams()[0].getInt()) : -1;
}

void Functions::getHasFeat(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->hasFeat(ctx.getParams()[0].getInt()) : 0;
}

void Functions::getClassByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32) kClassInvalid;

	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (!creature)
		return;

	uint32 classID;
	uint16 level;
	creature->getClass(MAX<int32>(ctx.getParams()[0].getInt() - 1, 0), classID, level);

	ctx.getReturn() = (int32) classID;
}

void Functions::getLevelByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (!creature)
		return;

	uint32 classID;
	uint16 level;
	creature->getClass(MAX<int32>(ctx.getParams()[0].getInt() - 1, 0), classID, level);

	ctx.getReturn() = (int32) level;
}

void Functions::getLevelByClass(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->getClassLevel(ctx.getParams()[0].getInt()) : 0;
}

void Functions::getXP(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (creature ? creature->getXP() : 0);
}

void Functions::getIsDead(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (creature->getCurrentHP() <= 0) : false;
}

void Functions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = NWN::ObjectContainer::toPC(getParamObject(ctx, 0)) != 0;
}

void Functions::getIsDM(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->isDM();
}

void Functions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (creature ? creature->getGender() : kGenderNone);
}

void Functions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (creature ? creature->getRace() : kRaceInvalid);
}

void Functions::getHitDice(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? creature->getHitDice() : 0;
}

void Functions::getLawChaosValue(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32) creature->getLawChaos() : -1;
}

void Functions::getGoodEvilValue(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32) creature->getGoodEvil() : -1;
}

void Functions::getAlignmentLawChaos(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32) NWN::getAlignmentLawChaos(creature->getLawChaos()) : -1;
}

void Functions::getAlignmentGoodEvil(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32) NWN::getAlignmentGoodEvil(creature->getGoodEvil()) : -1;
}

void Functions::getCommandable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (creature ? creature->isCommandable() : false);
}

void Functions::setCommandable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));

	if (creature)
		creature->setCommandable(ctx.getParams()[0].getInt() != 0);
}

void Functions::getMaster(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (Aurora::NWScript::Object *) (creature ? creature->getMaster() : 0);
}

void Functions::getAssociate(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (!creature)
		return;

	const int type = ctx.getParams()[0].getInt();
	const int nth  = ctx.getParams()[2].getInt();

	ctx.getReturn() = creature->getAssociate((AssociateType) type, nth);
}

void Functions::getHenchman(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Creature *creature = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!creature)
		return;

	ctx.getReturn() = creature->getAssociate(kAssociateTypeHenchman, ctx.getParams()[1].getInt());
}

void Functions::addHenchman(Aurora::NWScript::FunctionContext &ctx) {
	Creature *master   = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Creature *henchman = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!master || !henchman)
		return;

	master->addAssociate(*henchman, kAssociateTypeHenchman);
}

void Functions::removeHenchman(Aurora::NWScript::FunctionContext &ctx) {
	Creature *master   = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Creature *henchman = NWN::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!master || !henchman)
		return;

	master->removeAssociate(*henchman);
}

} // End of namespace NWN

} // End of namespace Engines

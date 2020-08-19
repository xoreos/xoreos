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
 *  Neverwinter Nights 2 engine functions messing with creatures.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/game.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/object.h"
#include "src/engines/nwn2/creature.h"
#include "src/engines/nwn2/item.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::getAbilityScore(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: nBaseAbilityScore

	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Ability   ability  = (Ability) ctx.getParams()[1].getInt();

	ctx.getReturn() = creature ? (int32_t) creature->getAbility(ability) : 0;
}

void Functions::getSkillRank(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->getSkillRank(ctx.getParams()[0].getInt()) : -1;
}

void Functions::getHasFeat(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->hasFeat(ctx.getParams()[0].getInt()) : 0;
}

void Functions::getClassByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kClassInvalid;

	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (!creature)
		return;

	uint32_t classID;
	uint16_t level;
	creature->getClass(MAX<int32_t>(ctx.getParams()[0].getInt() - 1, 0), classID, level);

	ctx.getReturn() = (int32_t) classID;
}

void Functions::getLevelByPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = 0;

	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (!creature)
		return;

	uint32_t classID;
	uint16_t level;
	creature->getClass(MAX<int32_t>(ctx.getParams()[0].getInt() - 1, 0), classID, level);
}

void Functions::getLevelByClass(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));

	ctx.getReturn() = creature ? creature->getClassLevel(ctx.getParams()[0].getInt()) : 0;
}

void Functions::getXP(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32_t) (creature ? creature->getXP() : 0);
}

void Functions::getIsDead(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->getIsDead();
}

void Functions::getImmortal(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->getImmortal();
}

void Functions::setImmortal(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (creature)
		creature->setImmortal(ctx.getParams()[1].getInt() != 0);
}

void Functions::getPlotFlag(Aurora::NWScript::FunctionContext &ctx) {
	NWN2::Object *object = NWN2::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	ObjectType type = object->getType();
	if (type == kObjectTypeCreature) {
		Creature *creature = NWN2::ObjectContainer::toCreature(object);
		ctx.getReturn() = creature && creature->getPlotFlag();
	} else if (type == kObjectTypeItem) {
		Item *item = NWN2::ObjectContainer::toItem(object);
		ctx.getReturn() = item && item->getPlotFlag();
	}
}

void Functions::setPlotFlag(Aurora::NWScript::FunctionContext &ctx) {
	NWN2::Object *object = NWN2::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	ObjectType type = object->getType();
	if (type == kObjectTypeCreature) {
		Creature *creature = NWN2::ObjectContainer::toCreature(object);
		if (creature)
			creature->setPlotFlag(ctx.getParams()[1].getInt() != 0);
	} else if (type == kObjectTypeItem) {
		Item *item = NWN2::ObjectContainer::toItem(object);
		if (item)
			item->setPlotFlag(ctx.getParams()[1].getInt() != 0);
	}
}

void Functions::getLootable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->getLootable();
}

void Functions::setLootable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (creature)
		creature->setLootable(ctx.getParams()[1].getInt() != 0);
}

void Functions::getIsCreatureDisarmable(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->getIsCreatureDisarmable();
}

void Functions::addJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 2));
	if (creature) {
		/*
		 * TODO:
		 *  - process bAllPartyMembers and bAllPlayers parameters
		 *  - set a local int variable on the creature(s):
		 *      name  = "NW_JOURNAL_ENTRY" + sPlotID
		 *      value = nState
		 */
		const Common::UString &plotID = ctx.getParams()[0].getString();
		const uint32_t state = ctx.getParams()[1].getInt();
		bool override = (ctx.getParams()[5].getInt() != 0);
		creature->addJournalQuestEntry(plotID, state, override);
	}
}

void Functions::removeJournalQuestEntry(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (creature) {
		/*
		 * TODO:
		 *  - process bAllPartyMembers and bAllPlayers parameters
		 *  - delete a local int variable from the creature(s):
		 *      name  = "NW_JOURNAL_ENTRY" + sPlotID
		 */
		const Common::UString &plotID = ctx.getParams()[0].getString();
		creature->removeJournalQuestEntry(plotID);
	}
}

void Functions::getJournalEntry(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 1));
	if (creature) {
		const Common::UString &plotID = ctx.getParams()[0].getString();
		ctx.getReturn() = static_cast<int32_t>(creature->getJournalEntry(plotID));
	}
}

void Functions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = NWN2::ObjectContainer::toPC(getParamObject(ctx, 0)) != 0;
}

void Functions::getIsDM(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature && creature->isDM();
}

void Functions::getGender(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32_t) (creature ? creature->getGender() : kGenderNone);
}

void Functions::getRacialType(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = (int32_t) (creature ? creature->getRace() : kRaceInvalid);
}

void Functions::getHitDice(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? creature->getHitDice() : 0;
}

void Functions::getLawChaosValue(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32_t) creature->getLawChaos() : -1;
}

void Functions::getGoodEvilValue(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32_t) creature->getGoodEvil() : -1;
}

void Functions::getAlignmentLawChaos(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32_t) NWN2::getAlignmentLawChaos(creature->getLawChaos()) : -1;
}

void Functions::getAlignmentGoodEvil(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));

	ctx.getReturn() = creature ? (int32_t) NWN2::getAlignmentGoodEvil(creature->getGoodEvil()) : -1;
}

void Functions::getIsRosterMember(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Creature *pc       = _game->getModule().getPC();

	ctx.getReturn() = (int32_t) ((creature && (creature == pc)) ? 1 : 0);
}

void Functions::getFactionLeader(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	Creature *pc       = _game->getModule().getPC();

	ctx.getReturn() = (Aurora::NWScript::Object *) ((creature == pc) ? creature : (Creature *) 0);
}

void Functions::featAdd(Aurora::NWScript::FunctionContext &ctx) {
	Creature *creature = NWN2::ObjectContainer::toCreature(getParamObject(ctx, 0));
	if (!creature)
		return;

	// Pass 'FeatID' and 'bCheckRequirements' parameters to creature's featAdd() call
	bool result = creature->featAdd(ctx.getParams()[1].getInt(), ctx.getParams()[2].getInt());
	// TODO: process remaining params: 'bFeedback' and 'bNotice'
	ctx.getReturn() = (int32_t) result;
}

} // End of namespace NWN2

} // End of namespace Engines

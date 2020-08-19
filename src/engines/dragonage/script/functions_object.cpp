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
 *  Dragon Age: Origins engine functions messing with objects.
 */

#include <memory>

#include <boost/make_shared.hpp>

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/dragonage/types.h"
#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/campaigns.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/objectcontainer.h"
#include "src/engines/dragonage/object.h"

#include "src/engines/dragonage/script/functions.h"

namespace Engines {

namespace DragonAge {

using Aurora::NWScript::kTypeInt;
using Aurora::NWScript::kTypeFloat;
using Aurora::NWScript::kTypeString;
using Aurora::NWScript::kTypeObject;
using Aurora::NWScript::kTypeEngineType;

void Functions::isObjectValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getParamObject(ctx, 0) != 0;
}

void Functions::getLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeInt).getInt();
}

void Functions::getLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeFloat).getFloat();
}

void Functions::getLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeString).getString();
}

void Functions::getLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeObject).getObject();
}

void Functions::getLocalResource(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeString).getString();
}

void Functions::getLocalLocation(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::getLocalPlayer(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::getLocalEvent(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::getLocalCommand(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::getLocalEffect(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::getLocalItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getVariable(ctx.getParams()[1].getString(), kTypeEngineType).getEngineType();
}

void Functions::setLocalInt(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getInt());
}

void Functions::setLocalFloat(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getFloat());
}

void Functions::setLocalString(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getString());
}

void Functions::setLocalObject(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getObject());
}

void Functions::setLocalResource(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getString());
}

void Functions::setLocalLocation(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::setLocalPlayer(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::setLocalEvent(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::setLocalCommand(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::setLocalEffect(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::setLocalItemProperty(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		object->setVariable(ctx.getParams()[1].getString(), ctx.getParams()[2].getEngineType());
}

void Functions::getObjectType(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (int32_t) kObjectTypeInvalid;

	DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object || ((uint32_t)object->getType() >= kObjectTypeMAX))
		return;

	ctx.getReturn() = (int32_t) object->getType();
}

void Functions::getTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString().clear();

	const Aurora::NWScript::Object *object = getParamObject(ctx, 0);
	if (object)
		ctx.getReturn() = object->getTag();
}

void Functions::getResRef(Aurora::NWScript::FunctionContext &ctx) {
	const DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	ctx.getReturn().getString() = object ? object->getResRef() : "";
}

void Functions::getName(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().getString() = "";

	const DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	ctx.getReturn().getString() = object->getNonLocalizedName();
	if (ctx.getReturn().getString().empty())
		ctx.getReturn().getString() = object->getName().getString();
}

void Functions::setName(Aurora::NWScript::FunctionContext &ctx) {
	DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	object->setNonLocalizedName(ctx.getParams()[1].getString());
}

void Functions::getArea(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	const DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	ctx.getReturn() = (Aurora::NWScript::Object *) campaign->getCurrentArea();
}

void Functions::getPosition(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn().setVector(0.0f, 0.0f, 0.0f);

	DragonAge::Object *object = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!object)
		return;

	float x, y, z;
	object->getPosition(x, y, z);

	ctx.getReturn().setVector(x, y, z);
}

#define SQR(x) ((x) * (x))
void Functions::getDistanceBetween(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = -1.0f;

	DragonAge::Object *object1 = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	DragonAge::Object *object2 = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 1));
	if (!object1 || !object2)
		return;

	float x1, y1, z1;
	object1->getPosition(x1, y1, z1);

	float x2, y2, z2;
	object2->getPosition(x2, y2, z2);

	ctx.getReturn() = sqrtf(SQR(x1 - x2) + SQR(y1 - y2) + SQR(z1 - z2));
}
#undef SQR

void Functions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	const Common::UString &tag = ctx.getParams()[0].getString();
	if (tag.empty())
		return;

	int nth = ctx.getParams()[1].getInt();

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(campaign->findObjectsByTag(tag));
	while (nth-- > 0)
		search->next();

	ctx.getReturn() = search->get();
}

void Functions::getNearestObject(Aurora::NWScript::FunctionContext &ctx) {
	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	DragonAge::Object *target = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!target)
		return;

	// Bitfield of type(s) to check for
	const uint32_t type = ctx.getParams()[1].getInt();
	// We want the nth nearest object
	size_t count = MAX<int32_t>(ctx.getParams()[2].getInt(), 0);

	if (count == 0)
		return;

	Aurora::NWScript::Variable::Array &result = ctx.getReturn().getArray();

	// TODO: nCheckLiving
	// TODO: nCheckPerceived

	const bool includeSelf = ctx.getParams()[5].getInt() != 0;
	if (includeSelf) {
		result.push_back(boost::make_shared<Aurora::NWScript::Variable>(target));
		count--;
	}

	if (count == 0)
		return;

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(campaign->findObjects());
	Aurora::NWScript::Object *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		// Needs to be a valid object and not the target
		DragonAge::Object *daObject = DragonAge::ObjectContainer::toObject(object);
		if (!daObject || (daObject == target))
			continue;

		// Ignore invalid object types
		const uint32_t objectType = (uint32_t) daObject->getType();
		if (objectType >= kObjectTypeMAX)
			continue;

		if (type & objectType)
			objects.push_back(daObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	for (std::list<Object *>::iterator it = objects.begin(); it != objects.end() && count > 0; ++it, count--)
		result.push_back(boost::make_shared<Aurora::NWScript::Variable>(*it));
}

void Functions::getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	DragonAge::Object *target = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!target)
		return;

	const Common::UString &tag = ctx.getParams()[1].getString();
	if (tag.empty())
		return;

	// Bitfield of type(s) to check for
	const uint32_t type = ctx.getParams()[2].getInt();
	// We want the nth nearest object
	size_t count = MAX<int32_t>(ctx.getParams()[3].getInt(), 0);

	if (count == 0)
		return;

	Aurora::NWScript::Variable::Array &result = ctx.getReturn().getArray();

	// TODO: nCheckLiving
	// TODO: nCheckPerceived

	const bool includeSelf = ctx.getParams()[6].getInt() != 0;
	if (includeSelf) {
		result.push_back(boost::make_shared<Aurora::NWScript::Variable>(target));
		count--;
	}

	if (count == 0)
		return;

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(campaign->findObjectsByTag(tag));
	Aurora::NWScript::Object       *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		// Needs to be a valid object and not the target
		DragonAge::Object *daObject = DragonAge::ObjectContainer::toObject(object);
		if (!daObject || (daObject == target))
			continue;

		// Ignore invalid object types
		const uint32_t objectType = (uint32_t) daObject->getType();
		if (objectType >= kObjectTypeMAX)
			continue;

		if (type & objectType)
			objects.push_back(daObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	for (std::list<Object *>::iterator it = objects.begin(); it != objects.end() && count > 0; ++it, count--)
		result.push_back(boost::make_shared<Aurora::NWScript::Variable>(*it));
}

void Functions::UT_getNearestObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;

	Campaign *campaign = _game->getCampaigns().getCurrentCampaign();
	if (!campaign)
		return;

	DragonAge::Object *target = DragonAge::ObjectContainer::toObject(getParamObject(ctx, 0));
	if (!target)
		return;

	const Common::UString &tag = ctx.getParams()[1].getString();
	if (tag.empty())
		return;

	const bool includeSelf = ctx.getParams()[2].getInt() != 0;
	if (includeSelf && (target->getTag() == tag)) {
		ctx.getReturn() = (Aurora::NWScript::Object *) target;
		return;
	}

	std::unique_ptr<Aurora::NWScript::ObjectSearch> search(campaign->findObjectsByTag(tag));
	Aurora::NWScript::Object *object = 0;

	std::list<Object *> objects;
	while ((object = search->next())) {
		// Needs to be a valid object and not the target
		DragonAge::Object *daObject = DragonAge::ObjectContainer::toObject(object);
		if (!daObject || (daObject == target))
			continue;

		objects.push_back(daObject);
	}

	objects.sort(ObjectDistanceSort(*target));

	if (!objects.empty())
		ctx.getReturn() = (Aurora::NWScript::Object *) *objects.begin();
}

} // End of namespace DragonAge

} // End of namespace Engines

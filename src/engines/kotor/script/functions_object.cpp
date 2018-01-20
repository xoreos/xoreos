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
 *  Star Wars: Knights of the Old Republic engine functions messing with objects.
 */

// TODO: check what happens on using invalid objects.

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor/types.h"
#include "src/engines/kotor/game.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/objectcontainer.h"
#include "src/engines/kotor/object.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

void Functions::getClickingObject(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getEnteringObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: This should return the *last* entered object, i.e. it should remember past triggerers.
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getExitingObject(Aurora::NWScript::FunctionContext &ctx) {
	// TODO: This should return the *last* exited object, i.e. it should remember past triggerers.
	ctx.getReturn() = ctx.getTriggerer();
}

void Functions::getIsObjectValid(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = getParamObject(ctx, 0) != 0;
}

void Functions::getIsPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = KotOR::ObjectContainer::toPC(getParamObject(ctx, 0)) != 0;
}

void Functions::getObjectByTag(Aurora::NWScript::FunctionContext &ctx) {
	Common::UString name = ctx.getParams()[0].getString();
	int nth = ctx.getParams()[1].getInt();

	Aurora::NWScript::ObjectSearch* search = _game->getModule().findObjectsByTag(name);
	for (int i = 0; i < nth; ++i) {
		search->next();
	}

	ctx.getReturn() = search->get();
}

void Functions::getMinOneHP(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	if (!kotorObject)
		throw Common::Exception("Functions::getMinOneHP(): invalid object");

	ctx.getReturn() = kotorObject->getMinOneHitPoints();
}

void Functions::setMinOneHP(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	bool enabled = ctx.getParams()[1].getInt();

	Object *kotorObject = ObjectContainer::toObject(object);

	if (!kotorObject)
		throw Common::Exception("Functions::setMinOneHP(): invalid object");

	kotorObject->setMinOneHitPoints(enabled);
}

void Functions::getCurrentHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	ctx.getReturn() = kotorObject ? kotorObject->getCurrentHitPoints() : 0;
}

void Functions::getMaxHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();

	Object *kotorObject = ObjectContainer::toObject(object);

	ctx.getReturn() = kotorObject ? kotorObject->getMaxHitPoints() : 0;
}

void Functions::setMaxHitPoints(Aurora::NWScript::FunctionContext &ctx) {
	Aurora::NWScript::Object *object = ctx.getParams()[0].getObject();
	int maxHitPoints = ctx.getParams()[1].getInt();

	if (maxHitPoints == 0)
		maxHitPoints = 1;

	Object *kotorObject = ObjectContainer::toObject(object);
	if (!kotorObject)
		throw Common::Exception("Functions::setMaxHitPoints(): Invalid object");

	kotorObject->setCurrentHitPoints(maxHitPoints);
	kotorObject->setMaxHitPoints(maxHitPoints);
}

} // End of namespace KotOR

} // End of namespace Engines

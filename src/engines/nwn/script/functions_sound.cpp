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
 *  Neverwinter Nights engine functions handling sound.
 */

#include "src/aurora/ssffile.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/nwn/objectcontainer.h"
#include "src/engines/nwn/object.h"
#include "src/engines/nwn/area.h"

#include "src/engines/nwn/script/functions.h"

namespace Engines {

namespace NWN {

void Functions::playVoiceChat(Aurora::NWScript::FunctionContext &ctx) {
	NWN::Object *object = NWN::ObjectContainer::toObject(getParamObject(ctx, 1));
	if (!object)
		return;

	const Aurora::SSFFile *ssf = object->getSSF();
	if (!ssf)
		return;

	object->playSound(ssf->getSound(ctx.getParams()[0].getInt()).fileName, true);
}

void Functions::musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->playAmbientMusic();
}

void Functions::musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->stopAmbientMusic();
}

void Functions::musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicDayTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicNightTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (area ? area->getMusicDayTrack() : -1);
}

void Functions::musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = (int32) (area ? area->getMusicNightTrack() : -1);
}

} // End of namespace NWN

} // End of namespace Engines

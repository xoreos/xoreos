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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords engine functions handling sound.
 */

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor2/objectcontainer.h"
#include "src/engines/kotor2/object.h"
#include "src/engines/kotor2/area.h"

#include "src/engines/kotor2/script/functions.h"

namespace Engines {

namespace KotOR2 {

void Functions::musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->playAmbientMusic();
}

void Functions::musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->stopAmbientMusic();
}

void Functions::musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicDayTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicNightTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32)area->getMusicDayTrack() : -1;
}

void Functions::musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = KotOR2::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32)area->getMusicNightTrack() : -1;
}

} // End of namespace KotOR2

} // End of namespace Engines

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
 *  The Witcher engine functions handling sound.
 */

#include "src/aurora/talkman.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/aurora/util.h"

#include "src/engines/witcher/objectcontainer.h"
#include "src/engines/witcher/object.h"
#include "src/engines/witcher/area.h"

#include "src/engines/witcher/nwscript/functions.h"

namespace Engines {

namespace Witcher {

void Functions::playSoundByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());

	const Common::UString sound = TalkMan.getSoundResRef((uint32_t) ctx.getParams()[0].getInt());

	// TODO: Run as action
	// bool runAsAction = ctx.getParams()[1].getInt() != 0;

	if (object)
		object->playSound(sound);
	else
		::Engines::playSound(sound, Sound::kSoundTypeVoice);
}

void Functions::playSound(Aurora::NWScript::FunctionContext &ctx) {
	Witcher::Object *object = Witcher::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->playSound(ctx.getParams()[0].getString());
	else
		::Engines::playSound(ctx.getParams()[0].getString(), Sound::kSoundTypeSFX);

	unimplementedFunction(ctx);
}

void Functions::musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->playAmbientMusic();
}

void Functions::musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->stopAmbientMusic();
}

void Functions::musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicDayTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicNightTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32_t)area->getMusicDayTrack() : -1;
}

void Functions::musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = Witcher::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32_t)area->getMusicNightTrack() : -1;
}

} // End of namespace Witcher

} // End of namespace Engines

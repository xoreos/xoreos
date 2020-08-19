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
 *  Neverwinter Nights 2 engine functions handling sound.
 */

#include "src/aurora/talkman.h"
#include "src/aurora/ssffile.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/objectcontainer.h"
#include "src/engines/nwn2/object.h"
#include "src/engines/nwn2/area.h"

#include "src/engines/nwn2/script/functions.h"

namespace Engines {

namespace NWN2 {

void Functions::playVoiceChat(Aurora::NWScript::FunctionContext &ctx) {
	NWN2::Object *object = NWN2::ObjectContainer::toObject(getParamObject(ctx, 1));
	if (!object)
		return;

	const Aurora::SSFFile *ssf = object->getSSF();
	if (!ssf)
		return;

	object->playSound(ssf->getSoundFile(ctx.getParams()[0].getInt()), true);
}

void Functions::playSoundByStrRef(Aurora::NWScript::FunctionContext &ctx) {
	NWN2::Object *object = NWN2::ObjectContainer::toObject(ctx.getCaller());

	const Common::UString sound = TalkMan.getSoundResRef((uint32_t) ctx.getParams()[0].getInt());

	// TODO: Run as action
	// bool runAsAction = ctx.getParams()[1].getInt() != 0;

	if (object)
		object->playSound(sound);
	else
		::Engines::playSound(sound, Sound::kSoundTypeVoice);
}

void Functions::playSound(Aurora::NWScript::FunctionContext &ctx) {
	NWN2::Object *object = NWN2::ObjectContainer::toObject(ctx.getCaller());

	if (object)
		object->playSound(ctx.getParams()[0].getString());
	else
		::Engines::playSound(ctx.getParams()[0].getString(), Sound::kSoundTypeSFX);

	unimplementedFunction(ctx);
}

void Functions::musicBackgroundPlay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->playAmbientMusic();
}

void Functions::musicBackgroundStop(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->stopAmbientMusic();
}

void Functions::musicBackgroundChangeDay(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicDayTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundChangeNight(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));
	if (area)
		area->setMusicNightTrack(ctx.getParams()[1].getInt());
}

void Functions::musicBackgroundGetDayTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32_t)area->getMusicDayTrack() : -1;
}

void Functions::musicBackgroundGetNightTrack(Aurora::NWScript::FunctionContext &ctx) {
	Area *area = NWN2::ObjectContainer::toArea(getParamObject(ctx, 0));

	ctx.getReturn() = area ? (int32_t)area->getMusicNightTrack() : -1;
}

} // End of namespace NWN2

} // End of namespace Engines

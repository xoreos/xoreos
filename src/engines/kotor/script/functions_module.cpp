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
 *  Star Wars: Knights of the Old Republic engine functions operating on the current module.
 */

#include "src/common/util.h"

#include "src/aurora/nwscript/functioncontext.h"

#include "src/engines/kotor/game.h"
#include "src/engines/kotor/module.h"
#include "src/engines/kotor/objectcontainer.h"

#include "src/engines/kotor/script/functions.h"

namespace Engines {

namespace KotOR {

void Functions::getModule(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) &_game->getModule();
}

void Functions::getFirstPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) _game->getModule().getPC();
}

void Functions::getNextPC(Aurora::NWScript::FunctionContext &ctx) {
	ctx.getReturn() = (Aurora::NWScript::Object *) 0;
}

void Functions::setGlobalFadeOut(Aurora::NWScript::FunctionContext &ctx) {
	float wait = ctx.getParams()[0].getFloat();
	float run = ctx.getParams()[1].getFloat();
	float r = ctx.getParams()[2].getFloat();
	float g = ctx.getParams()[3].getFloat();
	float b = ctx.getParams()[4].getFloat();

	Graphics::Aurora::FadeQuad &fadeQuad = _game->getModule().getFadeQuad();

	fadeQuad.setColor(r, g, b);
	fadeQuad.setWaitTime(wait);
	fadeQuad.setRunTime(run);
	fadeQuad.fadeOut();
}

void Functions::setGlobalFadeIn(Aurora::NWScript::FunctionContext &ctx) {
	float wait = ctx.getParams()[0].getFloat();
	float run = ctx.getParams()[1].getFloat();
	float r = ctx.getParams()[2].getFloat();
	float g = ctx.getParams()[3].getFloat();
	float b = ctx.getParams()[4].getFloat();

	Graphics::Aurora::FadeQuad &fadeQuad = _game->getModule().getFadeQuad();

	fadeQuad.setColor(r, g, b);
	fadeQuad.setWaitTime(wait);
	fadeQuad.setRunTime(run);
	fadeQuad.fadeIn();
}

void Functions::setReturnStrref(Aurora::NWScript::FunctionContext &ctx) {
	bool show = (ctx.getParams()[0].getInt() != 0);
	int returnStrref = ctx.getParams()[1].getInt();
	int returnQueryStrref = ctx.getParams()[2].getInt();

	if (show) {
		_game->getModule().setReturnStrref(returnStrref);
		_game->getModule().setReturnQueryStrref(returnQueryStrref);
	} else {
		// if the travel system is deactivated change the string to "Travelsystem deactivated"
		_game->getModule().setReturnStrref(38550);
	}
	_game->getModule().setReturnEnabled(show);
}

} // End of namespace KotOR

} // End of namespace Engines

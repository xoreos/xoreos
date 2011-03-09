/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/close.cpp
 *  A NWN close button widget.
 */

#include "common/ustring.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn/gui/widgets/close.h"

namespace Engines {

namespace NWN {

WidgetClose::WidgetClose(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : ModelWidget(gui, tag, model) {

}

WidgetClose::~WidgetClose() {
}

void WidgetClose::leave() {
	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetClose::mouseDown(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_model->setState("down");
	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetClose::mouseUp(uint8 state, float x, float y) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}

} // End of namespace NWN

} // End of namespace Engines

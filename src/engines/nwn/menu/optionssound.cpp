/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/optionssound.cpp
 *  The sound options menu.
 */

#include "engines/nwn/menu/optionssound.h"
#include "engines/nwn/menu/optionssoundadv.h"

#include "engines/aurora/util.h"

namespace Engines {

namespace NWN {

OptionsSoundMenu::OptionsSoundMenu() {
	load("options_sound");

	std::list<Widget *> speakerGroup;
	speakerGroup.push_back(getWidget("71Speakers"));
	speakerGroup.push_back(getWidget("51Speakers"));
	speakerGroup.push_back(getWidget("2Speakers"));
	speakerGroup.push_back(getWidget("4Speakers"));
	speakerGroup.push_back(getWidget("Surround"));
	speakerGroup.push_back(getWidget("Headphones"));
	declareGroup(speakerGroup);

	// TODO: Sound volumes
	getWidget("MusicSlider"  , true)->setDisabled(true);
	getWidget("VoicesSlider" , true)->setDisabled(true);
	getWidget("SoundFXSlider", true)->setDisabled(true);

	// TODO: Sound settings
	getWidget("EAXCheckbox", true)->setDisabled(true);
	getWidget("HardwareBox", true)->setDisabled(true);
	getWidget("71Speakers" , true)->setDisabled(true);
	getWidget("51Speakers" , true)->setDisabled(true);
	getWidget("2Speakers"  , true)->setDisabled(true);
	getWidget("4Speakers"  , true)->setDisabled(true);
	getWidget("Surround"   , true)->setDisabled(true);
	getWidget("Headphones" , true)->setDisabled(true);

	_advanced = new OptionsSoundAdvancedMenu;
}

OptionsSoundMenu::~OptionsSoundMenu() {
	delete _advanced;
}

void OptionsSoundMenu::callbackActive(Widget &widget) {
	if ((widget.getTag() == "CancelButton") ||
	    (widget.getTag() == "XButton")) {
		_returnCode = 1;
		// TODO: Scrap changes
		return;
	}

	if (widget.getTag() == "OkButton") {
		_returnCode = 2;
		// TODO: Adopt changes
		return;
	}

	if (widget.getTag() == "AdvSoundBtn") {
		sub(*_advanced);
		return;
	}
}

} // End of namespace NWN

} // End of namespace Engines

/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/kotor/gui/main/options.cpp
 *  The options menu.
 */

#include "engines/aurora/widget.h"

#include "engines/kotor/gui/main/options.h"
#include "engines/kotor/gui/options/gameplay.h"
#include "engines/kotor/gui/options/feedback.h"
#include "engines/kotor/gui/options/autopause.h"
#include "engines/kotor/gui/options/graphics.h"
#include "engines/kotor/gui/options/sound.h"

#include "gui/widgets/button.h"
#include "gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

OptionsMenu::OptionsMenu() {
	load("optionsmain");

	_gameplay = new OptionsGameplayMenu();
	_feedback = new OptionsFeedbackMenu();
	_autopause = new OptionsAutoPauseMenu();
	_graphics = new OptionsGraphicsMenu();
	_sound = new OptionsSoundMenu();

	configureText();

}

OptionsMenu::~OptionsMenu() {
	delete _gameplay;
	delete _feedback;
	delete _autopause;
	delete _graphics;
	delete _sound;
}

void OptionsMenu::configureText() {
	setDefaultHighlighting(getButton("BTN_GAMEPLAY")->getTextHighlightableComponent());
	setDefaultHighlighting(getButton("BTN_FEEDBACK")->getTextHighlightableComponent());
	setDefaultHighlighting(getButton("BTN_AUTOPAUSE")->getTextHighlightableComponent());
	setDefaultHighlighting(getButton("BTN_GRAPHICS")->getTextHighlightableComponent());
	setDefaultHighlighting(getButton("BTN_SOUND")->getTextHighlightableComponent());
	setDefaultHighlighting(getButton("BTN_BACK")->getTextHighlightableComponent());
}

void OptionsMenu::setDefaultHighlighting(Graphics::Aurora::Highlightable* highlightable) {
	highlightable->setHighlightable(true);
	highlightable->setHighlightDelta(0, 0, 0, .05);
	highlightable->setHighlightLowerBound(1, 1, 0, .2);
	highlightable->setHighlightUpperBound(1, 1, 0, 1);
}

void OptionsMenu::callbackActive(Widget &widget) {
	if (widget.getTag() == "BTN_GAMEPLAY") {
		sub(*_gameplay);
		return;
	}

	if (widget.getTag() == "BTN_FEEDBACK") {
		sub(*_feedback);
		return;
	}

	if (widget.getTag() == "BTN_AUTOPAUSE") {
		sub(*_autopause);
		return;
	}

	if (widget.getTag() == "BTN_GRAPHICS") {
		sub(*_graphics);
		return;
	}

	if (widget.getTag() == "BTN_SOUND") {
		sub(*_sound);
		return;
	}

	if (widget.getTag() == "BTN_BACK") {
		adoptChanges();
		_returnCode = 1;
		return;
	}
}

void OptionsMenu::adoptChanges() {
	dynamic_cast<OptionsGameplayMenu*>(_gameplay)->adoptChanges();
}


} // End of namespace KotOR

} // End of namespace Engines

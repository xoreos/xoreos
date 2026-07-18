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
 *  The race chooser in NWN CharGen.
 */
#include "src/aurora/talkman.h"

#include "src/common/util.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/creature.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/buttonsgroup.h"
#include "src/engines/nwn/gui/widgets/editbox.h"

#include "src/engines/nwn/gui/chargen/chargenchoices.h"
#include "src/engines/nwn/gui/chargen/charrace.h"

namespace Engines {

namespace NWN {

CharRace::CharRace(CharGenChoices &choices, ::Engines::Console *console) : CharGenBase(console) {
	_choices = &choices;

	try {
		load("cg_race");
	} catch (Common::Exception &e) {
		warning("CharRace::load(\"cg_race\") failed: %s", e.what());
		return;
	}

	// TODO Implement subrace.
	// NWN:EE renames several of these widgets. The lenient loader will
	// log a warning + return null instead of throwing, so we use plain
	// warning() (no abort) to keep the engine moving under EE while
	// still flagging the unexpected shape to the operator.
	Widget *subRace = getWidget("SubRaceButton", true);
	if (!subRace)
		warning("CharRace: SubRaceButton missing (variant compat) at %s:%d", __FILE__, __LINE__);
	if (subRace) subRace->setDisabled(true);

	// Init buttons and helpbox.
	WidgetEditBox *helpBox = getEditBox("HelpBox", true);
	if (!helpBox) {
		warning("CharRace: HelpBox missing at %s:%d; UI will be inert", __FILE__, __LINE__);
		reset();
		return;
	}
	_buttons = std::make_unique<ButtonsGroup>(helpBox);

	// EE renames many of these buttons. Each lookup is soft-missed in
	// lenient mode; we filter to the ones that actually resolve, then
	// continue. Original NWN continues to require every button (strict).
	struct { const char *tag; uint titleID; uint textID; } raceBtns[] = {
		{ "DwarfButton",     1985, 251 },
		{ "ElfButton",       1986, 252 },
		{ "GnomeButton",     1987, 253 },
		{ "HalflingButton",  1988, 254 },
		{ "HalfElfButton",   1989, 255 },
		{ "HalfOrcButton",   1990, 256 },
		{ "HumanButton",     1991, 257 },
	};
	for (auto &rb : raceBtns) {
		WidgetButton *btn = getButton(rb.tag, true);
		if (!btn) {
			warning("CharRace: %s missing (variant compat) at %s:%d",
			        rb.tag, __FILE__, __LINE__);
			continue;
		}
		_buttons->addButton(btn, TalkMan.getString(rb.titleID), TalkMan.getString(rb.textID));
	}

	reset();
}

CharRace::~CharRace() {
}

void CharRace::reset() {
	_buttons->setActive(getButton("HumanButton", true));

	getEditBox("HelpBox", true)->setTitle("fnt_galahad14", TalkMan.getString(481));
	getEditBox("HelpBox", true)->setText("fnt_galahad14", TalkMan.getString(485), 1.0f);

	// Set human as default race.
	_choices->setRace(6);
}

void CharRace::hide() {
	Engines::GUI::hide();

	if (_returnCode == 1) {
		// Set previous choice if any.
		if (_choices->getCharacter().getRace() < kRaceInvalid)
			_buttons->setActive(_choices->getCharacter().getRace());
	}
}

void CharRace::callbackActive(Widget &widget) {
	if (widget.getTag() == "OkButton") {
		_choices->setRace(_buttons->getChoice());
		_returnCode = 2;
		return;
	}

	if (widget.getTag() == "CancelButton") {
		_returnCode = 1;
		return;
	}

	if (widget.getTag() == "RecommendButton") {
		_buttons->setActive(6);
	}

	_buttons->setActive(dynamic_cast<WidgetButton *>(&widget));
}

} // End of namespace NWN

} // End of namespace Engines

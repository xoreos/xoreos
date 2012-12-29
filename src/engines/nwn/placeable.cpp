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

/** @file engines/nwn/placeable.cpp
 *  NWN placeable.
 */

#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn/placeable.h"

#include "engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

Placeable::Placeable(const Aurora::GFFStruct &placeable) : Situated(kObjectTypePlaceable),
	_state(kStateDefault), _tooltip(0) {

	load(placeable);
}

Placeable::~Placeable() {
	delete _tooltip;
}

void Placeable::load(const Aurora::GFFStruct &placeable) {
	Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFFFile *utp = 0;
	if (!temp.empty()) {
		try {
			utp = new Aurora::GFFFile(temp, Aurora::kFileTypeUTP, MKID_BE('UTP '));
		} catch (...) {
			delete utp;
		}
	}

	Situated::load(placeable, utp ? &utp->getTopLevel() : 0);

	delete utp;
}

void Placeable::setModelState() {
	if (!_model)
		return;

	switch (_state) {
		case kStateDefault:
			_model->setState("default");
			break;

		case kStateOpen:
			_model->setState("open");
			break;

		case kStateClosed:
			_model->setState("close");
			break;

		case kStateDestroyed:
			_model->setState("dead");
			break;

		case kStateActivated:
			_model->setState("on");
			break;

		case kStateDeactivated:
			_model->setState("off");
			break;

		default:
			_model->setState("");
			break;
	}

}

void Placeable::hide() {
	leave();

	hideTooltip();

	delete _tooltip;
	_tooltip = 0;

	Situated::hide();
}

void Placeable::loadObject(const Aurora::GFFStruct &gff) {
	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);
}

void Placeable::loadAppearance() {
	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeables");

	_modelName    = twoda.getRow(_appearanceID).getString("ModelName");
	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
}

void Placeable::enter() {
	highlight(true);
}

void Placeable::leave() {
	highlight(false);
}

void Placeable::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);

	if (enabled)
		showTooltip();
	else
		hideTooltip();
}

void Placeable::createTooltip() {
	if (_tooltip)
		return;

	_tooltip = new Tooltip(Tooltip::kTypeFeedback, *_model);

	_tooltip->setAlign(0.5);
	_tooltip->addLine(_name, 0.5, 0.5, 1.0, 1.0);
	_tooltip->setPortrait(_portrait);
}

void Placeable::showTooltip() {
	createTooltip();
	_tooltip->show();
}

void Placeable::hideTooltip() {
	if (!_tooltip)
		return;

	_tooltip->hide();
}

bool Placeable::isOpen() const {
	return _state == kStateOpen;
}

bool Placeable::click(Object *triggerer) {
	// If the door has a used script, call that
	if (hasScript(kScriptUsed))
		return runScript(kScriptUsed, this, triggerer);

	return true;
}

void Placeable::playAnimation(Animation animation) {
	// TODO: Door::Placeable(): Animate

	switch (animation) {
		case kAnimationPlaceableActivate:
			playSound(_soundUsed);
			_state = kStateActivated;
			break;

		case kAnimationPlaceableDeactivate:
			playSound(_soundUsed);
			_state = kStateDeactivated;
			break;

		case kAnimationPlaceableOpen:
			playSound(_soundOpened);
			_state = kStateOpen;
			break;

		case kAnimationPlaceableClose:
			playSound(_soundClosed);
			_state = kStateClosed;
			break;

		default:
			break;
	}

	setModelState();
}
} // End of namespace NWN

} // End of namespace Engines

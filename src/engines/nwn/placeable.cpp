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
 *  NWN placeable.
 */

#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/placeable.h"

#include "src/engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Situated(kObjectTypePlaceable),
	_state(kStateDefault), _tooltip(0) {

	load(placeable);
}

Placeable::~Placeable() {
	delete _tooltip;
}

void Placeable::load(const Aurora::GFF3Struct &placeable) {
	Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFF3File *utp = 0;
	if (!temp.empty()) {
		try {
			utp = new Aurora::GFF3File(temp, Aurora::kFileTypeUTP, MKTAG('U', 'T', 'P', ' '));
		} catch (...) {
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

void Placeable::loadObject(const Aurora::GFF3Struct &gff) {
	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);
}

void Placeable::loadAppearance() {
	const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("placeables");

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

	_tooltip->setAlign(0.5f);
	_tooltip->addLine(_name, 0.5f, 0.5f, 1.0f, 1.0f);
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

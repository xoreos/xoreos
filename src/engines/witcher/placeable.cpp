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
 *  A placeable object in a The Witcher area.
 */

#include "src/common/util.h"

#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/witcher/placeable.h"

namespace Engines {

namespace Witcher {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Situated(kObjectTypePlaceable),
	_state(kStateDefault) {

	load(placeable);
}

Placeable::~Placeable() {
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

void Placeable::show() {
	setModelState();

	Situated::show();
}

void Placeable::hide() {
	leave();

	Situated::hide();
}

void Placeable::loadObject(const Aurora::GFF3Struct &gff) {
	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);
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
}

bool Placeable::isOpen() const {
	return _state == kStateOpen;
}

bool Placeable::isActivated() const {
	return isOpen();
}

bool Placeable::click(Object *UNUSED(triggerer)) {
	return true;
}

} // End of namespace Witcher

} // End of namespace Engines

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

/** @file engines/nwn2/placeable.cpp
 *  NWN2 placeable.
 */

#include "common/util.h"

#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"

#include "engines/nwn2/placeable.h"

namespace Engines {

namespace NWN2 {

Placeable::Placeable(const Aurora::GFFStruct &placeable) : Situated(kObjectTypePlaceable),
	_state(kStateDefault) {

	load(placeable);
}

Placeable::~Placeable() {
}

void Placeable::load(const Aurora::GFFStruct &placeable) {
	Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFFFile *utp = 0;
	if (!temp.empty()) {
		try {
			utp = new Aurora::GFFFile(temp, Aurora::kFileTypeUTP, MKTAG('U', 'T', 'P', ' '));
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
	Situated::hide();
}

void Placeable::loadObject(const Aurora::GFFStruct &gff) {
	// State

	_state = (State) gff.getUint("AnimationState", (uint) _state);
}

void Placeable::loadAppearance() {
	const Aurora::TwoDAFile &twoda = TwoDAReg.get("placeables");

	if (_modelName.empty())
		_modelName = twoda.getRow(_appearanceID).getString("ModelName");
	if (_modelName == "RESERVED")
		_modelName.clear();

	if (_modelName.empty())
		_modelName = twoda.getRow(_appearanceID).getString("NWN2_ModelName");

	_soundAppType = twoda.getRow(_appearanceID).getInt("SoundAppType");
}

bool Placeable::isOpen() const {
	return _state == kStateOpen;
}

} // End of namespace NWN2

} // End of namespace Engines

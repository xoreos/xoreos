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
 *  A placeable in a Jade Empire area.
 */

#include "src/common/error.h"
#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/talkman.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/jade/placeable.h"

namespace Engines {

namespace Jade {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Object(kObjectTypePlaceable),
	_appearanceType(Aurora::kFieldIDInvalid), _model(0) {

	load(placeable);
}

Placeable::~Placeable() {
	delete _model;
	delete _fsm;
}

void Placeable::show() {
	if (_model)
		_model->show();
}

void Placeable::hide() {
	leave();
	if (_model)
		_model->hide();
}

void Placeable::load(const Aurora::GFF3Struct &placeable) {
	_resRef = placeable.getString("ResRef");

	if (!_resRef.empty()) {
		Aurora::GFF3File *pla = 0;
		try {
			pla = new Aurora::GFF3File(_resRef, Aurora::kFileTypePLA, MKTAG('P', 'L', 'A', ' '));
			loadBlueprint(pla->getTopLevel());
			loadProperties();
		} catch (...) {
			warning("Placeable \"%s\" has no blueprint", _tag.c_str());
			delete pla;
			throw;
		}
		delete pla;
	}

	loadInstance(placeable);
	loadAppearance();
}

void Placeable::loadBlueprint(const Aurora::GFF3Struct &gff) {
	// Appearance
	_appearanceType = gff.getUint("AppearanceType", _appearanceType);

	// Sound Cue
	_soundCue = gff.getString("SoundCue");

	// Scripts
	readScripts(gff);
}

void Placeable::loadProperties() {
	if (_appearanceType == Aurora::kFieldIDInvalid) {
		warning("placeable \"%s\" without an appearance", _tag.c_str());
		return;
	}

	const Aurora::TwoDARow &placeable = TwoDAReg.get2DA("placeables").getRow(_appearanceType);

	// Modelname
	_modelName = placeable.getString("modelname");

	uint32 strref = placeable.getInt("strref");
	_name = TalkMan.getString(strref);
}

void Placeable::loadInstance(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Sound Cue
	_soundCue = gff.getString("SoundCue");

	// Usable
	_usable = gff.getBool("Active");

	// Modelname
	Common::UString appearance = gff.getString("Appearance", _modelName);
	if (!appearance.empty())
		_modelName = appearance;

	const Aurora::GFF3Struct &positional = gff.getStruct("Positional");

	double x, y, z;

	// Position

	positional.getVector("Position", x, y, z);
	setPosition(x, y, z);

	// Orientation

	positional.getVector("Orientation", x, y, z);
	setOrientation(x, y, z, 0);

	// Name
	gff.getString("Name");

	// State
	_state = gff.getSint("State");

	Common::UString fsmName = gff.getString("FSM");
	_fsm = new Aurora::GFF3File(fsmName, Aurora::kFileTypeFSM, MKTAG('F', 'S', 'M', ' '));

}

void Placeable::loadAppearance() {

	// Model

	if (!_modelName.empty()) {
		_model = loadModelObject(_modelName);

		if (!_model)
			throw Common::Exception("Failed to load placeable model \"%s\"",
			                        _modelName.c_str());

		// Clickable
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		// Position and Orientation
		float x, y, z, angle;
		getPosition(x, y, z);
		_model->setPosition(x, y, z);
		getOrientation(x, y, z, angle);
		_model->setOrientation(x, y, z, angle);

		// ID
		_ids.push_back(_model->getID());
	} else
		warning("Placeable \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());

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

} // End of namespace Jade

} // End of namespace Engines

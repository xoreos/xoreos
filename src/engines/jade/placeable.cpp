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
#include "src/engines/jade/types.h"

namespace Engines {

namespace Jade {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Object(kObjectTypePlaceable),
	_appearanceType(Aurora::kFieldIDInvalid), _lastOpenedBy(0), _lastClosedBy(0) {

	load(placeable);
}

Placeable::~Placeable() {
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
		try {
			std::unique_ptr<Aurora::GFF3File>
				pla(new Aurora::GFF3File(_resRef, Aurora::kFileTypePLA, MKTAG('P', 'L', 'A', ' ')));

			loadBlueprint(pla->getTopLevel());
			loadProperties();

		} catch (Common::Exception &e) {
			e.add("Placeable \"%s\" has no blueprint", _tag.c_str());
			throw;
		}
	}

	loadInstance(placeable);
	loadAppearance();
}

void Placeable::loadBlueprint(const Aurora::GFF3Struct &gff) {
	//Conversation
	_conversation = gff.getString("Conversation", _conversation);

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

	uint32_t strref = placeable.getInt("strref");
	_name = TalkMan.getString(strref);
}

void Placeable::loadInstance(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Sound Cue
	_soundCue = gff.getString("SoundCue");

	// Active
	_active = gff.getBool("Active");

	// Modelname
	Common::UString appearance = gff.getString("Appearance", _modelName);
	if (!appearance.empty())
		_modelName = appearance;

	loadPositional(gff);

	// Name
	gff.getString("Name");

	// State
	_state = gff.getSint("State");

	Common::UString fsmName = gff.getString("FSM");
	_fsm = std::make_unique<Aurora::GFF3File>(fsmName, Aurora::kFileTypeFSM, MKTAG('F', 'S', 'M', ' '));
}

void Placeable::loadAppearance() {

	// Model

	if (!_modelName.empty()) {
		_model.reset(loadModelObject(_modelName));

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

bool Placeable::open(Object *opener) {
	int32_t newState = nextState("open");

	if (newState == -1) {
		runScript(kScriptOnFailToOpen, this, opener);
		return false;
	}

	_lastOpenedBy = opener;
	_state = newState;

	playAnimation(kAnimationOpeningADoorDoubleDoor);
	runScript(kScriptOnOpen, this, opener);

	return true;
}

bool Placeable::close(Object *closer) {
	int32_t newState = nextState("closed");

	if (newState != -1) {
		_lastClosedBy = closer;

		_state = newState;

		runScript(kScriptOnClose, this, closer);

		return true;
	}
	return false;
}

int32_t Placeable::nextState(const Common::UString &input) {
	const Aurora::GFF3Struct &top = _fsm->getTopLevel();

	const Aurora::GFF3Struct &trans = top.getStruct("StateTrans");

	int32_t inputCount = top.getSint("InputCount");

	const Aurora::GFF3Struct &inputs = top.getStruct("InputNames");
	for (int i = 0;i < inputCount; i++) {
		const Common::UString &inputKey = Common::UString::format("I%i", i);
		const Common::UString &inputValue = inputs.getString(inputKey);
		if (input == inputValue) {
			const Common::UString &transKey = Common::UString::format("S%i_I%i", _state, i);
			return trans.getSint(transKey);
		}
	}
	return -1;
}

} // End of namespace Jade

} // End of namespace Engines

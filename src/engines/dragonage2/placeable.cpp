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
 *  A placeable object in a Dragon Age II area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gdafile.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/dragonage2/placeable.h"
#include "src/engines/dragonage2/util.h"

namespace Engines {

namespace DragonAge2 {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Object(kObjectTypePlaceable),
	_appearanceID(0xFFFFFFFF) {

	load(placeable);
}

Placeable::~Placeable() {
	hide();
}

void Placeable::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Placeable::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

void Placeable::show() {
	if (_model)
		_model->show();
}

void Placeable::hide() {
	if (_model)
		_model->hide();
}

void Placeable::enter() {
	status("Placeable \"%s\" (\"%s\"): \"%s\"",
	       _tag.c_str(), _name.getString().c_str(), _description.getString().c_str());

	highlight(true);
}

void Placeable::leave() {
	highlight(false);
}

void Placeable::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

bool Placeable::click(Object *triggerer) {
	runScript(kEventTypeClick, this, triggerer);
	runScript(kEventTypeUse  , this, triggerer);

	return true;
}

void Placeable::load(const Aurora::GFF3Struct &placeable) {
	_resRef = placeable.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utp;
	if (!_resRef.empty())
		utp.reset(loadOptionalGFF3(_resRef, Aurora::kFileTypeUTP, MKTAG('U', 'T', 'P', ' ')));

	load(placeable, utp ? &utp->getTopLevel() : 0);
}

void Placeable::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint = 0) {
	if (blueprint)
		loadProperties(*blueprint);
	loadProperties(instance);

	const Aurora::GDAFile &gda = getMGDA(kWorksheetPlaceables);

	_model.reset(loadModelObject(gda.getString(gda.findRow(_appearanceID), "Model")));

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());
	}

	syncPosition();
	syncOrientation();
}

void Placeable::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name and description
	gff.getLocString("LocName"     , _name);
	gff.getLocString("LocPopupText", _description);

	// Conversation
	_conversation = gff.getString("Conversation", _conversation);

	// Static and usable
	_static = !gff.getBool("Active" , !_static);
	_usable =  gff.getBool("Useable",  _usable);

	// Appearance
	_appearanceID = gff.getUint("Appearance", _appearanceID);

	// Position
	if (gff.hasField("XPosition")) {
		const float position[3] = {
			(float) gff.getDouble("XPosition"),
			(float) gff.getDouble("YPosition"),
			(float) gff.getDouble("ZPosition")
		};

		setPosition(position[0], position[1], position[2]);
	}

	// Orientation
	if (gff.hasField("XOrientation")) {
		const float orientation[4] = {
			(float) gff.getDouble("XOrientation"),
			(float) gff.getDouble("YOrientation"),
			(float) gff.getDouble("ZOrientation"),
			(float) Common::rad2deg(acos(gff.getDouble("WOrientation")) * 2.0)
		};

		setOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
	}

	// Variables and script
	readVarTable(gff);
	readScript(gff);
	enableEvents(true);
}

} // End of namespace Dragon Age

} // End of namespace Engines

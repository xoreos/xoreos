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

#include "src/engines/aurora/model.h"

#include "src/engines/dragonage2/placeable.h"
#include "src/engines/dragonage2/util.h"

namespace Engines {

namespace DragonAge2 {

Placeable::Placeable(const Aurora::GFF3Struct &placeable) : Object(kObjectTypePlaceable),
	_appearance(0xFFFFFFFF), _model(0) {

	try {
		load(placeable);
	} catch (...) {
		delete _model;
		throw;
	}
}

Placeable::~Placeable() {
	hide();

	delete _model;
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

void Placeable::load(const Aurora::GFF3Struct &placeable) {
	const Common::UString temp = placeable.getString("TemplateResRef");

	Aurora::GFF3File *utp = 0;
	if (!temp.empty()) {
		try {
			utp = new Aurora::GFF3File(temp, Aurora::kFileTypeUTP, MKTAG('U', 'T', 'P', ' '));
		} catch (...) {
		}
	}

	try {
		load(placeable, utp ? &utp->getTopLevel() : 0);
	} catch (...) {
		delete utp;
		throw;
	}

	delete utp;
}

void Placeable::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint = 0) {
	if (blueprint)
		loadProperties(*blueprint);
	loadProperties(instance);

	const Aurora::GDAFile &gda = getMGDA(kWorksheetPlaceables);

	_model = loadModelObject(gda.getString(gda.findRow(_appearance), "Model"));

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());
	}

	syncPosition();
	syncOrientation();
}

void Placeable::loadProperties(const Aurora::GFF3Struct &gff) {
	_tag = gff.getString("Tag", _tag);

	if (gff.hasField("LocName"))
		gff.getLocString("LocName", _name);
	if (gff.hasField("LocPopupText"))
		gff.getLocString("LocPopupText", _description);

	_conversation = gff.getString("Conversation", _conversation);

	_static = !gff.getBool("Active" , !_static);
	_usable =  gff.getBool("Useable",  _usable);

	_appearance = gff.getUint("Appearance", _appearance);

	if (gff.hasField("VarTable"))
		readVarTable(gff.getList("VarTable"));

	if (gff.hasField("XPosition")) {
		const float position[3] = {
			(float) gff.getDouble("XPosition"),
			(float) gff.getDouble("YPosition"),
			(float) gff.getDouble("ZPosition")
		};

		setPosition(position[0], position[1], position[2]);
	}

	if (gff.hasField("XOrientation")) {
		const float orientation[4] = {
			(float) gff.getDouble("XOrientation"),
			(float) gff.getDouble("YOrientation"),
			(float) gff.getDouble("ZOrientation"),
			(float) Common::rad2deg(acos(gff.getDouble("WOrientation")) * 2.0)
		};

		setOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
	}
}

} // End of namespace Dragon Age

} // End of namespace Engines

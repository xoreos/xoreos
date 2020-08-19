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
 *  A placeable object in a Sonic Chronicles: The Dark Brotherhood area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/uuid.h"

#include "src/aurora/gff4file.h"
#include "src/aurora/gdafile.h"
#include "src/aurora/2dareg.h"

#include "src/aurora/nwscript/objectman.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"
#include "src/engines/aurora/resources.h"

#include "src/engines/sonic/placeable.h"
#include "src/engines/sonic/area.h"

/** Map a placeable type ID onto an appearance ID. */
static const uint32_t kTypeAppearances[] = {
	        84,         81,         82, 0xFFFFFFFF,         89, 0xFFFFFFFF,         92, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,        101,
	       136, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	       127, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,        130, 0xFFFFFFFF,        134,
	       135, 0xFFFFFFFF, 0xFFFFFFFF,
};

namespace Engines {

namespace Sonic {

Placeable::Placeable(const Aurora::GFF4Struct &placeable) : Object(kObjectTypePlaceable),
	_placeableID(0xFFFFFFFF), _typeID(0xFFFFFFFF), _appearanceID(0xFFFFFFFF), _scale(1.0f) {
	_id = Common::generateIDNumber();
	ObjectMan.registerObject(this);

	load(placeable);
}

Placeable::~Placeable() {
	ObjectMan.unregisterObject(this);

	hide();

	try {
		deindexResources(_modelTexture);
	} catch (...) {
	}
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
	warning("Placeable %u: \"%s\"", _id, _tag.c_str());

	highlight(true);
}

void Placeable::leave() {
	highlight(false);
}

void Placeable::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

void Placeable::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model) {
		Area::getWorldPosition(x, y, z, x, y, z);

		_model->setPosition(x, y, z);
	}
}

void Placeable::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

void Placeable::load(const Aurora::GFF4Struct &placeable) {
	_tag = placeable.getString(Aurora::kGFF4Tag);

	_placeableID = placeable.getUint(40023, 0xFFFFFFFF);
	_typeID      = placeable.getUint(40018, 0xFFFFFFFF);

	_appearanceID = 0xFFFFFFFF;
	if (_typeID < ARRAYSIZE(kTypeAppearances))
		_appearanceID = kTypeAppearances[_typeID];

	if (_appearanceID != 0xFFFFFFFF) {
		const Aurora::GDAFile &appearances = TwoDAReg.getGDA("appearances");

		if (appearances.hasRow(_appearanceID)) {
			_modelName = appearances.getString(_appearanceID, 2122127238);
			_scale     = appearances.getFloat (_appearanceID, "Scale", 1.0f);
		}
	}

	if (!_modelName.empty()) {
		/* TODO: This basically just protects against an exception for opening the
		 *       same archive twice when loading objects using the same model.
		 *       We should probably add some kind of reference counting somewhere...
		 */
		try {
			indexMandatoryArchive(_modelName + ".nsbtx", 100 + _id, &_modelTexture);
		} catch (...) {
		}

		_model.reset(loadModelObject(_modelName));

		if (_model) {
			_model->setScale(_scale, _scale, _scale);

			_model->setClickable(true);
			_modelID = _model->getID();
		}
	}

	if (placeable.hasField(Aurora::kGFF4Position)) {
		double x = 0.0, y = 0.0, z = 0.0;

		placeable.getVector3(Aurora::kGFF4Position, x, y, z);
		setPosition(x, y, z);
	}

	double orientation = placeable.getDouble(Aurora::kGFF4Orientation);
	setOrientation(0.0f, 1.0f, 0.0f, Common::rad2deg(orientation));
}

} // End of namespace Sonic

} // End of namespace Engines

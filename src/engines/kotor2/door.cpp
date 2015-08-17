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
 *  A door in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#include "src/common/util.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/kotor2/door.h"

namespace Engines {

namespace KotOR2 {

Door::Door(const Aurora::GFF3Struct &door) : Situated(kObjectTypeDoor),
	_genericType(Aurora::kFieldIDInvalid) {

	load(door);
}

Door::~Door() {
}

void Door::load(const Aurora::GFF3Struct &door) {
	Common::UString temp = door.getString("TemplateResRef");

	Aurora::GFF3File *utd = 0;
	if (!temp.empty()) {
		try {
			utd = new Aurora::GFF3File(temp, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' '));
		} catch (...) {
		}
	}

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	if (!utd)
		warning("Door \"%s\" has no blueprint", _tag.c_str());

	delete utd;
}

void Door::loadObject(const Aurora::GFF3Struct &gff) {
	_genericType = gff.getUint("GenericType", _genericType);
}

void Door::loadAppearance() {
	if (_appearanceID == 0) {
		if (_genericType == Aurora::kFieldIDInvalid)
			throw Common::Exception("Door \"%s\" has no appearance ID and no generic type",
			                        _tag.c_str());

		loadAppearance(TwoDAReg.get2DA("genericdoors"), _genericType);
	} else
		loadAppearance(TwoDAReg.get2DA("doortypes"), _appearanceID);
}

void Door::loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id) {
	uint32 column = twoda.headerToColumn("ModelName");
	if (column == Aurora::kFieldIDInvalid)
		column = twoda.headerToColumn("Model");

	_modelName = twoda.getRow(id).getString(column);
}

void Door::hide() {
	leave();

	Situated::hide();
}

void Door::enter() {
	highlight(true);
}

void Door::leave() {
	highlight(false);
}

void Door::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

} // End of namespace KotOR2

} // End of namespace Engines

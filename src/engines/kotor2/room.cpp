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
 *  A room within a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/kotor2/room.h"

namespace Engines {

namespace KotOR2 {

Room::Room(const Common::UString &resRef, float x, float y, float z) {
	load(resRef, x, y, z);
}

Room::~Room() {
}

void Room::load(const Common::UString &resRef, float x, float y, float z) {
	if (resRef == "****")
		return;

	_model.reset(loadModelObject(resRef));
	if (!_model)
		throw Common::Exception("Can't load room model \"%s\"", resRef.c_str());

	_model->setPosition(x, y, z);
}

void Room::show() {
	if (_model)
		_model->show();
}

void Room::hide() {
	if (_model)
		_model->hide();
}

} // End of namespace KotOR2

} // End of namespace Engines

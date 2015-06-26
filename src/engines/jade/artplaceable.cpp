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
 *  A simple model object within a Jade Empire area, guided by a state machine.
 */

#include "src/common/util.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/model.h"

#include "src/engines/jade/artplaceable.h"

namespace Engines {

namespace Jade {

ArtPlaceable::ArtPlaceable(const Common::UString &resRef, uint32 id, float x, float y, float z) :
	_model(0) {

	load(resRef, id, x, y, z);
}

ArtPlaceable::~ArtPlaceable() {
	unload();
}

void ArtPlaceable::load(const Common::UString &resRef, uint32 id, float x, float y, float z) {
	_tag  = Common::UString::format("%s#%u", resRef.c_str(), id);
	_name = resRef;

	_model = loadModelObject(resRef);
	if (!_model)
		warning("Failed to load art placeable object model \"%s\"", resRef.c_str());

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());
	}

	setPosition(x, y, z);
}

void ArtPlaceable::unload() {
	delete _model;
	_model = 0;
}

void ArtPlaceable::show() {
	if (_model)
		_model->show();
}

void ArtPlaceable::hide() {
	if (_model)
		_model->hide();
}

void ArtPlaceable::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void ArtPlaceable::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

void ArtPlaceable::enter() {
	highlight(true);
}

void ArtPlaceable::leave() {
	highlight(false);
}

void ArtPlaceable::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

} // End of namespace Jade

} // End of namespace Engines

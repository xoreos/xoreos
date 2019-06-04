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
 *  Stage implementation for actionscript.
 */

#include "src/aurora/actionscript/stage.h"

Aurora::ActionScript::Stage::Stage() : _width(0), _height(0) {
}

bool Aurora::ActionScript::Stage::hasMember(const Common::UString &id) const {
	if (id == "width" || id == "height")
		return true;

	return Object::hasMember(id);
}

Aurora::ActionScript::Variable Aurora::ActionScript::Stage::getMember(const Aurora::ActionScript::Variable &id) {
	if (id.asString() == "width")
		return _width;
	if (id.asString() == "height")
		return _height;

	return Object::getMember(id);
}

void Aurora::ActionScript::Stage::setSize(unsigned int width, unsigned int height) {
	_width = width;
	_height = height;
}

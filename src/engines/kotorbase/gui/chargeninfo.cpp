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
 *  Character generation information for KotOR games.
 */

#include "src/engines/kotorbase/gui/chargeninfo.h"

namespace Engines {

namespace KotORBase {

const Common::UString &CharacterGenerationInfo::getName() const {
	return _name;
}

Skin CharacterGenerationInfo::getSkin() const {
	return _skin;
}

uint8_t CharacterGenerationInfo::getFace() const {
	return _face;
}

Class CharacterGenerationInfo::getClass() const {
	return _class;
}

Gender CharacterGenerationInfo::getGender() const {
	return _gender;
}

const CreatureInfo::Abilities &CharacterGenerationInfo::getAbilities() const {
	return _abilities;
}

void CharacterGenerationInfo::setName(const Common::UString &name) {
	_name = name;
}

void CharacterGenerationInfo::setSkin(Skin skin) {
	_skin = skin;
}

void CharacterGenerationInfo::setFace(uint8_t face) {
	_face = face;
}

CharacterGenerationInfo::CharacterGenerationInfo(const CharacterGenerationInfo &info) {
	_class = info._class;
	_gender = info._gender;
	_skin = info._skin;
	_face = info._face;
	_name = info._name;
}

CharacterGenerationInfo::~CharacterGenerationInfo() {
}

void CharacterGenerationInfo::operator=(const CharacterGenerationInfo &info) {
	_class = info._class;
	_gender = info._gender;
	_skin = info._skin;
	_face = info._face;
	_name = info._name;
}

CharacterGenerationInfo::CharacterGenerationInfo() {
}

} // End of namespace KotORBase

} // End of namespace Engines

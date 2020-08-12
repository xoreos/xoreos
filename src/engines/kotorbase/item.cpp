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
 *  Inventory item in KotOR games.
 */

#include "src/common/string.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/kotorbase/item.h"

namespace Engines {

namespace KotORBase {

Item::Item(const Common::UString &item) : Object(kObjectTypeItem) {
	std::unique_ptr<Aurora::GFF3File> uti = std::make_unique<Aurora::GFF3File>(item, Aurora::kFileTypeUTI);

	load(uti->getTopLevel());
}

void Item::load(const Aurora::GFF3Struct &gff) {
	// Tag, name and description
	_tag = gff.getString("Tag");
	_name = gff.getString("LocalizedName");
	_description = gff.getString("Description");

	// Base item
	_baseItem = gff.getSint("BaseItem");
	const Aurora::TwoDARow &twoDA = TwoDAReg.get2DA("baseitems").getRow(_baseItem);
	_equipableSlotsMask = twoDA.getInt("equipableslots");
	_itemClass = twoDA.getString("itemclass");
	_weaponWield = static_cast<WeaponWield>(twoDA.getInt("weaponwield"));
	_rangedWeapon = twoDA.getInt("rangedweapon");
	_maxAttackRange = twoDA.getFloat("maxattackrange");
	_numDice = twoDA.getInt("numdice");
	_dieToRoll = twoDA.getInt("dietoroll");

	// Model, body and texture variations
	_modelVariation = gff.getSint("ModelVariation");
	_bodyVariation = gff.getSint("BodyVariation");
	_textureVariation = gff.getSint("TextureVar");
}

const Common::UString &Item::getName() const {
	return _name;
}

WeaponWield Item::getWeaponWield() const {
	return _weaponWield;
}

float Item::getMaxAttackRange() const {
	return _maxAttackRange;
}

int Item::getNumDice() const {
	return _numDice;
}

int Item::getDieToRoll() const {
	return _dieToRoll;
}

bool Item::isSlotEquipable(InventorySlot slot) const {
	if (slot > 31)
		return false;

	return (_equipableSlotsMask & (1U << slot)) != 0;
}

bool Item::isRangedWeapon() const {
	return _rangedWeapon;
}

int Item::getBodyVariation() const {
	return _bodyVariation;
}

int Item::getTextureVariation() const {
	return _textureVariation;
}

const Common::UString Item::getIcon() const {
	int variation = (_modelVariation == 0) ? _textureVariation : _modelVariation;
	if (variation == 0)
		variation = 1;
	return Common::String::format("i%s_%03d", _itemClass.c_str(), variation);
}

const Common::UString Item::getModelName() const {
	return Common::String::format("%s_%03d", _itemClass.c_str(), _modelVariation);
}

} // End of namespace KotORBase

} // End of namespace Engines

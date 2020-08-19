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

#ifndef ENGINES_KOTORBASE_ITEM_H
#define ENGINES_KOTORBASE_ITEM_H

#include "src/engines/kotorbase/object.h"

namespace Engines {

namespace KotORBase {

class Item : public Object {
public:
	Item(const Common::UString &item);

	// Basic properties

	const Common::UString &getName() const;
	WeaponWield getWeaponWield() const;
	float getMaxAttackRange() const;
	int getNumDice() const;
	int getDieToRoll() const;

	bool isSlotEquipable(InventorySlot slot) const;
	bool isRangedWeapon() const;

	// Visual properties

	int getBodyVariation() const;
	int getTextureVariation() const;
	const Common::UString getIcon() const;
	const Common::UString getModelName() const;

private:
	int _baseItem;
	Common::UString _itemClass;
	int32_t _equipableSlotsMask;
	WeaponWield _weaponWield;
	bool _rangedWeapon;
	float _maxAttackRange;
	int _numDice;
	int _dieToRoll;

	int _modelVariation;
	int _bodyVariation;
	int _textureVariation;

	void load(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_ITEM_H

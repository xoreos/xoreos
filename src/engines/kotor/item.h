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
 *  An item in a Star Wars: Knights of the Old Republic area.
 */

#ifndef ENGINES_KOTOR_ITEM_H
#define ENGINES_KOTOR_ITEM_H

#include "src/engines/kotorbase/object.h"

namespace Engines {

namespace KotOR {

class Item : public Object {
public:
	Item(const Common::UString &item);

	const Common::UString &getName() const;
	EquipmentSlot getEquipableSlots() const;

	int getBodyVariation() const;
	int getTextureVariation() const;
	const Common::UString getIcon() const;
	const Common::UString getModelName() const;

private:
	int _baseItem;
	Common::UString _itemClass;
	EquipmentSlot _equipableSlots;

	int _modelVariation;
	int _bodyVariation;
	int _textureVariation;

	void load(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_ITEM_H

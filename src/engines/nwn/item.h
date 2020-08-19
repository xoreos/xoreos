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
 *  An inventory item in Neverwinter Nights.
 */

#ifndef ENGINES_NWN_ITEM_H
#define ENGINES_NWN_ITEM_H

#include <memory>

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/object.h"

namespace Engines {

namespace NWN {

class Item : public Object {
public:
	enum Color {
		kColorMetal1   = 0,
		kColorMetal2      ,
		kColorLeather1    ,
		kColorLeather2    ,
		kColorCloth1      ,
		kColorCloth2      ,

		kColorMAX
	};

	Item(const Aurora::GFF3Struct &item);
	~Item();

	// Basic visuals

	void loadModel();   ///< Load the item's model.
	void unloadModel(); ///< Unload the item's model.

	void show(); ///< Show the item's model.
	void hide(); ///< Hide the item's model.

	// Basic properties

	bool isArmor() const;

	uint32_t getColor(Color color) const;
	uint32_t getArmorPart(size_t index) const;

private:
	/** Parts of an armor set. */
	enum ArmorPartType {
		kArmorPartHead       = 0,
		kArmorPartNeck          ,
		kArmorPartTorso         ,
		kArmorPartPelvis        ,
		kArmorPartBelt          ,
		kArmorPartRightFoot     ,
		kArmorPartLeftFoot      ,
		kArmorPartRightShin     ,
		kArmorPartLeftShin      ,
		kArmorPartLeftThigh     ,
		kArmorPartRightThigh    ,
		kArmorPartRightFArm     ,
		kArmorPartLeftFArm      ,
		kArmorPartRightBicep    ,
		kArmorPartLeftBicep     ,
		kArmorPartRightShoul    ,
		kArmorPartLeftShoul     ,
		kArmorPartRightHand     ,
		kArmorPartLeftHand      ,

		kArmorPartMAX
	};

	Common::UString _modelName; ///< The model's resource name.

	uint32_t _baseItem; ///< The index within the baseitem 2DA.

	uint32_t _colors[kColorMAX];         ///< The item's colors.
	uint32_t _armorParts[kArmorPartMAX]; ///< The item's armor parts.

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The item's model.

	void load(const Aurora::GFF3Struct &item);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff);
	void loadPortrait(const Aurora::GFF3Struct &gff);
	void loadArmorParts(const Aurora::GFF3Struct &gff);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_ITEM_H

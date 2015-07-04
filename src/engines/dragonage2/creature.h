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
 *  A creature in a Dragon Age II area.
 */

#ifndef ENGINES_DRAGONAGE2_CREATURE_H
#define ENGINES_DRAGONAGE2_CREATURE_H

#include <vector>
#include <list>

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/dragonage2/object.h"

namespace Aurora {
	class GDAFile;
}

namespace Engines {

namespace DragonAge2 {

class Creature : public Object {
public:
	/** Load from a placeable instance. */
	Creature(const Aurora::GFF3Struct &placeable);
	~Creature();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z, float angle);

	void show();
	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);


private:
	static const size_t kPartVariationCount = 4;

	struct EquipItem {
		Common::UString resRef;

		InventorySlot slot;

		bool stealable;
		bool droopable;

		int32 setNumber;
	};
	typedef std::vector<EquipItem> Items;

	typedef std::list<Graphics::Aurora::Model *> Models;


	uint32 _appearance;

	Common::UString _headMorph;
	uint32 _partVariation[kPartVariationCount];

	Items _items;

	Models _models;


	void load(const Aurora::GFF3Struct &placeable);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff);

	void loadModelsSimple(const Aurora::GDAFile &gda, size_t row);
	void loadModelsWelded(const Aurora::GDAFile &gda, size_t row);
	void loadModelsHead  (const Aurora::GDAFile &gda, size_t row);
	void loadModelsParts (const Aurora::GDAFile &gda, size_t row);

	void loadModelsHeadMorph(bool loadHair = true);
	void loadModelsHeadList(const Aurora::GDAFile &gda, size_t row, bool loadHair = true);

	Common::UString getItemModel(uint32 variation, const Common::UString &prefix,
	                             uint8 *armorType = 0) const;

	Common::UString findEquipModel(InventorySlot slot, const Common::UString &prefix,
	                               uint8 *armorType = 0) const;

	static Common::UString createModelPrefix(const Aurora::GDAFile &gda, size_t row);
	static Common::UString createModelPart(const Aurora::GDAFile &gda, size_t row,
	                                       const Common::UString &prefix);
};

} // End of namespace Dragon Age

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_CREATURE_H

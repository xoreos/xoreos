/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/item.h
 *  NWN inventory item.
 */

#ifndef ENGINES_NWN_ITEM_H
#define ENGINES_NWN_ITEM_H

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/nwn/object.h"

namespace Engines {

namespace NWN {

/** NWN inventory item. */
class Item : public Object {
public:
	Item();

	~Item();

	void loadModel();   ///< Load the situated object's model.
	void unloadModel(); ///< Unload the situated object's model.

	void show(); ///< Show the situated object's model.
	void hide(); ///< Hide the situated object's model.

	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint = 0);
	bool isArmor();
	int getArmorPart(int index);
protected:
	Common::UString _modelName; ///< The model's resource name.

	uint32 _appearanceID; ///< The index within the situated appearance 2DA.
	uint32 _soundAppType; ///< The index within the situated sounds 2DA.

	uint32 _baseitem; ///< The index within the baseitem 2DA.

	Graphics::Aurora::Model *_model; ///< The situated object's model.

	/** Load the situated object from an instance and its blueprint. */

	/** Load object-specific properties. */
	//virtual void loadObject(const Aurora::GFFStruct &gff) = 0;
	/** Load appearance-specific properties. */
	//virtual void loadAppearance() = 0;


private:
	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
	void loadArmorParts(const Aurora::GFFStruct &gff);
	void loadSounds();

	/** index to appropriate armor part. */
	struct ArmorPart {
		uint32 id; ///< Index of the part variant.
	};

	std::vector<ArmorPart> _armorParts; ///< The item's armor parts.
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_ITEM_H

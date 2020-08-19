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

#include <cassert>

#include "src/common/error.h"
#include "src/common/maths.h"
#include "src/common/util.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn/item.h"

namespace Engines {

namespace NWN {

Item::Item(const Aurora::GFF3Struct &item) : Object(kObjectTypeItem),
	_baseItem(Aurora::kFieldIDInvalid) {

	for (size_t i = 0; i < kColorMAX; i++)
		_colors[i] = Aurora::kFieldIDInvalid;
	for (size_t i = 0; i < kArmorPartMAX; i++)
		_armorParts[i] = Aurora::kFieldIDInvalid;

	load(item);
}

Item::~Item() {
}

void Item::load(const Aurora::GFF3Struct &item) {
	Common::UString temp = item.getString("EquippedRes");
	if (temp.empty())
		temp = item.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> uti;
	if (!temp.empty())
		uti.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTI, MKTAG('U', 'T', 'I', ' '), true));

	load(item, uti ? &uti->getTopLevel() : 0);
}

void Item::loadModel() {
	if (_model)
		return;

	if (_modelName.empty()) {
		warning("Item object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());
		return;
	}

	_model.reset(loadModelObject(_modelName));
	if (!_model)
		throw Common::Exception("Failed to load situated object model \"%s\"",
		                        _modelName.c_str());

	// Positioning

	float x, y, z, angle;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z, angle);
	setOrientation(x, y, z, angle);

	// Clickable

	_model->setTag(_tag);
	_model->setClickable(isClickable());

	_ids.push_back(_model->getID());
}

void Item::unloadModel() {
	hide();

	_model.reset();
}

void Item::show() {
	if (_model)
		_model->show();
}

void Item::hide() {
	if (_model)
		_model->hide();
}

void Item::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);     // Instance

	/* TODO:
	 * - BaseItem
	 *   - What exactly is this item (weapon, armor, ...)
	 *   - Index into baseitems.2da
	 * - StackSize
	 *   - The number of items that stack in the inventory
	 */
}

void Item::loadProperties(const Aurora::GFF3Struct &gff) {
	static const char * const kColorNames[kColorMAX] = {
		"Metal1Color"  , "Metal2Color",
		"Leather1Color", "Leather2Color",
		"Cloth1Color"  , "Cloth2Color"
	};

	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	_name = gff.getString("LocalizedName", _name);

	// Description
	_description = gff.getString("Description", _description);

	// This is an index into basitem.2da which contains inventory slot info
	_baseItem = gff.getUint("BaseItem", _baseItem);

	// TODO: Are these armor only?
	for (size_t i = 0; i < kColorMAX; i++)
		_colors[i] = gff.getUint(kColorNames[i], _colors[i]);

	// Armor parts
	loadArmorParts(gff);

	// Portrait
	loadPortrait(gff);

	// Scripts
	readScripts(gff);
}

void Item::loadPortrait(const Aurora::GFF3Struct &gff) {
	uint32_t portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Item::loadArmorParts(const Aurora::GFF3Struct &gff) {
	static const char * const kArmorPartNames[] = {
		"Appearance_Head" ,  // Heads appear to be a special case
		"ArmorPart_Neck"  ,
		"ArmorPart_Torso" ,
		"ArmorPart_Pelvis",
		"ArmorPart_Belt"  ,
		"ArmorPart_RFoot" , "ArmorPart_LFoot" ,
		"ArmorPart_RShin" , "ArmorPart_LShin" ,
		"ArmorPart_LThigh", "ArmorPart_RThigh",
		"ArmorPart_RFArm" , "ArmorPart_LFArm" ,
		"ArmorPart_RBicep", "ArmorPart_LBicep",
		"ArmorPart_RShoul", "ArmorPart_LShoul",
		"ArmorPart_RHand" , "ArmorPart_LHand"
	};

	for (size_t i = 0; i < kArmorPartMAX; i++)
		_armorParts[i] = gff.getUint(kArmorPartNames[i], _armorParts[i]);
}

bool Item::isArmor() const {
	// TODO: This should really be based on the baseitem.2da

	return _armorParts[kArmorPartTorso] != Aurora::kFieldIDInvalid;
}

uint32_t Item::getColor(Color color) const {
	assert((size_t)color < kColorMAX);

	return _colors[(size_t)color];
}

uint32_t Item::getArmorPart(size_t index) const {
	assert(index < kArmorPartMAX);

	return _armorParts[index];
}

} // End of namespace NWN

} // End of namespace Engines

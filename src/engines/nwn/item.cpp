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

/** @file engines/nwn/item.cpp
 *  NWN inventory item.
 */

#include "common/error.h"
#include "common/maths.h"
#include "common/util.h"

#include "aurora/locstring.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/item.h"

namespace Engines {

namespace NWN {

Item::Item() : Object(kObjectTypeItem), _appearanceID(Aurora::kFieldIDInvalid),
	_soundAppType(Aurora::kFieldIDInvalid), _model(0) {

	_armorParts.resize(kArmorPartMAX);
}

Item::~Item() {
	delete _model;
}

void Item::loadModel() {
	if (_model)
		return;

	if (_modelName.empty()) {
		warning("Item object \"%s\" (\"%s\") has no model", _name.c_str(), _tag.c_str());
		return;
	}

	_model = loadModelObject(_modelName);
	if (!_model)
		throw Common::Exception("Failed to load situated object model \"%s\"",
		                        _modelName.c_str());

	// Positioning

	float x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z);
	setOrientation(x, y, z);

	// Clickable

	_model->setTag(_tag);
	_model->setClickable(isClickable());

	_ids.push_back(_model->getID());
}

void Item::unloadModel() {
	hide();

	delete _model;
	_model = 0;
}

void Item::show() {
	if (_model)
		_model->show();
}

void Item::hide() {
	if (_model)
		_model->hide();
}

void Item::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
	// UTI def has a baseitem (index into baseitem.2da, determines equippable slots, model type)
	// armour is 16, modeltype 3
	// can look up armorpart_bodypart id
	// them use that to construct model names

	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance


	// Specialized object properties

	// if (blueprint)
	// 	loadObject(*blueprint); // Blueprint
	// loadObject(instance);    // Instance

	// TODO:
	//  baseitem -- needed for inventory UI interactions
	//  stacksize -- used by inventory UI

	// Appearance

	// if (_appearanceID == Aurora::kFieldIDInvalid)
		// throw Common::Exception("Item object without an appearance");

	// loadAppearance();
	loadSounds();
}

void Item::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	if (gff.hasField("LocalizedName")) {
		Aurora::LocString name;
		gff.getLocString("LocalizedName", name);

		_name = name.getString();
	}

	// Description
	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// This is an index into basitem.2da which contains inventory slot info
	_baseitem = gff.getUint("BaseItem", _baseitem);

	// TODO: Are these armor only?
	_colorMetal1 = gff.getUint("Metal1Color", _colorMetal1);
	_colorMetal2  = gff.getUint("Metal2Color", _colorMetal2);
	_colorLeather1 = gff.getUint("Leather1Color", _colorLeather1);
	_colorLeather2 = gff.getUint("Leather2Color", _colorLeather2);
	_colorCloth1 = gff.getUint("Cloth1Color", _colorCloth1);
	_colorCloth2 = gff.getUint("Cloth2Color", _colorCloth2);

	// Armor parts
	loadArmorParts(gff);

	// Portrait
	loadPortrait(gff);

	// Scripts
	readScripts(gff);
}

void Item::loadPortrait(const Aurora::GFFStruct &gff) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Item::loadSounds() {
	if (_soundAppType == Aurora::kFieldIDInvalid)
		return;
}

static const char *kArmorPartFields[] = {
	"Appearance_Head",  // Heads appear to be a special case
	"ArmorPart_Neck"  ,
	"ArmorPart_Torso" ,
	"ArmorPart_Pelvis",
	"ArmorPart_Belt"  ,
	"ArmorPart_RFoot", "ArmorPart_LFoot" ,
	"ArmorPart_RShin" , "ArmorPart_LShin" ,
	"ArmorPart_LThigh", "ArmorPart_RThigh",
	"ArmorPart_RFArm" , "ArmorPart_LFArm" ,
	"ArmorPart_RBicep", "ArmorPart_LBicep",
	"ArmorPart_RShoul", "ArmorPart_LShoul",
	"ArmorPart_RHand" , "ArmorPart_LHand"
};

void Item::loadArmorParts(const Aurora::GFFStruct &gff)
{
	for (uint i = 0; i < kArmorPartMAX; i++)
		if (gff.hasField(kArmorPartFields[i])) {
			_armorParts[i].id = gff.getUint(kArmorPartFields[i], _armorParts[i].id);
		}
		else
			_armorParts[i].id = 0;
}

bool Item::isArmor() {
	// TODO: This should really be based on the baseitem.2da
	return _armorParts[kArmorPartTorso].id > 0;
}

int Item::getArmorPart(int index) {
	return _armorParts[index].id;
}

} // End of namespace NWN

} // End of namespace Engines

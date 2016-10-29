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

#include "src/common/scopedptr.h"
#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/maths.h"
#include "src/common/error.h"

#include "src/aurora/language.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"
#include "src/aurora/gdafile.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/dragonage2/creature.h"
#include "src/engines/dragonage2/util.h"

namespace Engines {

namespace DragonAge2 {

static const uint32 kMORPID    = MKTAG('M', 'O', 'R', 'P');
static const uint32 kVersion01 = MKTAG('V', '0', '.', '1');

static const uint32 kUTCID     = MKTAG('U', 'T', 'C', ' ');
static const uint32 kUTIID     = MKTAG('U', 'T', 'I', ' ');

using ::Aurora::GFF3File;
using ::Aurora::GFF3Struct;
using ::Aurora::GFF3List;

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;

using Graphics::Aurora::Model;

Creature::Creature() : Object(kObjectTypeCreature) {
	init();
}

Creature::Creature(const GFF3Struct &creature) : Object(kObjectTypeCreature) {
	init();

	load(creature);
}

Creature::~Creature() {
	hide();
}

void Creature::init() {
	_isPC = false;

	_appearanceID = 0xFFFFFFFF;

	for (size_t i = 0; i < kPartVariationCount; i++)
		_partVariation[i] = 0xFFFFFFFF;
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	for (Models::iterator m = _models.begin(); m != _models.end(); ++m)
		(*m)->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	for (Models::iterator m = _models.begin(); m != _models.end(); ++m)
		(*m)->setOrientation(x, y, z, angle);
}

void Creature::show() {
	for (Models::iterator m = _models.begin(); m != _models.end(); ++m)
		(*m)->show();
}

void Creature::hide() {
	for (Models::iterator m = _models.begin(); m != _models.end(); ++m)
		(*m)->hide();
}

bool Creature::isPC() const {
	return _isPC;
}

void Creature::createFakePC() {
	_name.setString(LangMan.getCurrentLanguageText(), LangMan.getCurrentGender(), "Fakoo McFakeston");
	_tag = Common::UString::format("[PC: %s]", _name.getString().c_str());

	_isPC = true;

	enableEvents(false);
}

void Creature::enter() {
	status("Creature \"%s\" (\"%s\"): \"%s\"",
	       _tag.c_str(), _name.getString().c_str(), _description.getString().c_str());

	highlight(true);
}

void Creature::leave() {
	highlight(false);
}

void Creature::highlight(bool enabled) {
	for (Models::iterator m = _models.begin(); m != _models.end(); ++m)
		(*m)->drawBound(enabled);
}

bool Creature::click(Object *triggerer) {
	runScript(kEventTypeClick       , this, triggerer);
	runScript(kEventTypeConversation, this, triggerer);

	return true;
}

void Creature::load(const GFF3Struct &creature) {
	_resRef = creature.getString("TemplateResRef");

	Common::ScopedPtr<GFF3File> utc;
	if (!_resRef.empty())
		utc.reset(loadOptionalGFF3(_resRef, Aurora::kFileTypeUTC, kUTCID));

	load(creature, utc ? &utc->getTopLevel() : 0);
}

Common::UString Creature::createModelPrefix(const Aurora::GDAFile &gda, size_t row) {
	if (row == Aurora::GDAFile::kInvalidRow)
		return "";

	const Common::UString race   = gda.getString(row, "ModelRace");
	const Common::UString gender = gda.getString(row, "ModelGenderOverride");

	Common::UString prefix = race + gender;
	if (prefix.empty())
		prefix = "shared";

	return prefix;
}

Common::UString Creature::createModelPart(const Aurora::GDAFile &gda, size_t row,
                                          const Common::UString &prefix) {
	if (row == Aurora::GDAFile::kInvalidRow)
		return "";

	const Common::UString modelPath = gda.getString(row, "ModelPath");
	const Common::UString model     = gda.getString(row, "Model");

	if (modelPath.empty() && model.empty())
		return "";

	return modelPath + "\\" + prefix + "\\" + model;
}

Common::UString Creature::getItemModel(uint32 variation, const Common::UString &prefix,
                                       uint8 *armorType) const {

	if (armorType)
		*armorType = 0;

	const Aurora::GDAFile &variations = getMGDA(kWorksheetItemVariations);
	const size_t variationRow = variations.findRow(variation);
	if (!variation || (variationRow == Aurora::GDAFile::kInvalidRow))
		return "";

	const Common::UString model = createModelPart(variations, variationRow, prefix);
	if (model.empty())
		return "";

	if (armorType)
		*armorType = variations.getInt(variationRow, "MaterialGroup");

	return model;
}

void Creature::loadModelsSimple(const Aurora::GDAFile &gda, size_t row) {
	// Simple, single mesh

	Model *model = loadModelObject(gda.getString(row, "ModelName"));
	if (model)
		_models.push_back(model);
}

void Creature::loadModelsWelded(const Aurora::GDAFile &gda, size_t row) {
	// Welded, single mesh with baked weapons

	Model *model = loadModelObject(gda.getString(row, "ModelName"));
	if (model)
		_models.push_back(model);
}

void Creature::loadModelsHead(const Aurora::GDAFile &gda, size_t row) {
	// Head, single mesh for body + head model

	Model *model = loadModelObject(gda.getString(row, "ModelName"));
	if (model)
		_models.push_back(model);

	const Common::UString prefix = createModelPrefix(gda, row);

	bool haveHelm = false;
	if ((model = loadModelObject(findEquipModel(kInventorySlotHead, prefix)))) {
		haveHelm = true;

		_models.push_back(model);
	}

	if (!_headMorph.empty())
		loadModelsHeadMorph(!haveHelm);
	else
		loadModelsHeadList(gda, row, !haveHelm);
}

void Creature::loadModelsHeadMorph(bool loadHair) {
	static const size_t kHairPart = 2;

	try {
		GFF4File mor(_headMorph, Aurora::kFileTypeMOR, kMORPID);
		if (mor.getTypeVersion() != kVersion01)
			throw Common::Exception("Unsupported MOR version %s", Common::debugTag(mor.getTypeVersion()).c_str());

		std::vector<Common::UString> parts;
		mor.getTopLevel().getString(kGFF4MorphParts, parts);

		Model *model = 0;
		for (size_t i = 0; i < parts.size(); i++) {
			if ((i == kHairPart) && !loadHair)
				continue;

			if ((model = loadModelObject(parts[i])))
				_models.push_back(model);
		}

	} catch (...) {
	}
}

void Creature::loadModelsHeadList(const Aurora::GDAFile &gda, size_t row, bool loadHair) {
	static const size_t kHairPart = 2;

	static const char * const kSheetColumns[kPartVariationCount] =
		{"Head_Worksheet", "Eyes_Worksheet", "Hair_Worksheet", "Beard_Worksheet"};

	const Common::UString prefix = createModelPrefix(gda, row);

	for (size_t i = 0; i < kPartVariationCount; i++) {
		const size_t sheetIndex = gda.getInt(row, kSheetColumns[i]);
		if (sheetIndex == 0)
			continue;

		if ((i == kHairPart) && !loadHair)
			continue;

		const Aurora::GDAFile &sheet = getMGDA(sheetIndex);

		const size_t sheetRow = sheet.findRow(_partVariation[i]);
		if (sheetRow == Aurora::GDAFile::kInvalidRow)
			continue;

		Model *model = loadModelObject(createModelPart(sheet, sheetRow, prefix));
		if (model)
			_models.push_back(model);
	}
}

void Creature::loadModelsParts(const Aurora::GDAFile &gda, size_t row) {
	// Parts, various models for each body part

	const Common::UString prefix = createModelPrefix(gda, row);

	const uint32 chestModel  = gda.getInt(row, "ChestModelVariation");
	const uint32 glovesModel = gda.getInt(row, "GlovesModelVariation");
	const uint32 bootsModel  = gda.getInt(row, "BootsModelVariation");
	const uint32 helmModel   = gda.getInt(row, "HelmModelVariation");

	static const uint32 kNakedTorso  = 10;
	static const uint32 kNakedGloves = 11;
	static const uint32 kNakedBoots  = 12;

	uint8  armorType = 0;
	Model *model     = 0;

	// Torso: creature model -> appearance override -> equipped chest item -> naked

	model = loadModelObject(gda.getString(row, "ModelName"));
	if (!model)
		model = loadModelObject(getItemModel(chestModel, prefix));
	if (!model)
		model = loadModelObject(findEquipModel(kInventorySlotChest, prefix, &armorType));
	if (!model)
		model = loadModelObject(getItemModel(kNakedTorso, prefix, &armorType));

	if (model)
		_models.push_back(model);

	// Armor of type 5 (clothing) already includes hands and feet in the model...
	if (armorType != 5) {
		// Gloves: appearance override -> equipped gloves item -> naked

		model = loadModelObject(getItemModel(glovesModel, prefix));
		if (!model)
			model = loadModelObject(findEquipModel(kInventorySlotGloves, prefix));
		if (!model)
			model = loadModelObject(getItemModel(kNakedGloves, prefix));

		if (model)
			_models.push_back(model);

		// Boots: appearance override -> equipped boots item -> naked

		model = loadModelObject(getItemModel(bootsModel, prefix));
		if (!model)
			model = loadModelObject(findEquipModel(kInventorySlotBoots, prefix));
		if (!model)
			model = loadModelObject(getItemModel(kNakedBoots, prefix));

		if (model)
			_models.push_back(model);
	}

	// Helm: appearance override -> equipped helm item

	model = loadModelObject(getItemModel(helmModel, prefix));
	if (!model)
		model = loadModelObject(findEquipModel(kInventorySlotHead, prefix));

	bool haveHelm = false;
	if (model) {
		haveHelm = true;

		_models.push_back(model);
	}

	// Head: morph -> part list

	if (!_headMorph.empty())
		loadModelsHeadMorph(!haveHelm);
	else
		loadModelsHeadList(gda, row, !haveHelm);
}

Common::UString Creature::findEquipModel(InventorySlot slot, const Common::UString &prefix,
                                         uint8 *armorType) const {
	if (armorType)
		*armorType = 0;

	const Aurora::GDAFile &baseItems = getMGDA(kWorksheetItems);

	for (Items::const_iterator item = _items.begin(); item != _items.end(); ++item) {
		if (item->slot != slot)
			continue;

		try {
			GFF3File uti(item->resRef, Aurora::kFileTypeUTI, kUTIID);
			const GFF3Struct &utiTop = uti.getTopLevel();

			const uint32 baseItem  = (uint32) ((int32) utiTop.getSint("BaseItem", -1));
			const uint32 variation = utiTop.getUint("ModelVariation", 0xFFFFFFFF);

			const size_t itemRow = baseItems.findRow(baseItem);
			if (itemRow == Aurora::GDAFile::kInvalidRow)
				continue;

			return getItemModel(variation, prefix, armorType);

		} catch (...) {
		}
	}

	return "";
}

void Creature::load(const GFF3Struct &instance, const GFF3Struct *blueprint = 0) {
	if (blueprint)
		loadProperties(*blueprint);
	loadProperties(instance);

	const Aurora::GDAFile &gda = getMGDA(kWorksheetAppearances);
	const size_t row = gda.findRow(_appearanceID);

	const Common::UString modelType = gda.getString(row, "ModelType");

	if      (modelType == "S")
		loadModelsSimple(gda, row);
	else if (modelType == "W")
		loadModelsWelded(gda, row);
	else if (modelType == "H")
		loadModelsHead(gda, row);
	else if (modelType == "P")
		loadModelsParts(gda, row);

	const float scaleX = gda.getFloat(row, "ModelScaleX", 1.0f);
	const float scaleY = gda.getFloat(row, "ModelScaleY", 1.0f);
	const float scaleZ = gda.getFloat(row, "ModelScaleZ", 1.0f);

	for (Models::iterator m = _models.begin(); m != _models.end(); ++m) {
		(*m)->setScale(scaleX, scaleY, scaleZ);

		(*m)->setTag(_tag);
		(*m)->setClickable(isClickable());

		_ids.push_back((*m)->getID());
	}

	syncPosition();
	syncOrientation();
}

void Creature::loadProperties(const GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name and description
	gff.getLocString("LocName"    , _name);
	gff.getLocString("Description", _description);

	// Conversation
	_conversation = gff.getString("DialogResRef", _conversation);

	// Static and usable
	_static = !gff.getBool("Active"      , !_static);
	_usable =  gff.getBool("IsSelectable",  _usable);

	// Appearance
	_appearanceID = gff.getUint("Appearance", _appearanceID);

	if (gff.hasField("Appearance Data")) {
		const GFF3Struct &app = gff.getStruct("Appearance Data");

		_appearanceID = (uint32) ((int32) app.getSint("Appearance_Type", (int32) _appearanceID));

		_headMorph = app.getString("HeadMorph", _headMorph);

		if (app.hasField("PartList")) {
			const GFF3List &parts = app.getList("PartList");

			for (size_t i = 0; i < kPartVariationCount; i++)
				_partVariation[i] = (i < parts.size()) ? parts[i]->getUint("PartVariation") : 0xFFFFFFFF;
		}
	}

	// Equipped items
	if (gff.hasField("Equip_ItemList")) {
		const GFF3List &itemList = gff.getList("Equip_ItemList");
		_items.resize(itemList.size());

		for (size_t i = 0; i < itemList.size(); i++) {
			_items[i].slot = (InventorySlot) itemList[i]->getID();

			_items[i].resRef = itemList[i]->getString("TemplateResRef");

			_items[i].stealable = itemList[i]->getBool("Stealable");
			_items[i].droopable = itemList[i]->getBool("Droppable");

			_items[i].setNumber = itemList[i]->getSint("SetNumber", -1);
		}
	}

	// Position
	if (gff.hasField("XPosition")) {
		const float position[3] = {
			(float) gff.getDouble("XPosition"),
			(float) gff.getDouble("YPosition"),
			(float) gff.getDouble("ZPosition")
		};

		setPosition(position[0], position[1], position[2]);
	}

	// Orientation
	if (gff.hasField("XOrientation")) {
		const float orientation[4] = {
			(float) gff.getDouble("XOrientation"),
			(float) gff.getDouble("YOrientation"),
			(float) gff.getDouble("ZOrientation"),
			(float) Common::rad2deg(acos(gff.getDouble("WOrientation")) * 2.0)
		};

		setOrientation(orientation[0], orientation[1], orientation[2], orientation[3]);
	}

	// Variables and script
	readVarTable(gff);
	readScript(gff);
	enableEvents(true);
}

} // End of namespace Dragon Age

} // End of namespace Engines

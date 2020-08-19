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
 *  A creature in a Neverwinter Nights area.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/readfile.h"
#include "src/common/configman.h"

#include "src/aurora/types.h"
#include "src/aurora/talkman.h"
#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/pltfile.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn/creature.h"
#include "src/engines/nwn/item.h"
#include "src/engines/nwn/area.h"

static const uint32_t kBICID = MKTAG('B', 'I', 'C', ' ');

namespace Engines {

namespace NWN {

Creature::Class::Class() : classID(UINT32_MAX), level(UINT16_MAX), domain1(UINT8_MAX),
    domain2(UINT8_MAX), school(UINT8_MAX) {
}

Creature::Associate::Associate(AssociateType t, Creature *a) : type(t), associate(a) {
}


Creature::BodyPart::BodyPart() : id(Aurora::kFieldIDInvalid) {
}

Creature::Creature() : Object(kObjectTypeCreature) {
	init();
}

Creature::Creature(const Aurora::GFF3Struct &creature) : Object(kObjectTypeCreature) {
	init();

	load(creature);
}

Creature::Creature(const Common::UString &bic, bool local) : Object(kObjectTypeCreature) {
	init();

	loadCharacter(bic, local);
}

Creature::~Creature() {
	if (_master)
		_master->removeAssociate(*this);

	for (std::list<Associate>::iterator a = _associates.begin(); a != _associates.end(); ++a)
		a->associate->setMaster(0);

	hide();
}

void Creature::init() {
	_lastChangedGUIDisplay = 0;

	_gender = kGenderNone;
	_race   = kRaceInvalid;

	_portrait = "gui_po_nwnlogo_";

	_isPC = false;
	_isDM = false;

	_age = 0;

	_xp = 0;

	_baseHP    = 0;
	_bonusHP   = 0;
	_currentHP = 0;

	_hitDice = 0;

	_goodEvil = 0;
	_lawChaos = 0;

	_appearanceID = Aurora::kFieldIDInvalid;
	_phenotype    = Aurora::kFieldIDInvalid;

	_startingPackage = 0;
	_skills.assign(28, 0);

	_colorSkin    = Aurora::kFieldIDInvalid;
	_colorHair    = Aurora::kFieldIDInvalid;
	_colorTattoo1 = Aurora::kFieldIDInvalid;
	_colorTattoo2 = Aurora::kFieldIDInvalid;

	_colorMetal1 = Aurora::kFieldIDInvalid;
	_colorMetal2 = Aurora::kFieldIDInvalid;
	_colorLeather1 = Aurora::kFieldIDInvalid;
	_colorLeather2 = Aurora::kFieldIDInvalid;
	_colorCloth1 = Aurora::kFieldIDInvalid;
	_colorCloth2 = Aurora::kFieldIDInvalid;

	_master = 0;

	_isCommandable = true;

	for (size_t i = 0; i < kAbilityMAX; i++)
		_abilities[i] = 0;

	_bodyParts.resize(kBodyPartMAX);
}

void Creature::show() {
	if (_model)
		_model->show();
}

void Creature::hide() {
	leave();

	hideTooltip();

	if (_model)
		_model->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z, float angle) {
	Object::setOrientation(x, y, z, angle);
	Object::getOrientation(x, y, z, angle);

	if (_model)
		_model->setOrientation(x, y, z, angle);
}

uint32_t Creature::lastChangedGUIDisplay() const {
	return _lastChangedGUIDisplay;
}

const Common::UString &Creature::getFirstName() const {
	return _firstName;
}

const Common::UString &Creature::getLastName() const {
	return _lastName;
}

Gender Creature::getGender() const {
	return _gender;
}

void Creature::setGender(Gender gender) {
	_gender = gender;
}

bool Creature::isFemale() const {
	// Male and female are hardcoded.  Other genders (none, both, other)
	// count as male when it comes to tokens in text strings.

	return _gender == kGenderFemale;
}

uint32_t Creature::getRace() const {
	return _race;
}

void Creature::setRace(uint32_t race) {
	if (race >= kRaceInvalid)
		error("Unable to set race, raceID is invalid.");

	_race = race;
}

void Creature::setPortrait(const Common::UString &portrait) {
	_portrait = portrait;

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

const Common::UString &Creature::getPortrait() const {
	return _portrait;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isDM() const {
	return _isDM;
}

uint32_t Creature::getAge() const {
	return _age;
}

uint32_t Creature::getXP() const {
	return _xp;
}

int32_t Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32_t Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
}

uint8_t Creature::getStartingPackage() const {
	return _startingPackage;
}

void Creature::setStartingPackage(uint8_t package) {
	_startingPackage = package;
}

void Creature::getDomains(uint32_t classID, uint8_t &domain1, uint8_t &domain2) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass) {
		domain1 = UINT8_MAX;
		domain2 = UINT8_MAX;
		return;
	}

	domain1 = creatureClass->domain1;
	domain2 = creatureClass->domain2;
}

void Creature::setDomains(uint32_t classID, uint8_t domain1, uint8_t domain2) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass)
		return;

	creatureClass->domain1 = domain1;
	creatureClass->domain2 = domain2;
}

bool Creature::hasSpell(uint32_t classID, size_t spellLevel, uint16_t spell) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass)
		return false;

	if (creatureClass->knownList.size() < spellLevel)
		return false;

	std::vector<uint16_t> &spellLvlList = creatureClass->knownList[spellLevel];
	// Check if the creature already knows the spell.
	for (std::vector<uint16_t>::iterator s = spellLvlList.begin(); s != spellLvlList.end(); ++s)
		if (*s == spell)
			return true;

	return false;
}

void Creature::setSchool(uint32_t classID, uint8_t school) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass)
		return;

	creatureClass->school = school;
}

void Creature::setKnownSpell(uint32_t classID, size_t spellLevel, uint16_t spell) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass)
		return;

	if (creatureClass->knownList.size() < spellLevel + 1)
		creatureClass->knownList.resize(spellLevel + 1);

	std::vector<uint16_t> &spellLvlList = creatureClass->knownList[spellLevel];
	// Check if the creature already knows the spell.
	for (std::vector<uint16_t>::iterator s = spellLvlList.begin(); s != spellLvlList.end(); ++s)
		if (*s == spell)
			return;

	spellLvlList.push_back(spell);
	return;
}

void Creature::setMemorizedSpell(uint32_t classID, size_t spellLevel, uint16_t spell) {
	Class *creatureClass = findClass(classID);
	if (!creatureClass)
		return;

	if (creatureClass->memorizedList.size() < spellLevel + 1)
		creatureClass->memorizedList.resize(spellLevel + 1);

	std::vector<uint16_t> &spellLvlList = creatureClass->memorizedList[spellLevel];
	// Check if the creature already knows the spell.
	for (std::vector<uint16_t>::iterator s = spellLvlList.begin(); s != spellLvlList.end(); ++s)
		if (*s == spell)
			return;

	spellLvlList.push_back(spell);
	return;
}

void Creature::setAppearance(uint32_t appearanceID) {
	_appearanceID = appearanceID;
}

void Creature::setPhenotype(uint32_t phenotype) {
	_phenotype = phenotype;
}

void Creature::setColorSkin(uint32_t colorSkin) {
	_colorSkin = colorSkin;
}

void Creature::setColorHair(uint32_t colorHair) {
	_colorHair = colorHair;
}

void Creature::setColorTatto1(uint32_t colorTattoo1) {
	_colorTattoo1 = colorTattoo1;
}

void Creature::setColorTatto2(uint32_t colorTattoo2) {
	_colorTattoo2 = colorTattoo2;
}

void Creature::setHead(uint32_t headID) {
	_bodyParts[kBodyPartHead].id = headID;
}

void Creature::addEquippedItem(Item *item) {
	_equippedItems.push_back(item);
}

void Creature::setArea(Area *area) {
	Object::setArea(area);

	if (area && _model && _environmentMap.equalsIgnoreCase("default"))
		_model->setEnvironmentMap(area->getEnvironmentMap());
}

void Creature::addAssociate(Creature &henchman, AssociateType type) {
	removeAssociate(henchman);

	assert(!henchman.getMaster());

	_associates.push_back(Associate(type, &henchman));
	henchman.setMaster(this);
}

void Creature::removeAssociate(Creature &henchman) {
	for (std::list<Associate>::iterator a = _associates.begin(); a != _associates.end(); ++a) {
		if (a->associate == &henchman) {
			assert(a->associate->getMaster() == this);

			a->associate->setMaster(0);
			_associates.erase(a);
			break;
		}
	}
}

Creature *Creature::getAssociate(AssociateType type, size_t nth) const {
	if (_associates.empty())
		return 0;

	Creature *curAssociate = 0;

	std::list<Associate>::const_iterator associate = _associates.begin();
	while (nth-- > 0) {
		while ((associate != _associates.end()) && (associate->type != type))
			++associate;

		if (associate == _associates.end())
			return 0;

		curAssociate = associate->associate;
	}

	return curAssociate;
}

void Creature::setMaster(Creature *master) {
	_master = master;
}

Creature *Creature::getMaster() const {
	return _master;
}

bool Creature::isCommandable() const {
	return _isCommandable;
}

void Creature::setCommandable(bool commandable) {
	_isCommandable = commandable;
}

void Creature::constructPartName(const Common::UString &type, uint32_t id,
		const Common::UString &gender, const Common::UString &race,
		const Common::UString &phenoType, Common::UString &part) {

	part = Common::UString::format("p%s%s%s_%s%03d",
	       gender.c_str(), race.c_str(), phenoType.c_str(), type.c_str(), id);
}

void Creature::constructPartName(const Common::UString &type, uint32_t id,
		const Common::UString &gender, const Common::UString &race,
		const Common::UString &phenoType, const Common::UString &phenoTypeAlt,
		Aurora::FileType fileType, Common::UString &part) {

	constructPartName(type, id, gender, race, phenoType, part);
	if ((fileType == Aurora::kFileTypeNone) || ResMan.hasResource(part, fileType))
		return;

	constructPartName(type, id, gender, race, phenoTypeAlt, part);
	if (!ResMan.hasResource(part, fileType))
		part.clear();
}

void Creature::constructModelName(const Common::UString &type, uint32_t id,
		const Common::UString &gender, const Common::UString &race,
		const Common::UString &phenoType, const Common::UString &phenoTypeAlt,
		Common::UString &model, Common::UString &texture) {

	constructPartName(type, id, gender, race, phenoType, phenoTypeAlt, Aurora::kFileTypeMDL, model);

	constructPartName(type, id, gender, race, phenoType, phenoTypeAlt, Aurora::kFileTypePLT, texture);

	// PLT texture doesn't exist, try a generic human PLT
	if (texture.empty())
		constructPartName(type, id, gender, "H", phenoType, phenoTypeAlt, Aurora::kFileTypePLT, texture);

	// Human PLT texture doesn't exist either, assume it's a non-PLT texture
	if (texture.empty())
		constructPartName(type, id, gender, race, phenoType, phenoTypeAlt, Aurora::kFileTypeNone, texture);
}

// Based on filenames in model2.bif
// These should be read from MDLNAME, NODENAME in capart.2da (in 2da.bif)
static const char * const kBodyPartModels[] = {
	"head"  ,
	"neck"  ,
	"chest" ,
	"pelvis",
	"belt"  ,
	"footr" , "footl" ,
	"shinr" , "shinl" ,
	"legl"  , "legr"  ,
	"forer" , "forel" ,
	"bicepr", "bicepl",
	"shor"  , "shol"  ,
	"handr" , "handl"
};

// Node names taken from pfa0.mdl
static const char * const kBodyPartNodes[] = {
	"head_g"     ,
	"neck_g"     ,
	"torso_g"    ,
	"pelvis_g"   ,
	"belt_g"     ,
	"rfoot_g"    , "lfoot_g"    ,
	"rshin_g"    , "lshin_g"    ,
	"lthigh_g"   , "rthigh_g"   ,
	"rforearm_g" , "lforearm_g" ,
	"rbicep_g"   , "lbicep_g"   ,
	"rshoulder_g", "lshoulder_g",
	"rhand_g"    , "lhand_g"
};

void Creature::getPartModels() {
	const Aurora::TwoDAFile &appearance = TwoDAReg.get2DA("appearance");

	const Aurora::TwoDARow &gender = TwoDAReg.get2DA("gender").getRow((uint) _gender);
	const Aurora::TwoDARow &race   = TwoDAReg.get2DA("racialtypes").getRow(_race);
	const Aurora::TwoDARow &raceAp = appearance.getRow(race.getInt("Appearance"));
	const Aurora::TwoDARow &pheno  = TwoDAReg.get2DA("phenotype").getRow(_phenotype);

	Common::UString genderChar   = gender.getString("GENDER");
	Common::UString raceChar     = raceAp.getString("RACE");
	Common::UString phenoChar    = Common::UString::format("%d", _phenotype);
	Common::UString phenoAltChar = pheno.getString("DefaultPhenoType");

	// Important to capture the supermodel
	_partsSuperModelName = Common::UString::format("p%s%s%s",
	                       genderChar.c_str(), raceChar.c_str(), phenoChar.c_str());

	// Fall back to the default phenotype if required
	if (!ResMan.hasResource(_partsSuperModelName, Aurora::kFileTypeMDL))
		_partsSuperModelName = Common::UString::format("p%s%s%s",
		                       genderChar.c_str(), raceChar.c_str(), phenoAltChar.c_str());

	for (size_t i = 0; i < kBodyPartMAX; i++)
		constructModelName(kBodyPartModels[i],
		                   _bodyParts[i].idArmor > 0 ? _bodyParts[i].idArmor : _bodyParts[i].id,
		                   genderChar, raceChar, phenoChar, phenoAltChar,
		                   _bodyParts[i].modelName, _bodyParts[i].textureName);
}

void Creature::getArmorModels() {
	for (Common::PtrVector<Item>::iterator e = _equippedItems.begin(); e != _equippedItems.end(); ++e) {
		const Item &item = **e;
		if (!item.isArmor())
			continue;

		status("Equipping armour \"%s\" on model \"%s\"", item.getName().c_str(), _tag.c_str());

		// Set the body part models
		for (size_t i = 0; i < kBodyPartMAX; i++) {
			uint32_t id = item.getArmorPart(i);
			if (id != Aurora::kFieldIDInvalid)
				_bodyParts[i].idArmor = id;
		}

		// Set the armour color channels
		_colorMetal1   = item.getColor(Item::kColorMetal1);
		_colorMetal2   = item.getColor(Item::kColorMetal2);
		_colorLeather1 = item.getColor(Item::kColorLeather1);
		_colorLeather2 = item.getColor(Item::kColorLeather2);
		_colorCloth1   = item.getColor(Item::kColorCloth1);
		_colorCloth2   = item.getColor(Item::kColorCloth2);
	}
}

Creature::Class *Creature::findClass(uint32_t classID) {
	for (std::vector<Class>::iterator c = _classes.begin(); c != _classes.end(); ++c) {
		if (c->classID != classID)
			continue;

		return &(*c);
	}

	return 0;
}

void Creature::finishPLTs(const std::list<Graphics::Aurora::TextureHandle> &plts) {
	for (std::list<Graphics::Aurora::TextureHandle>::const_iterator p = plts.begin(); p != plts.end(); ++p) {
		Graphics::Aurora::PLTFile *plt = dynamic_cast<Graphics::Aurora::PLTFile *>(&p->getTexture());
		if (!plt)
			continue;

		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerSkin    , _colorSkin);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerHair    , _colorHair);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo1 , _colorTattoo1);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo2 , _colorTattoo2);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerMetal1  , _colorMetal1);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerMetal2  , _colorMetal2);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerLeather1, _colorLeather1);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerLeather2, _colorLeather2);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerCloth1  , _colorCloth1);
		plt->setLayerColor(Graphics::Aurora::PLTFile::kLayerCloth2  , _colorCloth2);

		plt->rebuild();
	}
}

void Creature::loadModel() {
	if (_model)
		return;

	if (_appearanceID == Aurora::kFieldIDInvalid) {
		warning("Creature \"%s\" has no appearance", _tag.c_str());
		return;
	}

	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearanceID);

	if (_portrait.empty())
		_portrait = appearance.getString("PORTRAIT");

	_environmentMap = appearance.getString("ENVMAP");

	if (appearance.getString("MODELTYPE") == "P") {
		getArmorModels();
		getPartModels();
		_model.reset(loadModelObject(_partsSuperModelName));

		for (size_t i = 0; i < kBodyPartMAX; i++) {
			if (_bodyParts[i].modelName.empty())
				continue;

			TextureMan.startRecordNewTextures();

			// Try to load in the corresponding part model
			Graphics::Aurora::Model *partModel = loadModelObject(_bodyParts[i].modelName, _bodyParts[i].textureName);
			if (!partModel)
				continue;

			// Add the loaded model to the appropriate part node
			_model->attachModel(kBodyPartNodes[i], partModel);

			std::list<Common::UString> newTextures;
			TextureMan.stopRecordNewTextures(newTextures);

			for (std::list<Common::UString>::const_iterator t = newTextures.begin(); t != newTextures.end(); ++t) {
				Graphics::Aurora::TextureHandle texture = TextureMan.getIfExist(*t);
				if (texture.empty())
					continue;

				_bodyParts[i].textures.push_back(texture);
			}

			finishPLTs(_bodyParts[i].textures);
		}

	} else
		_model.reset(loadModelObject(appearance.getString("RACE")));

	// Positioning

	float x, y, z, angle;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z, angle);
	setOrientation(x, y, z, angle);

	// Clickable

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());

		if (!_environmentMap.empty()) {
			Common::UString environmentMap = _environmentMap;
			if (environmentMap.equalsIgnoreCase("default"))
				environmentMap = _area ? _area->getEnvironmentMap() : "";

			_model->setEnvironmentMap(environmentMap);
		}
	}
}

void Creature::unloadModel() {
	hide();

	destroyTooltip();

	_model.reset();
}

void Creature::loadCharacter(const Common::UString &bic, bool local) {
	std::unique_ptr<Aurora::GFF3File> gff(openPC(bic, local));

	load(gff->getTopLevel(), 0);

	// All BICs should be PCs.
	_isPC = true;

	// Set the PC tag to something recognizable for now.
	// Let's hope no script depends on it being "".

	_tag = Common::UString::format("[PC: %s]", _name.c_str());

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

void Creature::load(const Aurora::GFF3Struct &creature) {
	const Common::UString temp = creature.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> utc;
	if (!temp.empty())
		utc.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' '), true));

	load(creature, utc ? &utc->getTopLevel() : 0);

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

void Creature::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

static const char * const kBodyPartFields[] = {
	"Appearance_Head",
	"BodyPart_Neck"  ,
	"BodyPart_Torso" ,
	"BodyPart_Pelvis",
	"BodyPart_Belt"  ,
	"ArmorPart_RFoot", "BodyPart_LFoot" ,
	"BodyPart_RShin" , "BodyPart_LShin" ,
	"BodyPart_LThigh", "BodyPart_RThigh",
	"BodyPart_RFArm" , "BodyPart_LFArm" ,
	"BodyPart_RBicep", "BodyPart_LBicep",
	"BodyPart_RShoul", "BodyPart_LShoul",
	"BodyPart_RHand" , "BodyPart_LHand"
};

void Creature::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Name

	_firstName = gff.getString("FirstName", _firstName);
	_lastName  = gff.getString("LastName" , _lastName);

	_name = _firstName + " " + _lastName;
	_name.trim();

	// Description

	_description = gff.getString("Description", _description);

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Portrait

	loadPortrait(gff, _portrait);

	// Gender
	_gender = (Gender) gff.getUint("Gender", (uint64_t) _gender);

	// Race
	_race = gff.getUint("Race", _race);

	// Subrace
	_subRace = gff.getString("Subrace", _subRace);

	// PC and DM
	_isPC = gff.getBool("IsPC", _isPC);
	_isDM = gff.getBool("IsDM", _isDM);

	// Age
	_age = gff.getUint("Age", _age);

	// Experience
	_xp = gff.getUint("Experience", _xp);

	// Abilities
	_abilities[kAbilityStrength]     = gff.getUint("Str", _abilities[kAbilityStrength]);
	_abilities[kAbilityDexterity]    = gff.getUint("Dex", _abilities[kAbilityDexterity]);
	_abilities[kAbilityConstitution] = gff.getUint("Con", _abilities[kAbilityConstitution]);
	_abilities[kAbilityIntelligence] = gff.getUint("Int", _abilities[kAbilityIntelligence]);
	_abilities[kAbilityWisdom]       = gff.getUint("Wis", _abilities[kAbilityWisdom]);
	_abilities[kAbilityCharisma]     = gff.getUint("Cha", _abilities[kAbilityCharisma]);

	// Classes
	loadClasses(gff, _classes, _hitDice);

	// Package
	_startingPackage = gff.getUint("StartingPackage", _startingPackage);

	// Skills
	if (gff.hasField("SkillList")) {
		_skills.clear();

		const Aurora::GFF3List &skills = gff.getList("SkillList");
		for (Aurora::GFF3List::const_iterator s = skills.begin(); s != skills.end(); ++s) {
			const Aurora::GFF3Struct &skill = **s;

			_skills.push_back(skill.getSint("Rank"));
		}
	}

	// Feats
	if (gff.hasField("FeatList")) {
		_feats.clear();

		const Aurora::GFF3List &feats = gff.getList("FeatList");
		for (Aurora::GFF3List::const_iterator f = feats.begin(); f != feats.end(); ++f) {
			const Aurora::GFF3Struct &feat = **f;

			_feats.push_back(feat.getUint("Feat"));
		}
	}

	// Deity
	_deity = gff.getString("Deity", _deity);

	// Health
	if (gff.hasField("HitPoints")) {
		_baseHP    = gff.getSint("HitPoints");
		_bonusHP   = gff.getSint("MaxHitPoints", _baseHP) - _baseHP;
		_currentHP = gff.getSint("CurrentHitPoints", _baseHP);
	}

	// Alignment

	_goodEvil = gff.getUint("GoodEvil", _goodEvil);
	_lawChaos = gff.getUint("LawfulChaotic", _lawChaos);

	// Appearance

	_appearanceID = gff.getUint("Appearance_Type", _appearanceID);
	_phenotype    = gff.getUint("Phenotype"      , _phenotype);

	// Body parts
	for (size_t i = 0; i < kBodyPartMAX; i++) {
		_bodyParts[i].id      = gff.getUint(kBodyPartFields[i], _bodyParts[i].id);
		_bodyParts[i].idArmor = 0;
	}

	// Colors
	_colorSkin    = gff.getUint("Color_Skin", _colorSkin);
	_colorHair    = gff.getUint("Color_Hair", _colorHair);
	_colorTattoo1 = gff.getUint("Color_Tattoo1", _colorTattoo1);
	_colorTattoo2 = gff.getUint("Color_Tattoo2", _colorTattoo2);

	// Equipped Items
	loadEquippedItems(gff);

	// Scripts
	readScripts(gff);
}

void Creature::loadPortrait(const Aurora::GFF3Struct &gff, Common::UString &portrait) {
	uint32_t portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");

		Common::UString portrait2DA = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait2DA.empty())
			portrait = "po_" + portrait2DA;
	}

	portrait = gff.getString("Portrait", portrait);
}

void Creature::loadEquippedItems(const Aurora::GFF3Struct &gff) {
	if (!gff.hasField("Equip_ItemList"))
		return;

	const Aurora::GFF3List &cEquipped = gff.getList("Equip_ItemList");
	for (Aurora::GFF3List::const_iterator e = cEquipped.begin(); e != cEquipped.end(); ++e)
		_equippedItems.push_back(new Item(**e));
}

void Creature::loadClasses(const Aurora::GFF3Struct &gff,
                           std::vector<Class> &classes, uint8_t &hitDice) {

	if (!gff.hasField("ClassList"))
		return;

	classes.clear();
	hitDice = 0;

	const Aurora::GFF3List &cClasses = gff.getList("ClassList");
	for (Aurora::GFF3List::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
		classes.push_back(Class());

		const Aurora::GFF3Struct &cClass = **c;

		classes.back().classID = cClass.getUint("Class");
		classes.back().level   = cClass.getUint("ClassLevel");

		hitDice += classes.back().level;
	}
}

const Common::UString &Creature::getConvRace() const {
	const uint32_t strRef = TwoDAReg.get2DA("racialtypes").getRow(_race).getInt("ConverName");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvrace() const {
	const uint32_t strRef = TwoDAReg.get2DA("racialtypes").getRow(_race).getInt("ConverNameLower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvRaces() const {
	const uint32_t strRef = TwoDAReg.get2DA("racialtypes").getRow(_race).getInt("NamePlural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getSubRace() const {
	return _subRace;
}

void Creature::getClass(uint32_t position, uint32_t &classID, uint16_t &level) const {
	if (position >= _classes.size()) {
		classID = kClassInvalid;
		level   = 0;
		return;
	}

	classID = _classes[position].classID;
	level   = _classes[position].level;
}

uint16_t Creature::getClassLevel(uint32_t classID) const {
	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c)
		if (c->classID == classID)
			return c->level;

	return 0;
}

void Creature::changeClassLevel(uint32_t classID, int16_t levelChange) {
	for (std::vector<Class>::iterator c = _classes.begin(); c != _classes.end(); ++c) {
		if (c->classID != classID)
			continue;

		if (c->level + levelChange >= 0)
			c->level += levelChange;

		return;
	}

	if (levelChange < 0)
		return;

	// If there is no level in the class, create it.
	Class newClass = Class();
	newClass.classID = classID;
	newClass.level = levelChange;
	_classes.push_back(newClass);
}

const Common::UString &Creature::getConvClass() const {
	const uint32_t classID = _classes.front().classID;
	const uint32_t strRef  = TwoDAReg.get2DA("classes").getRow(classID).getInt("Name");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvclass() const {
	const uint32_t classID = _classes.front().classID;
	const uint32_t strRef  = TwoDAReg.get2DA("classes").getRow(classID).getInt("Lower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvClasses() const {
	const uint32_t classID = _classes.front().classID;
	const uint32_t strRef  = TwoDAReg.get2DA("classes").getRow(classID).getInt("Plural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getDeity() const {
	return _deity;
}

uint8_t Creature::getGoodEvil() const {
	return _goodEvil;
}

void Creature::setGoodEvil(uint8_t goodness) {
	_goodEvil = goodness;
}

uint8_t Creature::getLawChaos() const {
	return _lawChaos;
}

void Creature::setLawChaos(uint8_t lawfulness) {
	_lawChaos = lawfulness;
}

void Creature::setSoundSet(uint32_t soundSet) {
	_soundSet = soundSet;
}

Common::UString Creature::getClassString() const {
	Common::UString classString;

	getClassString(_classes, classString);

	return classString;
}

uint8_t Creature::getHitDice() const {
	return _hitDice;
}

uint8_t Creature::getAbility(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

void Creature::setAbility(Ability ability, uint8_t score) {
	assert((ability >= 0) && ability < kAbilityMAX);

	_abilities[ability] = score;
}

int8_t Creature::getSkillRank(uint32_t skill) const {
	if (skill >= _skills.size())
		return -1;

	return _skills[skill];
}

void Creature::setSkillRank(size_t skill, uint8_t rank) {
	if (skill >= _skills.size())
		return;

	_skills[skill] = rank;
}

void Creature::setFeat(uint32_t feat) {
	if (hasFeat(feat))
		return;

	_feats.push_back(feat);
}

bool Creature::hasFeat(uint32_t feat) const {
	for (std::vector<uint32_t>::const_iterator f = _feats.begin(); f != _feats.end(); ++f)
		if (*f == feat)
			return true;

	return false;
}

void Creature::enter() {
	highlight(true);
}

void Creature::leave() {
	highlight(false);
}

void Creature::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);

	if (enabled)
		showFeedbackTooltip();
	else
		hideTooltip();
}

bool Creature::click(Object *triggerer) {
	// Try the onDialog script first
	if (hasScript(kScriptDialogue))
		return runScript(kScriptDialogue, this, triggerer);

	// Next, look we have a generic onClick script
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	// Lastly, try to start a conversation directly
	return beginConversation(triggerer);
}

bool Creature::createTooltip(Tooltip::Type type) {
	if (!_model)
		return false;

	if (!_tooltip) {
		_tooltip = std::make_unique<Tooltip>(type, *_model);

		_tooltip->setAlign(0.5f);
		_tooltip->setPortrait(_portrait);
	}

	return true;
}

void Creature::playAnimation(const Common::UString &animation, bool restart,
                             float length, float speed) {
	if (!_model)
		return;

	if (animation.empty()) {
		_model->playDefaultAnimation();
		return;
	}

	_model->playAnimation(animation, restart, length, speed);
}

void Creature::playAnimation(Animation animation, bool restart, float length, float speed) {
	if (!_model)
		return;

	if (!isAnimationLooping(animation))
		length = 0.0f;

	Common::UString anim = getCreatureAnimationName(animation, false);
	if (_model->hasAnimation(anim))
		anim = getCreatureAnimationName(animation, true);

	warning("Creature::playAnimation(%d = \"%s\", %s, %f, %f)", (int) animation, anim.c_str(),
	        restart ? "true" : "false", length, speed);

	playAnimation(anim, restart, length, speed);
}

void Creature::getPCListInfo(const Common::UString &bic, bool local,
                             Common::UString &name, Common::UString &classes,
                             Common::UString &portrait) {


	std::unique_ptr<Aurora::GFF3File> gff(openPC(bic, local));

	const Aurora::GFF3Struct &top = gff->getTopLevel();

	// Reading name
	const Common::UString firstName = top.getString("FirstName");
	const Common::UString lastName  = top.getString("LastName");

	name = firstName + " " + lastName;
	name.trim();

	// Reading portrait (failure non-fatal)
	try {
		loadPortrait(top, portrait);
	} catch (...) {
		portrait.clear();

		Common::exceptionDispatcherWarning("Can't read portrait for PC \"%s\"", bic.c_str());
	}

	// Reading classes
	std::vector<Class> classLevels;
	uint8_t hitDice;

	loadClasses(top, classLevels, hitDice);
	getClassString(classLevels, classes);

	classes = "(" + classes + ")";
}

Aurora::GFF3File *Creature::openPC(const Common::UString &bic, bool local) {
	const Common::UString pcDir  = ConfigMan.getString(local ? "NWN_localPCDir" : "NWN_serverPCDir");
	const Common::UString pcFile = pcDir + "/" + bic + ".bic";

	return new Aurora::GFF3File(new Common::ReadFile(pcFile), kBICID);
}

void Creature::getClassString(const std::vector<Class> &classes, Common::UString &str) {
	for (std::vector<Class>::const_iterator c = classes.begin(); c != classes.end(); ++c) {
		if (!str.empty())
			str += '/';

		uint32_t strRef = TwoDAReg.get2DA("classes").getRow(c->classID).getInt("Name");

		str += TalkMan.getString(strRef);
	}
}

} // End of namespace NWN

} // End of namespace Engines

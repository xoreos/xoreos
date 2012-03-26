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

/** @file engines/nwn/creature.cpp
 *  NWN creature.
 */

#include "common/util.h"
#include "common/maths.h"
#include "common/file.h"
#include "common/configman.h"

#include "aurora/types.h"
#include "aurora/talkman.h"
#include "aurora/resman.h"
#include "aurora/gfffile.h"
#include "aurora/2dafile.h"
#include "aurora/2dareg.h"

#include "graphics/aurora/model.h"
#include "graphics/aurora/modelnode.h"
#include "graphics/aurora/pltfile.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/model.h"

#include "engines/nwn/creature.h"
#include "engines/nwn/item.h"

#include "engines/nwn/gui/widgets/tooltip.h"

static const uint32 kBICID = MKID_BE('BIC ');

namespace Engines {

namespace NWN {

Creature::Associate::Associate(AssociateType t, Creature *a) : type(t), associate(a) {
}


Creature::BodyPart::BodyPart() : id(Aurora::kFieldIDInvalid) {
}

Creature::Creature(const Aurora::GFFStruct &creature) : Object(kObjectTypeCreature) {
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

	delete _model;
	delete _tooltip;
}

void Creature::init() {
	_lastChangedGUIDisplay = 0;

	_gender = kGenderNone;
	_race   = kRaceInvalid;

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

	_colorSkin    = Aurora::kFieldIDInvalid;
	_colorHair    = Aurora::kFieldIDInvalid;
	_colorTattoo1 = Aurora::kFieldIDInvalid;
	_colorTattoo2 = Aurora::kFieldIDInvalid;

	_master = 0;

	_isCommandable = true;

	_model   = 0;
	_tooltip = 0;

	for (int i = 0; i < kAbilityMAX; i++)
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

	delete _tooltip;
	_tooltip = 0;

	if (_model)
		_model->hide();
}

void Creature::setPosition(float x, float y, float z) {
	Object::setPosition(x, y, z);
	Object::getPosition(x, y, z);

	if (_model)
		_model->setPosition(x, y, z);
}

void Creature::setOrientation(float x, float y, float z) {
	Object::setOrientation(x, y, z);
	Object::getOrientation(x, y, z);

	if (_model)
		_model->setRotation(x, z, -y);
}

uint32 Creature::lastChangedGUIDisplay() const {
	return _lastChangedGUIDisplay;
}

const Common::UString &Creature::getFirstName() const {
	return _firstName;
}

const Common::UString &Creature::getLastName() const {
	return _lastName;
}

uint32 Creature::getGender() const {
	return _gender;
}

bool Creature::isFemale() const {
	// Male and female are hardcoded.  Other genders (none, both, other)
	// count as male when it comes to tokens in text strings.

	return _gender == Aurora::kGenderFemale;
}

uint32 Creature::getRace() const {
	return _race;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isDM() const {
	return _isDM;
}

uint32 Creature::getAge() const {
	return _age;
}

uint32 Creature::getXP() const {
	return _xp;
}

int32 Creature::getCurrentHP() const {
	return _currentHP + _bonusHP;
}

int32 Creature::getMaxHP() const {
	return _baseHP + _bonusHP;
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

Creature *Creature::getAssociate(AssociateType type, int nth) const {
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

void Creature::constructModelName(const Common::UString &type, uint32 id,
                                  const Common::UString &gender,
                                  const Common::UString &race,
                                  const Common::UString &phenoType,
                                  const Common::UString &phenoTypeAlt,
                                  Common::UString &model) {

	model = Common::UString::sprintf("p%s%s%s_%s%03d",
	        gender.c_str(), race.c_str(), phenoType.c_str(), type.c_str(), id);

	if (ResMan.hasResource(model, Aurora::kFileTypeMDL))
		return;

	model = Common::UString::sprintf("p%s%s%s_%s%03d",
	        gender.c_str(), race.c_str(), phenoTypeAlt.c_str(), type.c_str(), id);

	if (!ResMan.hasResource(model, Aurora::kFileTypeMDL))
		model.clear();
}

// Based on filenames in model2.bif
// These should be read from MDLNAME, NODENAME in capart.2da (in 2da.bif)
static const char *kBodyPartModels[] = {
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
static const char *kBodyPartNodes[] = {
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
	const Aurora::TwoDAFile &appearance = TwoDAReg.get("appearance");

	const Aurora::TwoDARow &gender = TwoDAReg.get("gender").getRow(_gender);
	const Aurora::TwoDARow &race   = TwoDAReg.get("racialtypes").getRow(_race);
	const Aurora::TwoDARow &raceAp = appearance.getRow(race.getInt("Appearance"));
	const Aurora::TwoDARow &pheno  = TwoDAReg.get("phenotype").getRow(_phenotype);

	Common::UString genderChar   = gender.getString("GENDER");
	Common::UString raceChar     = raceAp.getString("RACE");
	Common::UString phenoChar    = Common::UString("%d", _phenotype);
	Common::UString phenoAltChar = pheno.getString("DefaultPhenoType");

	// Important to capture the supermodel
	_partsSuperModelName = Common::UString::sprintf("p%s%s%s",
	                       genderChar.c_str(), raceChar.c_str(), phenoChar.c_str());

	// Fall back to the default phenotype if required
	if (!ResMan.hasResource(_partsSuperModelName, Aurora::kFileTypeMDL))
		_partsSuperModelName = Common::UString::sprintf("p%s%s%s",
		                       genderChar.c_str(), raceChar.c_str(), phenoAltChar.c_str());

	for (uint i = 0; i < kBodyPartMAX; i++)
		constructModelName(kBodyPartModels[i], _bodyParts[i].id,
		                   genderChar, raceChar, phenoChar, phenoAltChar,
		                   _bodyParts[i].modelName);
}

void Creature::finishPLTs(std::list<Graphics::Aurora::PLTHandle> &plts) {
	for (std::list<Graphics::Aurora::PLTHandle>::iterator p = plts.begin();
	     p != plts.end(); ++p) {

		Graphics::Aurora::PLTFile &plt = p->getPLT();

		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerSkin   , _colorSkin);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerHair   , _colorHair);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo1, _colorTattoo1);
		plt.setLayerColor(Graphics::Aurora::PLTFile::kLayerTattoo2, _colorTattoo2);

		plt.rebuild();
	}
}

void Creature::loadModel() {
	if (_model)
		return;

	if (_appearanceID == Aurora::kFieldIDInvalid) {
		warning("Creature \"%s\" has no appearance", _tag.c_str());
		return;
	}

	const Aurora::TwoDARow &appearance = TwoDAReg.get("appearance").getRow(_appearanceID);

	if (_portrait.empty())
		_portrait = appearance.getString("PORTRAIT");

	if (appearance.getString("MODELTYPE") == "P") {
		getPartModels();
		_model = loadModelObject(_partsSuperModelName);

		//lookup armour models from equipped items
		//set bodyParts[i].armorID
		//in getPartsModels use armorid if non-zero
		//else use id
		//getArmorModels();

		for (uint i = 0; i < kBodyPartMAX; i++) {
			if (_bodyParts[i].modelName.empty())
				continue;

			TextureMan.clearNewPLTs();

			// Try to load in the corresponding part model
			Graphics::Aurora::Model *part_model = loadModelObject(_bodyParts[i].modelName, _bodyParts[i].modelName);
			if (!part_model)
				continue;

			// Add the loaded model to the appropriate part node
            // add in the armour model if any, else body model
			Graphics::Aurora::ModelNode *part_node = _model->getNode(kBodyPartNodes[i]);
			if(part_node)
				part_node->addChild(part_model);

			TextureMan.getNewPLTs(_bodyParts[i].plts);

			finishPLTs(_bodyParts[i].plts);
		}

	} else
		_model = loadModelObject(appearance.getString("RACE"));

	// Positioning

	float x, y, z;

	getPosition(x, y, z);
	setPosition(x, y, z);

	getOrientation(x, y, z);
	setOrientation(x, y, z);

	// Clickable

	if (_model) {
		_model->setTag(_tag);
		_model->setClickable(isClickable());

		_ids.push_back(_model->getID());
	}
}

void Creature::unloadModel() {
	hide();

	delete _model;
	_model = 0;
}

void Creature::loadCharacter(const Common::UString &bic, bool local) {
	Aurora::GFFFile *gff = openPC(bic, local);

	try {
		load(gff->getTopLevel(), 0);
	} catch (...) {
		delete gff;
		throw;
	}

	// All BICs should be PCs.
	_isPC = true;

	// Set the PC tag to something recognizable for now.
	// Let's hope no script depends on it being "".

	_tag = Common::UString::sprintf("[PC: %s]", _name.c_str());

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

void Creature::load(const Aurora::GFFStruct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Aurora::GFFFile *utc = 0;
	if (!temp.empty()) {
		try {
			utc = new Aurora::GFFFile(temp, Aurora::kFileTypeUTC, MKID_BE('UTC '));
		} catch (...) {
			delete utc;
		}
	}

	load(creature, utc ? &utc->getTopLevel() : 0);

	delete utc;

	_lastChangedGUIDisplay = EventMan.getTimestamp();
}

void Creature::load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint) {
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

	float o[3];
	Common::vector2orientation(bearingX, bearingY, o[0], o[1], o[2]);

	setOrientation(o[0], o[1], o[2]);
}

static const char *kBodyPartFields[] = {
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

void Creature::loadProperties(const Aurora::GFFStruct &gff) {
	// Tag

	_tag = gff.getString("Tag", _tag);

	// Name

	if (gff.hasField("FirstName")) {
		Aurora::LocString firstName;
		gff.getLocString("FirstName", firstName);

		_firstName = firstName.getString();
	}

	if (gff.hasField("LastName")) {
		Aurora::LocString lastName;
		gff.getLocString("LastName", lastName);

		_lastName = lastName.getString();
	}

	_name = _firstName + " " + _lastName;
	_name.trim();

	// Description

	if (gff.hasField("Description")) {
		Aurora::LocString description;
		gff.getLocString("Description", description);

		_description = description.getString();
	}

	// Conversation

	_conversation = gff.getString("Conversation", _conversation);

	// Sound Set

	_soundSet = gff.getUint("SoundSetFile", Aurora::kFieldIDInvalid);

	// Portrait

	loadPortrait(gff, _portrait);

	// Gender
	_gender = gff.getUint("Gender", _gender);

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

	// Skills
	if (gff.hasField("SkillList")) {
		_skills.clear();

		const Aurora::GFFList &skills = gff.getList("SkillList");
		for (Aurora::GFFList::const_iterator s = skills.begin(); s != skills.end(); ++s) {
			const Aurora::GFFStruct &skill = **s;

			_skills.push_back(skill.getSint("Rank"));
		}
	}

	// Feats
	if (gff.hasField("FeatList")) {
		_feats.clear();

		const Aurora::GFFList &feats = gff.getList("FeatList");
		for (Aurora::GFFList::const_iterator f = feats.begin(); f != feats.end(); ++f) {
			const Aurora::GFFStruct &feat = **f;

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
	for (uint i = 0; i < kBodyPartMAX; i++)
		_bodyParts[i].id = gff.getUint(kBodyPartFields[i], _bodyParts[i].id);

	// Colors
	_colorSkin    = gff.getUint("Color_Skin", _colorSkin);
	_colorHair    = gff.getUint("Color_Hair", _colorHair);
	_colorTattoo1 = gff.getUint("Color_Tattoo1", _colorTattoo1);
	_colorTattoo2 = gff.getUint("Color_Tattoo2", _colorTattoo2);

	//Equipped Items
	loadEquippedItems(gff);

	// Scripts
	readScripts(gff);
}

void Creature::loadPortrait(const Aurora::GFFStruct &gff, Common::UString &portrait) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get("portraits");

		Common::UString portrait2DA = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait2DA.empty())
			portrait = "po_" + portrait2DA;
	}

	portrait = gff.getString("Portrait", portrait);
}

void Creature::loadEquippedItems(const Aurora::GFFStruct &gff) {
	if(!gff.hasField("Equip_ItemList"))
		return;

	_equippedItems.clear();
	const Aurora::GFFList &cEquipped = gff.getList("Equip_ItemList");
	for (Aurora::GFFList::const_iterator e = cEquipped.begin(); e != cEquipped.end(); ++e) {
		const Aurora::GFFStruct &cItem = **e;
		Common::UString itemref = cItem.getString("EquippedRes");

		Aurora::GFFFile *uti = 0;
		if (!itemref.empty()) {
			try {
				uti = new Aurora::GFFFile(itemref, Aurora::kFileTypeUTI, MKID_BE('UTI '));
			} catch (...) {
				delete uti;
			}
		}

		if(uti) {
			Item* item = new Item();
			item->load(uti->getTopLevel());

			//add it to the equipped list
			_equippedItems.push_back(*item);

			delete uti;
		}
    }
}

void Creature::loadClasses(const Aurora::GFFStruct &gff,
                           std::vector<Class> &classes, uint8 &hitDice) {

	if (!gff.hasField("ClassList"))
		return;

	classes.clear();
	hitDice = 0;

	const Aurora::GFFList &cClasses = gff.getList("ClassList");
	for (Aurora::GFFList::const_iterator c = cClasses.begin(); c != cClasses.end(); ++c) {
		classes.push_back(Class());

		const Aurora::GFFStruct &cClass = **c;

		classes.back().classID = cClass.getUint("Class");
		classes.back().level   = cClass.getUint("ClassLevel");

		hitDice += classes.back().level;
	}
}

const Common::UString &Creature::getConvRace() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("ConverName");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvrace() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("ConverNameLower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvRaces() const {
	const uint32 strRef = TwoDAReg.get("racialtypes").getRow(_race).getInt("NamePlural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getSubRace() const {
	return _subRace;
}

void Creature::getClass(uint32 position, uint32 &classID, uint16 &level) const {
	if (position >= _classes.size()) {
		classID = kClassInvalid;
		level   = 0;
		return;
	}

	classID = _classes[position].classID;
	level   = _classes[position].level;
}

uint16 Creature::getClassLevel(uint32 classID) const {
	for (std::vector<Class>::const_iterator c = _classes.begin(); c != _classes.end(); ++c)
		if (c->classID == classID)
			return c->level;

	return 0;
}

const Common::UString &Creature::getConvClass() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Name");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvclass() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Lower");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getConvClasses() const {
	const uint32 classID = _classes.front().classID;
	const uint32 strRef  = TwoDAReg.get("classes").getRow(classID).getInt("Plural");

	return TalkMan.getString(strRef);
}

const Common::UString &Creature::getDeity() const {
	return _deity;
}

uint8 Creature::getGoodEvil() const {
	return _goodEvil;
}

uint8 Creature::getLawChaos() const {
	return _lawChaos;
}

Common::UString Creature::getClassString() const {
	Common::UString classString;

	getClassString(_classes, classString);

	return classString;
}

uint8 Creature::getHitDice() const {
	return _hitDice;
}

uint8 Creature::getAbility(Ability ability) const {
	assert((ability >= 0) && (ability < kAbilityMAX));

	return _abilities[ability];
}

int8 Creature::getSkillRank(uint32 skill) const {
	if (skill >= _skills.size())
		return -1;

	return _skills[skill];
}

bool Creature::hasFeat(uint32 feat) const {
	for (std::vector<uint32>::const_iterator f = _feats.begin(); f != _feats.end(); ++f)
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
		showTooltip();
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

void Creature::createTooltip() {
	if (_tooltip)
		return;

	_tooltip = new Tooltip(Tooltip::kTypeFeedback, *_model);

	_tooltip->setAlign(0.5);
	_tooltip->addLine(_name, 0.5, 0.5, 1.0, 1.0);
	_tooltip->setPortrait(_portrait);
}

void Creature::showTooltip() {
	createTooltip();
	_tooltip->show();
}

void Creature::hideTooltip() {
	if (!_tooltip)
		return;

	_tooltip->hide();
}

void Creature::getPCListInfo(const Common::UString &bic, bool local,
                             Common::UString &name, Common::UString &classes,
                             Common::UString &portrait) {

	Aurora::GFFFile *gff = openPC(bic, local);

	try {
		const Aurora::GFFStruct &top = gff->getTopLevel();

		// Reading name
		Aurora::LocString firstName;
		top.getLocString("FirstName", firstName);

		Aurora::LocString lastName;
		top.getLocString("LastName", lastName);

		name = firstName.getString() + " " + lastName.getString();

		// Reading portrait (failure non-fatal)
		try {
			loadPortrait(top, portrait);
		} catch (Common::Exception &e) {
			portrait.clear();

			e.add("Can't read portrait for PC \"%s\"", bic.c_str());
			Common::printException(e, "WARNING: ");
		}

		// Reading classes
		std::vector<Class> classLevels;
		uint8 hitDice;

		loadClasses(top, classLevels, hitDice);
		getClassString(classLevels, classes);

		classes = "(" + classes + ")";

	} catch (...) {
		delete gff;
		throw;
	}

	delete gff;
}

Aurora::GFFFile *Creature::openPC(const Common::UString &bic, bool local) {
	Common::UString pcDir  = ConfigMan.getString(local ? "NWN_localPCDir" : "NWN_serverPCDir");
	Common::UString pcFile = pcDir + "/" + bic + ".bic";

	Common::File *pc = 0;
	try {
		pc = new Common::File(pcFile);
	} catch (...) {
		delete pc;
		throw;
	}

	Aurora::GFFFile *gff = 0;
	try {
		gff = new Aurora::GFFFile(pc, kBICID);
	} catch (...) {
		delete gff;
		throw;
	}

	return gff;
}

void Creature::getClassString(const std::vector<Class> &classes, Common::UString &str) {
	for (std::vector<Class>::const_iterator c = classes.begin(); c != classes.end(); ++c) {
		if (!str.empty())
			str += '/';

		uint32 strRef = TwoDAReg.get("classes").getRow(c->classID).getInt("Name");

		str += TalkMan.getString(strRef);
	}
}

} // End of namespace NWN

} // End of namespace Engines

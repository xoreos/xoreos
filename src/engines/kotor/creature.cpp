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
 *  A creature in a Star Wars: Knights of the Old Republic area.
 */

#include "src/common/util.h"
#include "src/common/maths.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/strutil.h"

#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/animationchannel.h"
#include "src/graphics/aurora/cursorman.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/model.h"

#include "src/engines/kotor/creature.h"
#include "src/engines/kotor/item.h"

#include "src/engines/kotor/gui/chargen/chargeninfo.h"

namespace Engines {

namespace KotOR {

Creature::Creature(const Aurora::GFF3Struct &creature)
		: Object(kObjectTypeCreature),
		  _walkRate(0.0f),
		  _runRate(0.0f) {

	init();
	load(creature);
}

Creature::Creature()
		: Object(kObjectTypeCreature),
		  _walkRate(0.0f),
		  _runRate(0.0f) {

	init();
}

Creature::~Creature() {
}

void Creature::init() {
	_isPC = false;

	_appearance = Aurora::kFieldIDInvalid;

	_gender = kGenderNone;
	_race = kRaceUnknown;
	_subRace = kSubRaceNone;
	_skin = kSkinMAX;
	_face = 0;

	_headModel = 0;
	_visible = false;
}

void Creature::show() {
	if (_visible)
		return;

	_visible = true;

	if (_model)
		_model->show();
}

void Creature::hide() {
	if (!_visible)
		return;

	if (_model)
		_model->hide();

	_visible = false;
}

bool Creature::isVisible() const {
	return _visible;
}

bool Creature::isPC() const {
	return _isPC;
}

bool Creature::isPartyMember() const {
	return _isPC;
}

Gender Creature::getGender() const {
	return _gender;
}

int Creature::getLevel(const Class &c) const {
	for (size_t i = 0; i < _levels.size(); ++i) {
		if (_levels[i].characterClass == c)
			return _levels[i].level;
	}

	return 0;
}

int Creature::getLevelByPosition(int position) const {
	if (_levels.size() >= static_cast<unsigned int>(position + 1))
		return _levels[position].level;

	return 0;
}

Class Creature::getClassByPosition(int position) const {
	if (_levels.size() >= static_cast<unsigned int>(position + 1))
		return _levels[position].characterClass;

	return kClassInvalid;
}

Race Creature::getRace() const {
	return _race;
}

SubRace Creature::getSubRace() const {
	return _subRace;
}

float Creature::getWalkRate() const {
	return _walkRate;
}

float Creature::getRunRate() const {
	return _runRate;
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

void Creature::load(const Aurora::GFF3Struct &creature) {
	Common::UString temp = creature.getString("TemplateResRef");

	Common::ScopedPtr<Aurora::GFF3File> utc;
	if (!temp.empty())
		utc.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' ')));

	load(creature, utc ? &utc->getTopLevel() : 0);

	if (!utc)
		warning("Creature \"%s\" has no blueprint", _tag.c_str());
}

void Creature::load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint) {
	// General properties

	if (blueprint)
		loadProperties(*blueprint); // Blueprint
	loadProperties(instance);    // Instance


	// Appearance

	if (_appearance == Aurora::kFieldIDInvalid)
		throw Common::Exception("Creature without an appearance");

	loadAppearance();

	// Position

	setPosition(instance.getDouble("XPosition"),
	            instance.getDouble("YPosition"),
	            instance.getDouble("ZPosition"));

	// Orientation

	float bearingX = instance.getDouble("XOrientation");
	float bearingY = instance.getDouble("YOrientation");

	setOrientation(0.0f, 0.0f, 1.0f, -Common::rad2deg(atan2(bearingX, bearingY)));
}

void Creature::loadProperties(const Aurora::GFF3Struct &gff) {
	// Tag
	_tag = gff.getString("Tag", _tag);

	// Name
	_name = gff.getString("LocName", _name);

	// Description
	_description = gff.getString("Description", _description);

	// Portrait
	loadPortrait(gff);

	// Appearance
	_appearance = gff.getUint("Appearance_Type", _appearance);

	// Static
	_static = gff.getBool("Static", _static);

	// Usable
	_usable = gff.getBool("Useable", _usable);

	// PC
	_isPC = gff.getBool("IsPC", _isPC);

	// Gender
	_gender = Gender(gff.getUint("Gender"));

	// Race
	_race = Race(gff.getSint("Race", _race));
	_subRace = SubRace(gff.getSint("SubraceIndex", _subRace));

	// Hit Points
	_currentHitPoints = gff.getSint("CurrentHitPoints", _maxHitPoints);
	_maxHitPoints = gff.getSint("MaxHitPoints", _currentHitPoints);

	_minOneHitPoint = gff.getBool("Min1HP", _minOneHitPoint);

	// Class Levels
	if (gff.hasField("ClassList")) {
		Aurora::GFF3List classList = gff.getList("ClassList");
		for (Aurora::GFF3List::const_iterator iter = classList.begin(); iter != classList.end(); iter++) {
			const Aurora::GFF3Struct &charClass = **iter;

			ClassLevel classLevel;
			classLevel.characterClass = Class(charClass.getSint("Class"));
			classLevel.level = charClass.getSint("ClassLevel");

			_levels.push_back(classLevel);
		}
	}

	// Scripts
	readScripts(gff);

	_conversation = gff.getString("Conversation", _conversation);
}

void Creature::loadPortrait(const Aurora::GFF3Struct &gff) {
	uint32 portraitID = gff.getUint("PortraitId");
	if (portraitID != 0) {
		const Aurora::TwoDAFile &twoda = TwoDAReg.get2DA("portraits");

		Common::UString portrait = twoda.getRow(portraitID).getString("BaseResRef");
		if (!portrait.empty())
			_portrait = "po_" + portrait;
	}

	_portrait = gff.getString("Portrait", _portrait);
}

void Creature::loadAppearance() {
	PartModels parts;

	getPartModels(parts);

	if ((_modelType == "P") || parts.body.empty()) {
		warning("TODO: Model \"%s\": ModelType \"%s\" (\"%s\")",
		        _tag.c_str(), _modelType.c_str(), parts.body.c_str());
		return;
	}

	loadBody(parts);
	loadHead(parts);

	setDefaultAnimations();
}

void Creature::getPartModels(PartModels &parts, uint32 state) {
	const Aurora::TwoDARow &appearance = TwoDAReg.get2DA("appearance").getRow(_appearance);

	_modelType = appearance.getString("modeltype");

	// TODO: load state based on character equipment
	if (appearance.getString("label").beginsWith("Party_"))
		state = 'b';

	if (_modelType == "B") {
		parts.body = appearance.getString(Common::UString("model") + state);
		parts.bodyTexture = appearance.getString(Common::UString("tex") + state) + "01";
	} else {
		parts.body = appearance.getString("race");
		parts.bodyTexture = appearance.getString("racetex");
	}

	if ((_modelType == "B") || (_modelType == "P")) {
		const int headNormalID = appearance.getInt("normalhead");
		const int headBackupID = appearance.getInt("backuphead");

		const Aurora::TwoDAFile &heads = TwoDAReg.get2DA("heads");

		if      (headNormalID >= 0)
			parts.head = heads.getRow(headNormalID).getString("head");
		else if (headBackupID >= 0)
			parts.head = heads.getRow(headBackupID).getString("head");
	}

	loadMovementRate(appearance.getString("moverate"));
}

void Creature::getPartModelsPC(PartModels &parts, uint32 state, uint8 textureVariation) {
	parts.body = "p";
	parts.head = "p";
	parts.bodyTexture = "p";

	switch (_gender) {
		case kGenderMale:
			parts.body += "m";
			parts.head += "m";
			parts.bodyTexture += "m";
			break;
		case kGenderFemale:
			parts.body += "f";
			parts.head += "f";
			parts.bodyTexture += "f";
			break;
		default:
			throw Common::Exception("Unknown gender");
	}

	parts.body += Common::UString("b") + state;
	parts.head += "h";
	parts.bodyTexture  += Common::UString("b") + state;

	switch (state) {
		case 'a':
		case 'b':
			switch (_levels.back().characterClass) {
				case kClassSoldier:
					parts.body += "l";
					parts.bodyTexture += "l";
					break;
				case kClassScout:
					parts.body += "m";
					parts.bodyTexture += "m";
					break;
				default:
				case kClassScoundrel:
					parts.body += "s";
					parts.bodyTexture += "s";
					break;
			}
			if (state == 'a') {
				switch (_skin) {
					case kSkinA:
						parts.bodyTexture += "A";
						break;
					case kSkinB:
						parts.bodyTexture += "B";
						break;
					default:
						break;
				}
			}
			break;
		default:
			switch (_gender) {
				case kGenderMale:
					parts.body += "l";
					break;
				case kGenderFemale:
					parts.body += "s";
					break;
				default:
					return;
			}
			break;
	}

	switch (_skin) {
		case kSkinA:
			parts.head += "a";
			break;
		case kSkinB:
			parts.head += "b";
			break;
		default:
		case kSkinC:
			parts.head += "c";
			break;
	}

	parts.portrait = "po_" + parts.head;
	parts.portrait += Common::composeString(_face + 1);

	parts.head += "0";
	parts.head += Common::composeString(_face + 1);

	parts.bodyTexture += Common::UString::format("%02u", textureVariation);

	loadMovementRate("PLAYER");
}

void Creature::loadBody(PartModels &parts) {
	// Model "P_BastilaBB" has broken animations. Replace it with the
	// correct one.
	if (parts.body.stricmp("P_BastilaBB") == 0)
		parts.body = "P_BastilaBB02";

	_model.reset(loadModelObject(parts.body, parts.bodyTexture));
	if (!_model)
		return;

	_ids.clear();
	_ids.push_back(_model->getID());

	_model->setTag(_tag);
	_model->setClickable(isClickable());

	if (_modelType != "B" && _modelType != "P")
		_model->addAnimationChannel(Graphics::Aurora::kAnimationChannelHead);
}

void Creature::loadHead(PartModels &parts) {
	if (!_model || parts.head.empty())
		return;

	_headModel = loadModelObject(parts.head);
	if (!_headModel)
		return;

	_model->attachModel("headhook", _headModel);
}

void Creature::loadMovementRate(const Common::UString &name) {
	const Aurora::TwoDARow &speed = TwoDAReg.get2DA("creaturespeed").getRow("2daname", name);

	_walkRate = speed.getFloat("walkrate");
	_runRate = speed.getFloat("runrate");
}

void Creature::changeBody() {
	uint32 state = 'a';
	uint8 textureVariation = 1;

	Common::PtrMap<EquipmentSlot, Item>::const_iterator i = _equipment.find(kEquipmentSlotBody);
	if (i != _equipment.end()) {
		Item *item = i->second;
		state += item->getBodyVariation() - 1;
		textureVariation = item->getTextureVariation();
	}

	PartModels parts;
	getPartModelsPC(parts, state, textureVariation);

	loadBody(parts);
	loadHead(parts);

	if (!_model)
		return;

	changeWeapon(kEquipmentSlotWeaponL);
	changeWeapon(kEquipmentSlotWeaponR);

	setDefaultAnimations();

	if (_visible) {
		float x, y, z;
		getPosition(x, y, z);
		_model->setPosition(x, y, z);

		float angle;
		getOrientation(x, y, z, angle);
		_model->setOrientation(x, y, z, angle);

		_model->show();
	}
}

void Creature::changeWeapon(EquipmentSlot slot) {
	assert((slot == kEquipmentSlotWeaponL) || (slot == kEquipmentSlotWeaponR));

	Graphics::Aurora::Model *weaponModel = 0;

	Common::PtrMap<EquipmentSlot, Item>::const_iterator i = _equipment.find(slot);
	if (i != _equipment.end()) {
		Item *item = i->second;
		weaponModel = loadModelObject(item->getModelName());
	}

	Common::UString hookNode;

	switch (slot) {
		case kEquipmentSlotWeaponL:
			hookNode = "lhand";
			break;
		case kEquipmentSlotWeaponR:
			hookNode = "rhand";
			break;
		default:
			throw Common::Exception("Unsupported equip slot");
	}

	_model->attachModel(hookNode, weaponModel);
}

void Creature::createFakePC() {
	_name = "Fakoo McFakeston";
	_tag  = Common::UString::format("[PC: %s]", _name.c_str());

	_isPC = true;
}

void Creature::createPC(const CharacterGenerationInfo &info) {
	_name = info.getName();
	_isPC = true;

	_race = kRaceHuman;
	_subRace = kSubRaceNone;

	_gender = info.getGender();

	switch (_gender) {
		case kGenderMale:
		case kGenderFemale:
			break;
		default:
			throw Common::Exception("Unknown gender");
	}

	// set the specific class to level 1
	_levels.resize(1);
	_levels[0].level = 1;
	_levels[0].characterClass = info.getClass();

	_skin = info.getSkin();
	_face = info.getFace();

	PartModels parts;
	getPartModelsPC(parts, 'a', 1);

	_portrait = parts.portrait;

	loadBody(parts);
	loadHead(parts);

	setDefaultAnimations();
}

void Creature::enter() {
	CursorMan.setGroup("talk");
	highlight(true);
}

void Creature::leave() {
	CursorMan.set();
	highlight(false);
}

void Creature::highlight(bool enabled) {
	_model->drawBound(enabled);
}

bool Creature::click(Object *triggerer) {
	// Try the onDialog script first
	if (hasScript(kScriptDialogue))
		return runScript(kScriptDialogue, this, triggerer);

	// Next, look we have a generic onClick script
	if (hasScript(kScriptClick))
		return runScript(kScriptClick, this, triggerer);

	return false;
}

const Common::UString &Creature::getConversation() const {
	return _conversation;
}

float Creature::getCameraHeight() const {
	float height = 1.8f;
	if (_model) {
		Graphics::Aurora::ModelNode *node = _model->getNode("camerahook");
		if (node) {
			float x, y, z;
			node->getPosition(x, y, z);
			height = z;
		}
	}
	return height;
}

void Creature::equipItem(Common::UString tag, EquipmentSlot slot) {
	Common::PtrMap<EquipmentSlot, Item>::iterator i = _equipment.find(slot);
	if (i != _equipment.end()) {
		_inventory.addItem(i->second->getTag());
		_equipment.erase(i);
	}
	if (!tag.empty()) {
		_inventory.removeItem(tag);
		try {
			Item *item = new Item(tag);
			_equipment.insert(std::pair<EquipmentSlot, Item *>(slot, item));
		} catch (Common::Exception &e) {
			e.add("Failed to load item: %s", tag.c_str());
			Common::printException(e, "WARNING: ");
		}
	}
	switch (slot) {
		case kEquipmentSlotBody:
			changeBody();
			break;
		case kEquipmentSlotWeaponL:
		case kEquipmentSlotWeaponR:
			changeWeapon(slot);
			break;
		default:
			break;
	}
}

Inventory &Creature::getInventory() {
	return _inventory;
}

Item *Creature::getEquipedItem(EquipmentSlot slot) const {
	Common::PtrMap<EquipmentSlot, Item>::const_iterator i = _equipment.find(slot);
	return i == _equipment.end() ? 0 : i->second;
}

void Creature::playDefaultAnimation() {
	if (_model)
		_model->playDefaultAnimation();
}

void Creature::playDefaultHeadAnimation() {
	if (!_model)
		return;

	Graphics::Aurora::AnimationChannel *headChannel = 0;

	if (_modelType == "B" || _modelType == "P") {
		Graphics::Aurora::Model *head = _model->getAttachedModel("headhook");
		if (head)
			headChannel = head->getAnimationChannel(Graphics::Aurora::kAnimationChannelAll);
	} else
		headChannel = _model->getAnimationChannel(Graphics::Aurora::kAnimationChannelHead);

	if (headChannel)
		headChannel->playDefaultAnimation();
}

void Creature::playAnimation(const Common::UString &anim, bool restart, float length, float speed) {
	if (_model)
		_model->playAnimation(anim, restart, length, speed);
}

void Creature::playHeadAnimation(const Common::UString &anim, bool restart, float length, float speed) {
	if (!_model)
		return;

	Graphics::Aurora::AnimationChannel *headChannel = 0;

	if (_modelType == "B" || _modelType == "P") {
		Graphics::Aurora::Model *head = _model->getAttachedModel("headhook");
		if (head)
			headChannel = head->getAnimationChannel(Graphics::Aurora::kAnimationChannelAll);
	} else
		headChannel = _model->getAnimationChannel(Graphics::Aurora::kAnimationChannelHead);

	if (headChannel)
		headChannel->playAnimation(anim, restart, length, speed);
}

void Creature::clearActionQueue() {
	_actionQueue.clear();
}

void Creature::enqueueAction(const Action &action) {
	_actionQueue.push_back(action);
}

const Action *Creature::peekAction() const {
	if (_actionQueue.empty())
		return 0;

	return &_actionQueue.front();
}

const Action *Creature::dequeueAction() {
	Action *action = _actionQueue.empty() ? 0 : &_actionQueue.front();
	if (action)
		_actionQueue.erase(_actionQueue.begin());
	return action;
}

void Creature::setDefaultAnimations() {
	if (!_model)
		return;

	if (_modelType == "S" || _modelType == "L")
		_model->addDefaultAnimation("cpause1", 100);
	else
		_model->addDefaultAnimation("pause1", 100);
}

} // End of namespace KotOR

} // End of namespace Engines

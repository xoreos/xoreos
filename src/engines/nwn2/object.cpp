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
 *  An object in a Neverwinter Nights 2 area.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/uuid.h"

#include "src/aurora/ssffile.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/dlgfile.h"

#include "src/aurora/nwscript/types.h"
#include "src/aurora/nwscript/util.h"
#include "src/aurora/nwscript/functioncontext.h"
#include "src/aurora/nwscript/functionman.h"
#include "src/aurora/nwscript/objectman.h"

#include "src/sound/sound.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/tokenman.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/object.h"
#include "src/engines/nwn2/area.h"
#include "src/engines/nwn2/module.h"
#include "src/engines/nwn2/item.h"
#include "src/engines/nwn2/faction.h"

static const uint8_t kRepEnemyMax  = 10; // Maximum reputation for an enemy
static const uint8_t kRepFriendMin = 90; // Minimum reputation for a friend

namespace Engines {

namespace NWN2 {

Object::Object(ObjectType type) : _type(type), _faction(2),
	_soundSet(Aurora::kFieldIDInvalid), _static(true), _usable(true),
	_listen(false), _container(false), _area(0) {
	_id = Common::generateIDNumber();
	ObjectMan.registerObject(this);

	_position   [0] = 0.0f;
	_position   [1] = 0.0f;
	_position   [2] = 0.0f;
	_orientation[0] = 0.0f;
	_orientation[1] = 0.0f;
	_orientation[2] = 0.0f;
	_orientation[3] = 0.0f;
}

Object::~Object() {
	ObjectMan.unregisterObject(this);
}

ObjectType Object::getType() const {
	return _type;
}

void Object::loadModel() {
}

void Object::unloadModel() {
}

void Object::show() {
}

void Object::hide() {
}

const Common::UString &Object::getName() const {
	return _name;
}

const Common::UString &Object::getDescription() const {
	return _description;
}

const Common::UString &Object::getConversation() const {
	return _conversation;
}

const Aurora::SSFFile *Object::getSSF() {
	loadSSF();

	return _ssf.get();
}

bool Object::isStatic() const {
	return _static;
}

bool Object::isUsable() const {
	return _usable;
}

bool Object::isClickable() const {
	return !_static && _usable;
}

void Object::setUsable(const bool usable) {
	_usable = usable;
}

bool Object::getHasInventory() const {
	return _container;
}

const std::list<uint32_t> &Object::getIDs() const {
	return _ids;
}

Area *Object::getArea() const {
	return _area;
}

void Object::setArea(Area *area) {
	_area = area;
}

Location Object::getLocation() const {
	// TODO: Object::getLocation(): Facing

	Location location;

	location.setArea(_area);
	location.setPosition(_position[0], _position[1], _position[2]);
	location.setFacing(0.0f);

	return location;
}

void Object::getPosition(float &x, float &y, float &z) const {
	x = _position[0];
	y = _position[1];
	z = _position[2];
}

void Object::getOrientation(float &x, float &y, float &z, float &angle) const {
	x = _orientation[0];
	y = _orientation[1];
	z = _orientation[2];

	angle = _orientation[3];
}

void Object::setPosition(float x, float y, float z) {
	_position[0] = x;
	_position[1] = y;
	_position[2] = z;
}

void Object::setOrientation(float x, float y, float z, float angle) {
	_orientation[0] = x;
	_orientation[1] = y;
	_orientation[2] = z;
	_orientation[3] = angle;
}

Item *Object::createItemOnObject(const Common::UString &UNUSED(blueprint),
	uint16_t UNUSED(stackSize), const Common::UString &UNUSED(tag)) {
	return 0;
}

void Object::enter() {
}

void Object::leave() {
}

void Object::highlight(bool UNUSED(enabled)) {
}

bool Object::click(Object *UNUSED(triggerer)) {
	return true;
}

void Object::loadSSF() {
	if (_ssf || (_soundSet == Aurora::kFieldIDInvalid))
		return;

	const Aurora::TwoDAFile &soundSets = TwoDAReg.get2DA("soundset");

	Common::UString ssfFile = soundSets.getRow(_soundSet).getString("RESREF");
	if (ssfFile.empty())
		return;

	try {
		_ssf = std::make_unique<Aurora::SSFFile>(ssfFile);
	} catch (...) {
		Common::exceptionDispatcherWarning("Failed to load SSF \"%s\" (object \"%s\")",
		                                   ssfFile.c_str(), _tag.c_str());
	}
}

void Object::readVarTable(const Aurora::GFF3List &varTable) {
	for (Aurora::GFF3List::const_iterator v = varTable.begin(); v != varTable.end(); ++v) {
		const Common::UString name  = (*v)->getString ("Name");
		const int32_t           type  = (*v)->getSint   ("Type");

		if (name.empty())
			continue;

		switch (type) {
			case -1:
				setVariable(name, Aurora::NWScript::Variable());
				break;

			case  1:
				setVariable(name, (int32_t) (*v)->getSint("Value"));
				break;

			case  2:
				setVariable(name, (float) (*v)->getDouble("Value"));
				break;

			case  3:
				setVariable(name, (*v)->getString("Value"));
				break;

			case  4:
				setVariable(name, (int32_t)((uint32_t) (*v)->getUint("Value")));
				break;

			case  5:
				warning("TODO: Object::readVarTable(), \"%s\" has location type", name.c_str());
				setVariable(name, Aurora::NWScript::Variable());
				break;

			default:
				throw Common::Exception("Unknown variable type %u (\"%s\")", type, name.c_str());
		}
	}
}

void Object::readVarTable(const Aurora::GFF3Struct &gff) {
	if (gff.hasField("VarTable"))
		readVarTable(gff.getList("VarTable"));
}

void Object::speakString(const Common::UString &string, uint32_t UNUSED(volume)) {
	// TODO: Object::speakString(): Show the string in a speech bubble

	status("<%s> \"%s\"", getName().c_str(), string.c_str());
}

void Object::speakOneLiner(Common::UString conv, Object *UNUSED(tokenTarget)) {
	if (conv.empty())
		conv = _conversation;
	if (conv.empty())
		return;

	Common::UString text;
	Common::UString sound;


	try {
		Aurora::DLGFile dlg(conv, this);

		const Aurora::DLGFile::Line *line = dlg.getOneLiner();
		if (line) {
			text  = line->text.getString();
			sound = line->sound;
		}

	} catch (...) {
		Common::exceptionDispatcherWarning("Failed evaluating one-liner from conversation \"%s\"", conv.c_str());
	}

	if (!text.empty())
		speakString(text, 0);
	if (!sound.empty())
		playSound(sound);
}

void Object::stopSound() {
	SoundMan.stopChannel(_sound);
}

void Object::playSound(const Common::UString &sound, bool pitchVariance) {
	stopSound();
	if (sound.empty())
		return;

	_sound = ::Engines::playSound(sound, Sound::kSoundTypeVoice, false, 1.0f, pitchVariance);
}

/** Return this object's faction. */
uint32_t Object::getFaction() const {
	return _faction;
}

/** Return this object's reputation with the source. */
uint8_t Object::getReputation(Object *source) const {
	assert(source);
	if (getArea())
		return getArea()->getModule().getFactions().getReputation(source, getFaction());
	else
		return 0; // Default to enemy
}

/**
 * Return true if this object's reputation with
 * the source is in the range [0, kRepEnemyMax].
 */

bool Object::getIsEnemy(Object *source) const {
	uint8_t repute = getReputation(source);
	return (repute <= kRepEnemyMax);
}

/**
 * Return true if this object's reputation with
 * the source is in the range [kRepFriendMin, 100].
 */
bool Object::getIsFriend(Object *source) const {
	uint8_t repute = getReputation(source);
	return (repute >= kRepFriendMin);
}

/**
 * Return true if this object's reputation with
 * the source is in the range (kRepEnemyMax, kRepFriendMin).
 */
bool Object::getIsNeutral(Object *source) const {
	uint8_t repute = getReputation(source);
	return ((repute > kRepEnemyMax) && (repute < kRepFriendMin));
}

bool Object::getIsListening() {
	return _listen;
}

void Object::setListening(bool listen) {
	_listen = listen;
}

int32_t Object::getListenPatternNumber(const Common::UString &pattern) {
	auto search = _patterns.find(pattern);
	return (search != _patterns.end()) ? search->second : 0;
}

void Object::setListenPattern(const Common::UString &pattern, int32_t number) {
	_patterns[pattern] = number;
}

} // End of namespace NWN2

} // End of namespace Engines

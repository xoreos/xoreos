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
 *  A trap on a Neverwinter Nights 2 door, placeable, or trigger.
 */

#include "src/aurora/gff3file.h"
#include "src/aurora/2dafile.h"
#include "src/aurora/2dareg.h"

#include "src/engines/nwn2/trap.h"

namespace Engines {

namespace NWN2 {

Trap::Trap(const Aurora::GFF3Struct &trap) {
	init();
	load(trap);
}

Trap::Trap(const uint8 type, const Creature * creator) {
	init();
	load(type, creator);
}

Trap::~Trap() {
}

/**
 * Initialize variables
 */
void Trap::init() {
	_isTrap = false; // Default is not trapped
	_isDetectable = true;
	_isDisarmable = true;
	_isRecoverable = true;
	_isTrapOneShot = true;
	_isTrapActive = false;
	_isFlagged = false;
	_isAutoRemoveKey = false;
	_trapType = 0; // Spike Trap, Minor
	_detectDC = 0;
	_disarmDC = 15;
	_keyTag = "";
	_itemResRef = ""; // Recover trap item
	_detectedBy = 0;
	_createdBy = 0;
}

/**
 * Return true if the trap can be triggered by
 * the creature 'triggerBy'.
 */
bool Trap::isTriggeredBy(Object * triggerBy) const {
	// Check if a trap
	if (!_isTrap)
		return false;

	assert(triggerBy);

	// TODO: Check if faction is friendly

	// Check if active
	return _isTrapActive;
}

/** Return true if trap is active */
bool Trap::getTrapActive() const {
	return _isTrap && _isTrapActive;
}

/** Return true if trap can be detected */
bool Trap::getTrapDetectable() const {
	return _isDetectable;
}

/** Return true if the trap was detected by the creature */
bool Trap::getTrapDetectedBy(Creature *detector) const {
	// Do either exist?
	if ((detector == 0) || (_detectedBy == 0))
		return false;

	// Get the identifiers
	// TODO

	return false;
}

/** Return true of the trap can be disarmed */
bool Trap::getTrapDisarmable() const {
	return _isDisarmable;
}

/** Return true if the trap is visible to all creatures */
bool Trap::getTrapFlagged() const {
	return _isTrap && _isFlagged;
}

/** Return true if the trap only fires one time */
bool Trap::getTrapOneShot() const {
	return _isTrapOneShot;
}

/** Return true if the trap can be recovered as an item */
bool Trap::getTrapRecoverable() const {
	return _isRecoverable;
}

/** Return the base type of the trap */
uint8 Trap::getTrapBaseType() const {
	return _trapType;
}

/** Return the trap detect DC */
uint8 Trap::getTrapDetectDC() const {
	return _detectDC;
}

/** Return the trap disarm DC */
uint8 Trap::getTrapDisarmDC() const {
	return _disarmDC;
}

/** Return the key tag for this trap */
Common::UString Trap::getTrapKeyTag() const {
	return _keyTag;
}

/** Get the trap creator */
Uint32 Trap::getTrapCreator() const {
	return _createdBy;
}

/** Set the trap active state */
void Trap::setTrapActive(bool active) {
	_isTrapActive = active;
	if (!active)
		_isFlagged = false;
}

/** Set the trap detectable state */
void Trap::setTrapDetectable(bool detectable) {
	_isDetectable = detectable;
}

/** Store the creature detecting the trap */
void Trap::setTrapDetectedBy(Creature *detector) {
	assert(detector);
	_detectedBy = detector->getID();
}

/** Set the disarmable state */
void Trap::setTrapDisarmable(bool disarmable) {
	_isDisarmable = disarmable;
}

/** Set the one-shot state */
void Trap::setTrapOneShot(bool oneShot) {
	_isTrapOneShot = oneShot;
}

/** Set the recoverable state */
void Trap::setTrapRecoverable(bool recoverable) {
	_isRecoverable = recoverable;
}

/** Set the trap detect DC value */
void Trap::setTrapDetectDC(uint8 detectDC) {
	_detectDC = detectDC;
}

/** Set the trap disarm DC value */
void Trap::setTrapDisarmDC(uint8 disarmDC) {
	_disarmDC = disarmDC;
}

/** Set the tag of the key that can disarm the trap */
void Trap::setTrapKeyTag(Common::UString keyTag) {
	_keyTag = keyTag;
}

/*
 * The agent is attempting to detect the trap using
 * the search skill and all applicable modifiers.
 * This is equivalent to a GetIsSkillSuccessful
 * call for the Search skill versus the _detectDC.
 * If successful, setTrapDetectedBy is called.
 */
bool Trap::detectTrap(Creature *agent) {
	// Already detected?
	if ((_detectedBy != 0) || _isFlagged)
		return true;

	assert(agent);

	// If DC is over 20, character must be a rogue
	if (_detectDC > 20)
		if (agent->getClassLevel(kCClassRogue) < 1)
			return false;

	// TODO: Check for a non-enemy faction so _isFlagged isn't set?

	// Make the Search skill check vs. trap detect DC
	bool result = agent->getIsSkillSuccessful(kSkillSearch, _detectDC);

	// On a success, set to detected
	if (result)
		setTrapDetectedBy(agent);

	return false;
}

/**
 * The agent is making a Disable Device skill check
 * on the trap. The 'option' parameter specifies the
 * action being performed.
 */
bool Trap::disarmTrap(Creature *agent, Disarm option) {
	const int kTrapExamineDCMod =  -7;
	const int kTrapFlagDCMod    =  -5;
	const int kTrapDisarmDCMod  =   0;
	const int kTrapRecoverDCMod = +10;

	// Already disabled?
	if (!_isTrapActive)
		return true;

	assert(agent);

	// TODO: Check party inventory for _keyTag item

	// Must have at least one rank in disable device
	uint8 ranks = agent->getSkillRank(kSkillDisableDevice);
	if (ranks < 1)
		return false;

	// Compute the DC modifier for the disarm option
	int adjustDC = 0;
	bool checkRogue = false;
	switch (option) {
		case kTrapExamine:
			adjustDC = kTrapExamineDCMod;
			break;
		case kTrapFlag:
			adjustDC = kTrapFlagDCMod;
			break;
		case kTrapDisarm:
			adjustDC = kTrapDisarmDCMod;
			checkRogue = true;
			break;
		case kTrapRecover:
			adjustDC = kTrapRecoverDCMod;
			checkRogue = true;
			break;
	}

	// If DC is over 19, character must be a rogue to disarm
	if (checkRogue && _disarmDC > 19)
		if (agent->getClassLevel(kCClassRogue) < 1)
			return false;

	// Make the Disable Device skill check vs. trap disable DC
	bool result = agent->getIsSkillSuccessful(kSkillDisableDevice, _disarmDC + adjustDC);

	// Check for a disarm attempt
	if (result) {
		switch (option) {
			case kTrapExamine:
				// TODO: Report to player
				break;
			case kTrapFlag:
				_isFlagged = false;
				break;
			case kTrapDisarm:
				setTrapActive(false);
				break;
			case kTrapRecover:
				setTrapActive(false);
				// TODO: add trap item to agent's inventory
				break;
		}
	}

	return result;
}

/**
 * The object has triggered the trap. If
 * _isTrapOneShot is true, deactivate the
 * trap.
 */
void Trap::triggeredTrap() {
	if (_isTrapOneShot) {
		_isTrapActive = false;
		_isFlagged = false;
	}
}

/** Load the trap information from the struct */
void Trap::load(const Aurora::GFF3Struct &gff) {
	_isTrapActive = true; // Default for a trigger trap

	// Load the traps.2da information
	_trapType = gff.getUint("TrapType", _trapType);
	loadTrap2da(TwoDAReg.get2DA("traps"), _trapType);

	_isTrap = gff.getBool("TrapFlag", _isTrap);
	_isDetectable = gff.getBool("TrapDetectable", _isDetectable);
	_isDisarmable = gff.getBool("TrapDisarmable", _isDisarmable);
	_isRecoverable = gff.getBool("TrapRecoverable", _isRecoverable);
	_isTrapOneShot = gff.getBool("TrapOneShot", _isTrapOneShot);
	_isTrapActive = gff.getBool("TrapActive", _isTrapActive);
	_isAutoRemoveKey = gff.getBool("AutoRemoveKey", _isAutoRemoveKey);

	_trapType = gff.getUint("TrapType", _trapType);
	_detectDC = gff.getUint("TrapDetectDC", _detectDC);
	_disarmDC = gff.getUint("DisarmDC", _disarmDC);

	_keyTag = gff.getString("KeyName", _keyTag);
}

/** Load the trap information from a traps.2da row */
void Trap::load(const uint8 type, const Creature * creator) {
	_trapType = type;
	loadTrap2da(TwoDAReg.get2DA("traps"), _trapType);
	_createdBy = creator->getID();
}

/** Load base trap information from the traps.2da file */
void Trap::loadTrap2da(const Aurora::TwoDAFile &twoda, uint32 id) {
	_detectDC = twoda.getRow(id).getInt("DetectDCMod");
	_disarmDC = twoda.getRow(id).getInt("DisarmDCMod");
	_itemResRef = twoda.getRow(id).getString("ResRef");
}

} // End of namespace NWN2

} // End of namespace Engines

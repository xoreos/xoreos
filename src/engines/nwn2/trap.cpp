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

#include <cassert>

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

Trap::Trap(const uint8_t type, const Creature *creator) {
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
bool Trap::isTriggeredBy(Object *triggerBy) const {
	// Check if a trap
	if (!_isTrap)
		return false;

	assert(triggerBy);

	// Check if active
	return _isTrapActive;
}

/** Return true if this object is trapped */
bool Trap::getIsTrapped() const {
	return _isTrap;
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
	if (_detectedBy == detector->getID())
		return true;

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
uint8_t Trap::getTrapBaseType() const {
	return _trapType;
}

/** Return the trap detect DC */
uint8_t Trap::getTrapDetectDC() const {
	return _detectDC;
}

/** Return the trap disarm DC */
uint8_t Trap::getTrapDisarmDC() const {
	return _disarmDC;
}

/** Return the key tag for this trap */
Common::UString Trap::getTrapKeyTag() const {
	return _keyTag;
}

/** Get the trap creator */
uint32_t Trap::getTrapCreator() const {
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

/** Disarm the trap */
void Trap::setTrapDisabled() {
	// TODO: Trigger the trap's OnDisarm event
	_isTrap = false;
	_isFlagged = false;
	_detectedBy = 0;
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
void Trap::setTrapDetectDC(uint8_t detectDC) {
	_detectDC = detectDC;
}

/** Set the trap disarm DC value */
void Trap::setTrapDisarmDC(uint8_t disarmDC) {
	_disarmDC = disarmDC;
}

/** Set the tag of the key that can disarm the trap */
void Trap::setTrapKeyTag(const Common::UString &keyTag) {
	_keyTag = keyTag;
}

/** Create a new trap type using the 'trapType' row data in 'traps.2da' */
void Trap::createTrapBaseType(uint8_t trapType) {
	const Aurora::TwoDAFile &twoDA = TwoDAReg.get2DA("traps");
	const size_t count = twoDA.getRowCount();
	if (trapType >= count)
		return;

	// Load the trap row
	const Aurora::TwoDARow &row = twoDA.getRow(trapType);

	// Set this up as a new trap
	_detectDC = row.getInt("DetectDCMod");
	_disarmDC = row.getInt("DisarmDCMod");
	_itemResRef = row.getString("ResRef");
	_trapType = trapType;
	_isTrap = true;
	_isFlagged = false;
	_detectedBy = 0;
	_createdBy = 0;
}

void Trap::createTrap(uint8_t trapType, uint32_t UNUSED(faction),
                      const Common::UString &UNUSED(disarm),
                      const Common::UString &UNUSED(triggered)) {
	createTrapBaseType(trapType);
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
	uint8_t ranks = agent->getSkillRank(kSkillDisableDevice);
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
	try {
		// Initialize using the 'traps.2da' information
		createTrapBaseType(gff.getUint("TrapType", _trapType));
	} catch (...) {
	}

	_isTrapActive = true; // Default for a trigger trap
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

/** Load the trap information from a 'traps.2da' row */
void Trap::load(const uint8_t type, const Creature *creator) {
	createTrapBaseType(type);
	_createdBy = creator->getID();
}

} // End of namespace NWN2

} // End of namespace Engines

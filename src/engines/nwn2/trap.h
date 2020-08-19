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

#ifndef ENGINES_NWN2_TRAP_H
#define ENGINES_NWN2_TRAP_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/nwn2/types.h"
#include "src/engines/nwn2/creature.h"

namespace Engines {

namespace NWN2 {

class Trap {
public:
	/** The disarm device option */
	enum Disarm {
		kTrapExamine = 1,
		kTrapFlag       ,
		kTrapDisarm     ,
		kTrapRecover    ,
	};

	Trap(const Aurora::GFF3Struct &trap);
	Trap(const uint8_t type, const Creature *creator);
	virtual ~Trap();

	void createTrapBaseType(uint8_t trapType);
	virtual void createTrap(uint8_t trapType, uint32_t faction,
	                        const Common::UString &disarm,
	                        const Common::UString &triggered);

	bool isTriggeredBy(Object *triggerBy) const;

	bool getIsTrapped() const;
	bool getTrapActive() const;
	bool getTrapDetectable() const;
	bool getTrapDetectedBy(Creature *detector) const;
	bool getTrapDisarmable() const;
	bool getTrapFlagged() const;
	bool getTrapOneShot() const;
	bool getTrapRecoverable() const;

	uint8_t getTrapBaseType() const;
	uint8_t getTrapDetectDC() const;
	uint8_t getTrapDisarmDC() const;

	Common::UString getTrapKeyTag() const;

	uint32_t getTrapCreator() const;

	void setTrapActive(bool active);
	void setTrapDetectable(bool detectable = true);
	void setTrapDetectedBy(Creature *detector);
	void setTrapDisarmable(bool disarmable = true);
	void setTrapOneShot(bool oneShot);
	void setTrapRecoverable(bool recoverable);
	void setTrapDisabled();

	void setTrapDetectDC(uint8_t detectDC);
	void setTrapDisarmDC(uint8_t disarmDC);

	void setTrapKeyTag(const Common::UString &keyTag);

	bool detectTrap(Creature *agent);
	bool disarmTrap(Creature *agent, Disarm option);
	void triggeredTrap();

private:
	bool _isTrap;           ///< Is this a trap?
	bool _isDetectable;     ///< Can the trap be detected?
	bool _isDisarmable;     ///< Can the trap be disarmed?
	bool _isRecoverable;    ///< Can the disarmed trap be recovered as an item?
	bool _isTrapOneShot;    ///< Does the trap only fire once?
	bool _isTrapActive;     ///< Is the trap active (placeable or door only)?
	bool _isFlagged;        ///< Is the trap visible to all creatures?
	bool _isAutoRemoveKey;  ///< Automatically remove the key?

	uint8_t _trapType;        ///< Row number in traps.2da.
	uint8_t _detectDC;        ///< DC to detect trap.
	uint8_t _disarmDC;        ///< DC to disarm trap.

	Common::UString _keyTag;     ///< Tag of disabling key
	Common::UString _itemResRef; ///< Recoverable item

	uint32_t _detectedBy;     ///< Creature that detected the trap.
	uint32_t _createdBy;      ///< Creature that created the trap.

	void init();

	void load(const Aurora::GFF3Struct &trap);
	void load(const uint8_t type, const Creature *creator);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_TRAP_H

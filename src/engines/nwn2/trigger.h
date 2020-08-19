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
 *  Trigger in a Neverwinter Nights 2 area.
 */

#ifndef ENGINES_NWN2_TRIGGER_H
#define ENGINES_NWN2_TRIGGER_H

#include "src/engines/aurora/trigger.h"

#include "src/engines/nwn2/object.h"
#include "src/engines/nwn2/trap.h"

namespace Engines {

namespace NWN2 {

class Trigger : public ::Engines::Trigger, public Object, public Trap {
public:
	Trigger(const Aurora::GFF3Struct &gff);

	// .--- Object
	void show();
	void hide();
	void notifyNotSeen();
	bool isVisible() const;
	// '---

	/** Get the reputation of the trigger with the source. */
	uint8_t getReputation(Object *source) const;
	/** Create a trap on the trigger. */
	void createTrap(uint8_t trapType, uint32_t faction,
	                const Common::UString &disarm,
	                const Common::UString &triggered);

protected:
	void load(const Aurora::GFF3Struct &gff);
	void loadBlueprint(const Aurora::GFF3Struct &gff);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_TRIGGER_H

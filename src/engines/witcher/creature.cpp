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
 *  A creature in a The Witcher area.
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/gff3file.h"

#include "src/engines/witcher/creature.h"

namespace Engines {

namespace Witcher {

Creature::Creature(const Common::UString &utc) : Object(kObjectTypeCreature),
	_isPC(false) {

	load(utc);
}

Creature::~Creature() {
}

bool Creature::isPC() const {
	return _isPC;
}

void Creature::load(const Common::UString &utc) {
	Aurora::GFF3File gff(utc, Aurora::kFileTypeUTC, MKTAG('U', 'T', 'C', ' '));

	load(gff.getTopLevel());
}

void Creature::load(const Aurora::GFF3Struct &utc) {
	// Unique ID and tag
	_uniqueID = utc.getString("UniqueID", _uniqueID);
	_tag      = utc.getString("Tag", _tag);

	// Name
	utc.getLocString("FirstName", _name);

	// Description
	utc.getLocString("Description", _description);

	// Static
	_static = false;

	// Usable
	_static = true;

	// PC
	_isPC = utc.getBool("IsPC", _isPC);

	// Scripts
	readScripts(utc);
}

} // End of namespace Witcher

} // End of namespace Engines

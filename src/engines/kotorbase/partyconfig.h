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
 *  Party configuration for KotOR games.
 */

#ifndef ENGINES_KOTORBASE_PARTYCONFIG_H
#define ENGINES_KOTORBASE_PARTYCONFIG_H

#include <vector>

#include "src/common/ustring.h"

namespace Engines {

namespace KotORBase {

struct PartyConfiguration {
	Common::UString slotTemplate[10];
	bool slotSelected[10];
	int forceNPC1;
	int forceNPC2;
	bool canCancel;

	PartyConfiguration();
	PartyConfiguration(const PartyConfiguration &config);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PARTYCONFIG_H

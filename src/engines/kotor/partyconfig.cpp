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
 *  Party configuration for Star Wars: Knights of the Old Republic.
 */

#include "src/engines/kotor/partyconfig.h"

static const int kMaxSlotCount = 10;

namespace Engines {

namespace KotOR {

PartyConfiguration::PartyConfiguration()
		: forceNPC1(-1),
		  forceNPC2(-1),
		  canCancel(true) {
	for (int i = 0; i < kMaxSlotCount; ++i) {
		slotSelected[i] = false;
	}
}

PartyConfiguration::PartyConfiguration(const PartyConfiguration &config)
		: forceNPC1(config.forceNPC1),
		  forceNPC2(config.forceNPC2),
		  canCancel(config.canCancel) {
	for (int i = 0; i < kMaxSlotCount; ++i) {
		slotTemplate[i] = config.slotTemplate[i];
		slotSelected[i] = config.slotSelected[i];
	}
}

} // End of namespace KotOR

} // End of namespace Engines

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
 *  Implementation of the NWN:EE widget name resolver.
 *
 *  Pass-through by default; a future iteration populates the mapping
 *  table from xoreos.profile.json coverage data.
 */

#include "src/engines/nwn/gui/nwnee_resolver.h"

namespace Engines {

namespace NWN {

Common::UString NWNEEWidgetNameResolver::resolve(const Common::UString &tag) const {
	// Pass-through: unknown tags stay as-is. As coverage dumps reveal
	// EE's actual widget tags, this table will grow one entry per
	// confirmed mismatch from xoreos's expectation.
	return tag;
}

} // End of namespace NWN

} // End of namespace Engines

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
 *  Per-game-version widget name resolver.
 *
 *  Some game variants (notably NWN: Enhanced Edition) rename GUI
 *  widget tags relative to the original release. The resolver maps
 *  xoreos's expected name to the variant's actual name; lookup falls
 *  back to the original name when no mapping is registered.
 *
 *  Lives in the top-level Aurora namespace (next to GFF3Struct) so
 *  that headers referring to "Aurora::Foo" don't accidentally resolve
 *  to "Engines::Aurora::Foo" via namespace hiding.
 *
 *  Design see: docs/superpowers/specs/2026-07-17-ee-gui-adaptation-design.md
 */

#ifndef AURORA_WIDGETNAMERESOLVER_H
#define AURORA_WIDGETNAMERESOLVER_H

#include "src/common/ustring.h"

namespace Aurora {

/** Maps xoreos's expected widget tag to the actual tag in a game variant. */
class WidgetNameResolver {
public:
	virtual ~WidgetNameResolver() {}

	/** Return the variant's actual widget tag for the requested tag.
	 *  Default implementation is a pass-through. */
	virtual Common::UString resolve(const Common::UString &tag) const {
		return tag;
	}
};

} // End of namespace Aurora

#endif // AURORA_WIDGETNAMERESOLVER_H

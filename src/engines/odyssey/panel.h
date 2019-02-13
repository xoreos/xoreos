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
 *  Panel widget for the Odyssey engine.
 */

#ifndef ENGINES_ODYSSEY_PANEL_H
#define ENGINES_ODYSSEY_PANEL_H

#include "src/common/ustring.h"

#include "src/engines/aurora/gui.h"

#include "src/engines/odyssey/widget.h"

namespace Engines {

namespace Odyssey {

class WidgetPanel : public Widget {
public:
	WidgetPanel(GUI &gui, const Common::UString &tag);

	WidgetPanel(GUI &gui,
	            const Common::UString &tag,
	            const Common::UString &texture,
	            float x, float y, float w, float h);
};

} // End of namespace Odyssey

} // End of namespace Engines

#endif // ENGINES_ODYSSEY_PANEL_H

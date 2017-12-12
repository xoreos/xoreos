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
 *  A NWN label widget.
 */

#include "src/engines/nwn/gui/widgets/label.h"

namespace Engines {

namespace NWN {

WidgetLabel::WidgetLabel(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &font, const Common::UString &text) :
	NWNWidgetWithCaption(gui, tag, font, text) {

}

WidgetLabel::~WidgetLabel() {
}

} // End of namespace NWN

} // End of namespace Engines

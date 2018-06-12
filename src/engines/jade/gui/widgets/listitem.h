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
 *  Inventory item widget.
 */

#ifndef ENGINES_JADE_GUI_WIDGETS_LISTITEM_H
#define ENGINES_JADE_GUI_WIDGETS_LISTITEM_H

#include "src/engines/aurora/gui.h"

#include "src/engines/jade/gui/widgets/jadewidget.h"

namespace Engines {

namespace Jade {

class WidgetListItem : public JadeWidget {
public:
	WidgetListItem(Engines::GUI &gui, const Common::UString &tag);
	void load(const Aurora::GFF3Struct &gff);
	void setCount(int count);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_GUI_WIDGETS_LISTITEM_H

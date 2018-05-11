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
 *  A KotOR listbox widget.
 */

#ifndef ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H
#define ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

#include "src/engines/kotor/gui/widgets/kotorwidget.h"

namespace Engines {

namespace KotOR {

class WidgetListBox : public KotORWidget {
public:
	WidgetListBox(::Engines::GUI &gui, const Common::UString &tag);
	~WidgetListBox();

	void load(const Aurora::GFF3Struct &gff);

	KotORWidget *createItem(Common::UString name);

private:
	const Aurora::GFF3Struct *_protoItem;
	const Aurora::GFF3Struct *_scrollBar;

	int _itemCount;
	int _padding;
	bool _leftScrollBar;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_WIDGETS_LISTBOX_H

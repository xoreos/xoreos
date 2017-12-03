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

#include "src/aurora/gff3file.h"

#include "src/engines/kotor/gui/gui.h"
#include "src/engines/kotor/gui/widgets/listbox.h"
#include "src/engines/kotor/gui/widgets/button.h"
#include "src/engines/kotor/gui/widgets/label.h"

namespace Engines {

namespace KotOR {

WidgetListBox::WidgetListBox(::Engines::GUI &gui, const Common::UString &tag) :
	KotORWidget(gui, tag), _protoItem(0), _itemCount(0), _padding(0) {
}

WidgetListBox::~WidgetListBox() {
}

void WidgetListBox::load(const Aurora::GFF3Struct &gff) {
	KotORWidget::load(gff);

	if (gff.hasField("PROTOITEM")) {
		_protoItem = &gff.getStruct("PROTOITEM");
	}

	if (gff.hasField("SCROLLBAR")) {

	}

	if (gff.hasField("PADDING")) {
		_padding = gff.getSint("PADDING");
	}
}

KotORWidget *WidgetListBox::createItem(Common::UString name) {
	if (!_protoItem)
		throw Common::Exception("ListBox widget has no PROTOITEM");

	KotORWidget *item;

	// Create a new widget.
	switch (_protoItem->getUint("CONTROLTYPE")) {
		case 4:
			item = new WidgetLabel(*_gui, name);
			break;
		case 6:
			item = new WidgetButton(*_gui, name);
			break;
		default:
			throw Common::Exception("TODO: Add other supported widget types to the ListBox");
	}

	// Load the prototype item.
	item->load(*_protoItem);

	// Calculate the new position for managing it in a list.
	float x, y, z;
	getPosition(x, y, z);

	assert(getHeight() > 0);

	y = y - _itemCount * (item->getHeight() + _padding) + getHeight() - item->getHeight();

	item->setPosition(x, y, z);
	item->show();

	_itemCount += 1;

	return item;
}

} // End of namespace KotOR

} // End of namespace Engines

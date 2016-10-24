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
 *  A NWN editbox widget.
 */

#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/fontman.h"

#include "src/graphics/aurora/text.h"

#include "src/engines/nwn/gui/widgets/editbox.h"

namespace Engines {

namespace NWN {

WidgetEditBox::WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model) :
	WidgetListBox(gui, tag, model){
}

WidgetEditBox::~WidgetEditBox() {
}

void WidgetEditBox::show() {
	Engines::NWN::WidgetListBox::show();

	if (_title)
		_title->show();
}

void WidgetEditBox::hide() {
	Engines::NWN::WidgetListBox::hide();

	if (_title)
		_title->hide();
}

void WidgetEditBox::setTitle(const Common::UString &font, const Common::UString &title) {
	if (!_title)
		_title.reset(new Graphics::Aurora::Text(FontMan.get(font), title));

	_title->set(title);

	// Get text node position.
	float tX, tY, tZ, wX, wY, wZ;
	getPosition(wX, wY, wZ);
	_model->getNode("title0")->getPosition(tX, tY, tZ);

	tY -= _title->getHeight();

	_title->setPosition(wX + tX, wY + tY, wZ - tZ);

	if (this->isVisible())
		_title->show();
}

} // End of namespace NWN

} // End of namespace Engines

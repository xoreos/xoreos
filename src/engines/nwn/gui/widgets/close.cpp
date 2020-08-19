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
 *  A NWN close button widget.
 */

#include "src/common/system.h"
#include "src/common/ustring.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn/gui/widgets/close.h"

namespace Engines {

namespace NWN {

WidgetClose::WidgetClose(::Engines::GUI &gui, const Common::UString &tag,
                         const Common::UString &model) : ModelWidget(gui, tag, model) {

	_model->setClickable(true);
}

WidgetClose::~WidgetClose() {
}

void WidgetClose::leave() {
	ModelWidget::leave();

	if (isDisabled())
		return;

	_model->setState("");
}

void WidgetClose::mouseDown(uint8_t state, float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	if (state != SDL_BUTTON_LMASK)
		return;

	_model->setState("down");
	playSound("gui_button", Sound::kSoundTypeSFX);
}

void WidgetClose::mouseUp(uint8_t UNUSED(state), float UNUSED(x), float UNUSED(y)) {
	if (isDisabled())
		return;

	_model->setState("");
	setActive(true);
}

} // End of namespace NWN

} // End of namespace Engines

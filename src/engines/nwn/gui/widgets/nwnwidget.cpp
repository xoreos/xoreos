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
 *  A NWN widget.
 */

#include "src/common/ustring.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"
#include "src/engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

NWNWidget::NWNWidget(::Engines::GUI &gui, const Common::UString &tag) : Widget(gui, tag) {
}

NWNWidget::~NWNWidget() {
}

void NWNWidget::hide() {
	if (!isVisible())
		return;

	if (_tooltip)
		_tooltip->hide();

	Widget::hide();
}

void NWNWidget::enter() {
	if (_tooltip)
		_tooltip->show(Tooltip::getDefaultDelay());
}

void NWNWidget::leave() {
	if (_tooltip)
		_tooltip->hide();
}

void NWNWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	if (_tooltip)
		_tooltip->updatePosition();
}

void NWNWidget::setTooltip(const Common::UString &text) {
	createTooltip();

	_tooltip->clearLines();
	_tooltip->addLine(text, 1.0f, 1.0f, 1.0f, 1.0f);
}

void NWNWidget::setTooltipPosition(float x, float y, float z) {
	createTooltip();

	_tooltip->setPosition(x, y, z);
}

void NWNWidget::createTooltip() {
	if (_tooltip)
		return;

	_tooltip = std::make_unique<Tooltip>(Tooltip::kTypeHelp, *this);
	_tooltip->setAlign(0.5f);
}

} // End of namespace NWN

} // End of namespace Engines

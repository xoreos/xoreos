/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/nwnwidget.cpp
 *  A NWN widget.
 */

#include "boost/bind.hpp"

#include "common/ustring.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"
#include "engines/nwn/gui/widgets/tooltip.h"

namespace Engines {

namespace NWN {

NWNWidget::NWNWidget(::Engines::GUI &gui, const Common::UString &tag) : Widget(gui, tag),
	_tooltip() {
}

NWNWidget::~NWNWidget() {
	delete _tooltip;
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
		_tooltip->show();
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
	_tooltip->addLine(text, 1.0, 1.0, 1.0, 1.0);
}

void NWNWidget::setTooltipPosition(float x, float y, float z) {
	createTooltip();

	_tooltip->setPosition(x, y, z);
}

void NWNWidget::createTooltip() {
	if (_tooltip)
		return;

	_tooltip = new Tooltip(Tooltip::kTypeHelp, *this);
	_tooltip->setAlign(0.5);
}

} // End of namespace NWN

} // End of namespace Engines

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
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

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

#include "common/util.h"
#include "common/ustring.h"

#include "graphics/aurora/fontman.h"
#include "graphics/aurora/model.h"
#include "graphics/aurora/text.h"

#include "engines/aurora/model.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"

namespace Engines {

namespace NWN {

NWNWidget::NWNWidget(::Engines::GUI &gui, const Common::UString &tag) : Widget(gui, tag),
	_hasTooltip(false),
	_tooltipX(0.0), _tooltipY(0.0), _tooltipZ(0.0),
	_tooltip(0), _tooltipBubble(0) {

}

NWNWidget::~NWNWidget() {
	delete _tooltip;
	delete _tooltipBubble;
}

void NWNWidget::hide() {
	if (!isVisible())
		return;

	hideTooltip();

	Widget::hide();
}

void NWNWidget::enter() {
	showTooltip();
}

void NWNWidget::leave() {
	hideTooltip();
}

void NWNWidget::showTooltip() {
	if (!_hasTooltip)
		return;

	_tooltip->show();
	_tooltipBubble->show();
}

void NWNWidget::hideTooltip() {
	if (!_hasTooltip)
		return;

	_tooltip->hide();
	_tooltipBubble->hide();
}

void NWNWidget::setPosition(float x, float y, float z) {
	Widget::setPosition(x, y, z);

	setTooltipPosition();
}

void NWNWidget::setTooltip(const Common::UString &font, const Common::UString &text) {
	hideTooltip();

	delete _tooltip;
	delete _tooltipBubble;

	_tooltip       = 0;
	_tooltipBubble = 0;

	_hasTooltip = false;

	if (font.empty() || text.empty())
		return;

	_tooltip = new Graphics::Aurora::Text(FontMan.get(font), text, 1.0, 1.0, 1.0, 1.0, 0.5);
	_tooltip->setTag(getTag() + "#Tooltip");

	Common::UString bubbleModel =
		getBubbleModel(_tooltip->getLineCount(), _tooltip->getWidth());

	_tooltipBubble = loadModelGUI(bubbleModel);
	if (!_tooltipBubble) {
		warning("NWNWidget::setTooltip(): Failed loading model \"%s\"", bubbleModel.c_str());

		delete _tooltip;
		_tooltip = 0;
		return;
	}

	_tooltipBubble->setTag(getTag() + "#TooltipBubble");

	_hasTooltip = true;

	setTooltipPosition();
}

void NWNWidget::setTooltipPosition(float x, float y, float z) {
	_tooltipX = x;
	_tooltipY = y;
	_tooltipZ = z;

	setTooltipPosition();
}

void NWNWidget::setTooltipPosition() {
	if (!_hasTooltip)
		return;

	float x, y, z;
	getPosition(x, y, z);

	const float bubbleWidth  = _tooltipBubble->getWidth () - 30.0;
	const float bubbleHeight = _tooltipBubble->getHeight() -  8.0;

	const float textWidth  = _tooltip->getWidth();
	const float textHeight = _tooltip->getHeight();

	const float bubbleX = x + _tooltipX - (bubbleWidth / 2.0);
	const float bubbleY = y + _tooltipY;
	const float bubbleZ = z + _tooltipZ;

	const float textX = x + _tooltipX - (textWidth / 2.0);
	const float textY = y + _tooltipY - (bubbleHeight / 2.0) - (textHeight / 2.0);
	const float textZ = z + _tooltipZ - 1.0;

	_tooltip->setPosition(textX, textY, textZ);
	_tooltipBubble->setPosition(bubbleX, bubbleY, bubbleZ);
}

Common::UString NWNWidget::getBubbleModel(uint32 lines, float width) {
	uint32 modelLines = 0;
	uint32 modelWidth = 0;

	if      (lines <=  1)
		modelLines =  1;
	else if (lines ==  2)
		modelLines =  2;
	else if (lines ==  3)
		modelLines =  3;
	else if (lines ==  4)
		modelLines =  4;
	else if (lines ==  5)
		modelLines =  5;
	else if (lines <=  7)
		modelLines =  7;
	else if (lines <= 10)
		modelLines = 10;
	else if (lines <= 16)
		modelLines = 16;
	else
		modelLines = 32;

	if ((modelLines >= 1) && (modelLines <= 3)) {
		if      (width <= 100.0)
			modelWidth = 100;
		else if (width <= 150.0)
			modelWidth = 150;
		else
			modelWidth = 300;
	} else
		modelWidth = 300;

	return Common::UString::sprintf("pnl_bubble%d_%d", modelLines, modelWidth);
}

} // End of namespace NWN

} // End of namespace Engines

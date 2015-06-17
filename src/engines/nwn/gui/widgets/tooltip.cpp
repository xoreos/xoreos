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
 *  A tooltip.
 */

#include <boost/bind.hpp>

#include "src/common/util.h"
#include "src/common/configman.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"
#include "src/graphics/camera.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/guiquad.h"

#include "src/engines/aurora/model.h"
#include "src/engines/aurora/widget.h"

#include "src/engines/nwn/gui/widgets/tooltip.h"
#include "src/engines/nwn/gui/widgets/portrait.h"

namespace Engines {

namespace NWN {

Tooltip::Tooltip(Type type) : _type(type),
	_parentWidget(0), _parentModel(0),
	_empty(true), _visible(false), _align(0.0f),
	_bubble(0), _portrait(0), _offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(false), _detectEdge(false) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::Tooltip(Type type, Widget &parent) : _type(type),
	_parentWidget(&parent), _parentModel(0),
	_empty(true), _visible(false), _align(0.0f),
	_bubble(0), _portrait(0), _offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(false), _detectEdge(true) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::Tooltip(Type type, Graphics::Aurora::Model &parent) : _type(type),
	_parentWidget(0), _parentModel(&parent),
	_empty(true), _visible(false), _align(0.0f),
	_bubble(0), _portrait(0), _offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(true), _detectEdge(false) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::~Tooltip() {
	hide();

	for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l)
		delete l->text;
	delete _portrait;
	delete _bubble;
}

void Tooltip::clearLines() {
	hide();

	for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l)
		delete l->text;

	_lines.clear();

	redoLayout();
}

void Tooltip::clearPortrait() {
	hide();

	delete _portrait;
	_portrait = 0;

	redoLayout();
}

void Tooltip::addLine(const Common::UString &text, float r, float g, float b, float a) {
	hide();

	if (text.empty())
		return;

	std::vector<Common::UString> lines;
	Common::UString::split(text, '\n', lines);

	for (std::vector<Common::UString>::const_iterator l = lines.begin();
	     l != lines.end(); ++l) {

		_lines.push_back(Line());

		_lines.back().r    = r;
		_lines.back().g    = g;
		_lines.back().b    = b;
		_lines.back().a    = a;
		_lines.back().line = *l;
		_lines.back().text = new Graphics::Aurora::Text(FontMan.get(getFontName()), *l, r, g, b, a);
		_lines.back().text->setTag("Tooltip#Text");
	}

	redoLayout();
}

void Tooltip::setPortrait(const Common::UString &image) {
	hide();

	if (_portrait)
		_portrait->setPortrait(image);
	else
		_portrait = new Portrait(image, Portrait::kSizeTiny, 1.0f);

	redoLayout();
}

void Tooltip::setAlign(float align) {
	hide();

	_align = align;
	redoLayout();
}

void Tooltip::notifyCameraMoved() {
	updatePosition();
}

bool Tooltip::getParentPosition(float &x, float &y, float &z) const {
	x = y = z = 0.0f;

	bool onscreen = true;

	if (_parentWidget)
		_parentWidget->getPosition(x, y, z);

	if (_parentModel) {
		float aX, aY, aZ;

		_parentModel->getTooltipAnchor(aX, aY, aZ);
		if (!GfxMan.project(aX, aY, aZ, x, y, z))
			return false;

		onscreen = ((z >= 0.0f) && (z <= 1.0f));

		z = 0.0f;
	}

	return onscreen;
}

void Tooltip::updatePosition() {
	if (_empty)
		return;

	Common::StackLock lock(_mutex);

	float pX, pY, pZ;
	if (!getParentPosition(pX, pY, pZ)) {
		_offscreen = true;
		doHide();
		return;
	} else {
		_offscreen = false;
		doShow();
	}

	// Set bubble position

	const bool hasBubble = _showBubble && _bubble;

	const float bubbleWidth  = hasBubble ? (_bubble->getWidth () - 30.0f) : _width;
	const float bubbleHeight = hasBubble ? (_bubble->getHeight() -  8.0f) : _height;

	const float bubbleWantX = pX + _x - (bubbleWidth / 2.0f);
	const float bubbleRight = bubbleWantX + bubbleWidth + 15.0f;

	const float maxX  = _detectEdge ? GfxMan.getScreenWidth() / 2.0f : 0.0f;
	const float overX = _detectEdge ? MAX(0.0f, bubbleRight - maxX)  : 0.0f;

	const float bubbleX = bubbleWantX - overX;
	const float bubbleY = pY + _y;
	const float bubbleZ = pZ + _z;

	if (hasBubble)
		_bubble->setPosition(floorf(bubbleX), floorf(bubbleY), floorf(bubbleZ));


	// Set portrait position

	const bool hasPortrait = _showPortrait && _portrait;

	const float portraitWidth  = hasPortrait ? _portrait->getWidth () : 0.0f;
	const float portraitHeight = hasPortrait ? _portrait->getHeight() : 0.0f;

	const float portraitBorderY = (bubbleHeight - portraitHeight) / 2.0f;

	const float portraitX = bubbleX + 5.0f;
	const float portraitY = bubbleY - bubbleHeight + portraitBorderY + 1.0f;
	const float portraitZ = bubbleZ - 1.0f;

	if (hasPortrait)
		_portrait->setPosition(floorf(portraitX), floorf(portraitY), floorf(portraitZ));


	// Set text position

	const float portraitSpacerWidth = portraitWidth + (_portrait ? 10.0f : 0.0f);

	const float bubbleTextWidth = bubbleWidth - portraitSpacerWidth;

	const float textHeight = _lines.size() * _lineHeight + (_lines.size() - 1) * _lineSpacing;

	const float textBorderY = (bubbleHeight - textHeight) / 2.0f;

	const float textBottomX = bubbleX + portraitSpacerWidth;
	const float textBottomY = bubbleY - textBorderY + 1.0f;
	const float textBottomZ = bubbleZ - 1.0f;

	float textY = textBottomY;
	for (std::vector<Line>::reverse_iterator l = _lines.rbegin(); l != _lines.rend(); ++l) {
		if (l->text) {
			const float textWidth   = l->text->getWidth();
			const float textBorderX = (bubbleTextWidth - textWidth) * _align;
			const float textX       = textBottomX + textBorderX;
			const float lineY       = textY - l->text->getHeight();
			l->text->setPosition(floorf(textX), floorf(lineY), floorf(textBottomZ));
		}

		textY -= (_lineHeight + _lineSpacing);
	}
}

void Tooltip::setPosition(float x, float y, float z) {
	hide();

	_x = x;
	_y = y;
	_z = z;

	updatePosition();
}

void Tooltip::show() {
	if (_visible || _empty)
		return;

	redoLines();

	uint32 delay = ConfigMan.getInt("tooltipdelay", 100);
	if (delay == 0)
		doShow(0);
	else
		TimerMan.addTimer(delay, _timer,
		                  boost::bind(&Tooltip::doShow, this, _1));

	_visible = true;
}

void Tooltip::hide() {
	TimerMan.removeTimer(_timer);

	_visible = false;
	doHide();
}

void Tooltip::getSize(float &width, float &height) {
	width = 0.0f;
	for (std::vector<Line>::const_iterator l = _lines.begin(); l != _lines.end(); ++l)
		if (l->text)
			width = MAX(width, l->text->getWidth());

	if (_portrait)
		width += _portrait->getWidth() + 10.0f;

	height = 0.0f;
	if (_lines.size() > 0)
		height = (_lines.size() * _lineHeight) + ((_lines.size() - 1) * _lineSpacing);

	if (_portrait)
		height = MAX(height, _portrait->getHeight());
}

void Tooltip::checkEmpty() {
	_empty = !_portrait && _lines.empty();
}

void Tooltip::redoLines() {
	bool needRedo = false;

	Common::UString fontName = getFontName();
	if (fontName != _font) {
		needRedo = true;

		for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l) {
			delete l->text;
			l->text = 0;
		}

		_font = fontName;

		Graphics::Aurora::FontHandle font = FontMan.get(_font);

		_lineHeight  = font.getFont().getHeight();
		_lineSpacing = font.getFont().getLineSpacing();

		_width  = 0.0f;
		_height = 0.0f;
	}

	bool showBubble, showText, showPortrait;
	getFeedbackMode(showBubble, showText, showPortrait);

	if ((showBubble   != _showBubble  ) ||
		  (showText     != _showText    ) ||
			(showPortrait != _showPortrait)) {

		needRedo = true;

		_width  = 0.0f;
		_height = 0.0f;

		_showBubble   = showBubble;
		_showText     = showText;
		_showPortrait = showPortrait;
	}

	if (needRedo)
		redoLayout();
}

void Tooltip::redoBubble() {
	delete _bubble;
	_bubble = 0;

	if (!_showBubble || (_height <= 0.0f))
		return;

	float  height = _height - _lineHeight;
	uint32 lines  = 1;

	while (height > _lineSpacing) {
		height -= (_lineSpacing + _lineHeight);
		lines++;
	}

	Common::UString bubbleModel = getBubbleModel(lines, _width);

	_bubble = loadModelGUI(bubbleModel);
	if (!_bubble) {
		warning("Tooltip::redoBubble(): Failed loading model \"%s\"", bubbleModel.c_str());
		return;
	}

	_bubble->setTag("Tooltip#Bubble");
}

void Tooltip::redoLayout() {
	checkEmpty();
	if (_empty)
		return;

	if (_font.empty())
		_font = getFontName();

	Graphics::Aurora::FontHandle font = FontMan.get(_font);

	_lineHeight  = font.getFont().getHeight();
	_lineSpacing = font.getFont().getLineSpacing();

	for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l) {
		if (l->text)
			continue;

		l->text = new Graphics::Aurora::Text(font, l->line, l->r, l->g, l->b, l->a);
		l->text->setTag("Tooltip#Text");
	}

	float width, height;
	getSize(width, height);

	if ((_width != width) || (_height != height)) {
		_width  = width;
		_height = height;

		redoBubble();
	}

	updatePosition();
}

void Tooltip::doShow() {
	if (_empty || _offscreen || !_visible)
		return;

	GfxMan.lockFrame();

	if (_bubble && _showBubble)
		_bubble->show();
	if (_portrait && _showPortrait)
		_portrait->show();

	if (_showText)
		for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l)
			if (l->text)
				l->text->show();

	GfxMan.unlockFrame();
}

void Tooltip::doHide() {
	if (_visible)
		return;

	GfxMan.lockFrame();

	if (_bubble)
		_bubble->hide();
	if (_portrait)
		_portrait->hide();

	for (std::vector<Line>::iterator l = _lines.begin(); l != _lines.end(); ++l)
		if (l->text)
			l->text->hide();

	GfxMan.unlockFrame();
}

uint32 Tooltip::doShow(uint32 UNUSED(oldInterval)) {
	Common::StackLock lock(_mutex);
	doShow();
	return 0;
}

void Tooltip::getFeedbackMode(bool &showBubble, bool &showText, bool &showPortrait) const {
	if (_type == kTypeHelp) {
		showBubble   = true;
		showText     = true;
		showPortrait = false;
		return;
	}

	uint32 mode = ConfigMan.getInt("feedbackmode", 2);

	showBubble   = mode == 2;
	showText     = mode >= 1;
	showPortrait = mode == 2;
}

Common::UString Tooltip::getFontName() {
	return ConfigMan.getBool("largefonts") ? "fnt_dialog_big16" : "fnt_dialog16x16";
}

Common::UString Tooltip::getBubbleModel(uint32 lines, float width) {
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
		if      (width <= 100.0f)
			modelWidth = 100;
		else if (width <= 150.0f)
			modelWidth = 150;
		else
			modelWidth = 300;
	} else
		modelWidth = 300;

	return Common::UString::format("pnl_bubble%d_%d", modelLines, modelWidth);
}

} // End of namespace NWN

} // End of namespace Engines

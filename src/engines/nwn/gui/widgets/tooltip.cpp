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
	_offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(false), _detectEdge(false) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::Tooltip(Type type, Widget &parent) : _type(type),
	_parentWidget(&parent), _parentModel(0),
	_empty(true), _visible(false), _align(0.0f),
	_offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(false), _detectEdge(true) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::Tooltip(Type type, Graphics::Aurora::Model &parent) : _type(type),
	_parentWidget(0), _parentModel(&parent),
	_empty(true), _visible(false), _align(0.0f),
	_offscreen(false), _x(0.0f), _y(0.0f), _z(0.0f),
	_lineHeight(0.0f), _lineSpacing(0.0f), _width(0.0f), _height(0.0f),
	_needCamera(true), _detectEdge(false) {

	getFeedbackMode(_showBubble, _showText, _showPortrait);
}

Tooltip::~Tooltip() {
	hide();

	deleteTexts();
}

void Tooltip::clearLines() {
	hide();

	_lines.clear();

	redoLines(true);
}

void Tooltip::clearPortrait() {
	hide();

	_portrait.reset();

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
	}

	redoLines(true);
}

void Tooltip::setPortrait(const Common::UString &image) {
	hide();

	if (_portrait)
		_portrait->setPortrait(image);
	else
		_portrait.reset(new Portrait(image, Portrait::kSizeTiny, 1.0f));

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

	const float maxX  = _detectEdge ? WindowMan.getWindowWidth() / 2.0f : 0.0f;
	const float overX = _detectEdge ? MAX(0.0f, bubbleRight - maxX)  : 0.0f;

	const float bubbleX = bubbleWantX - overX;
	const float bubbleY = pY + bubbleHeight + _y;
	const float bubbleZ = pZ + _z;

	if (hasBubble)
		_bubble->setPosition(floorf(bubbleX), floorf(bubbleY), floorf(bubbleZ));


	// Set portrait position

	const bool hasPortrait = _showPortrait && _portrait;

	const float portraitWidth  = hasPortrait ? _portrait->getWidth () : 0.0f;
	const float portraitHeight = hasPortrait ? _portrait->getHeight() : 0.0f;

	const float portraitX = bubbleX;
	const float portraitY = bubbleY - portraitHeight - 3.0f;
	const float portraitZ = bubbleZ - 1.0f;

	if (hasPortrait)
		_portrait->setPosition(floorf(portraitX), floorf(portraitY), floorf(portraitZ));


	// Set text position

	const float portraitSpacerWidth = portraitWidth + (_portrait ? 2.0f : 0.0f);

	const float bubbleTextWidth = bubbleWidth - portraitSpacerWidth;

	const float textHeight = _texts.size() * _lineHeight + (_texts.size() - 1) * _lineSpacing;

	const float textBorderY = (bubbleHeight - textHeight) / 2.0f;

	const float textBottomX = bubbleX + portraitSpacerWidth;
	const float textBottomY = bubbleY - textBorderY + 1.0f;
	const float textBottomZ = bubbleZ - 1.0f;

	float textY = textBottomY;
	for (std::vector<Graphics::Aurora::Text *>::iterator t = _texts.begin(); t != _texts.end(); ++t) {
		const float textWidth   = (*t)->getWidth();
		const float textBorderX = (bubbleTextWidth - textWidth) * _align;
		const float textX       = textBottomX + textBorderX;
		const float lineY       = textY - (*t)->getHeight();
		(*t)->setPosition(floorf(textX), floorf(lineY), floorf(textBottomZ));

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

void Tooltip::show(uint32 delay, uint32 timeOut) {
	if (_visible || _empty)
		return;

	redoLines();

	_visible = true;

	if (delay == 0)
		doShow(0);

	if (delay   != 0)
		TimerMan.addTimer(delay          , _timerShow, boost::bind(&Tooltip::doShow, this, _1));
	if (timeOut != 0)
		TimerMan.addTimer(delay + timeOut, _timerHide, boost::bind(&Tooltip::doHide, this, _1));
}

void Tooltip::hide() {
	Common::StackLock lock(_mutex);

	if (!_visible)
		return;

	_visible = false;

	TimerMan.removeTimer(_timerHide);
	TimerMan.removeTimer(_timerShow);

	doHide();
}

void Tooltip::getSize(float &width, float &height) {
	width = 0.0f;
	for (Common::PtrVector<Graphics::Aurora::Text>::const_iterator t = _texts.begin(); t != _texts.end(); ++t)
		width = MAX(width, (*t)->getWidth());

	if (_portrait)
		width += _portrait->getWidth() + 2.0f;

	height = 0.0f;
	if (_texts.size() > 0)
		height = (_texts.size() * _lineHeight) + ((_texts.size() - 1) * _lineSpacing);

	if (_portrait)
		height = MAX(height, _portrait->getHeight());
}

void Tooltip::checkEmpty() {
	_empty = !_portrait && _lines.empty();
}

void Tooltip::deleteTexts() {
	_texts.clear();
}

bool Tooltip::createTexts(float width, size_t maxLines) {
	deleteTexts();

	Graphics::Aurora::FontHandle font = FontMan.get(_font);

	const float maxWidth = _showBubble ? (width - (_showPortrait ? 18.0f : 0.0f)) : 0.0f;

	for (std::vector<Line>::const_iterator l = _lines.begin(); l != _lines.end(); l++) {
		std::vector<Common::UString> lineLines;

		font.getFont().split(l->line, lineLines, maxWidth);

		for (std::vector<Common::UString>::const_iterator i = lineLines.begin(); i != lineLines.end(); ++i) {
			_texts.push_back(new Graphics::Aurora::Text(font, *i, l->r, l->g, l->b, l->a));
			_texts.back()->setTag("Tooltip#Text");
		}
	}

	return !_showBubble || !maxLines || (_texts.size() <= maxLines);
}

void Tooltip::redoLines(bool force) {
	bool needRedo = force;

	bool showBubble, showText, showPortrait;
	getFeedbackMode(showBubble, showText, showPortrait);

	const Common::UString fontName = getFontName();

	if ((fontName     != _font        ) ||
	    (showBubble   != _showBubble  ) ||
		  (showText     != _showText    ) ||
			(showPortrait != _showPortrait)) {

		_font         = fontName;
		_showBubble   = showBubble;
		_showText     = showText;
		_showPortrait = showPortrait;

		needRedo = true;
	}

	if (!needRedo)
		return;

	Graphics::Aurora::FontHandle font = FontMan.get(_font);

	_lineHeight  = font.getFont().getHeight();
	_lineSpacing = font.getFont().getLineSpacing();

	_width  = 0.0f;
	_height = 0.0f;

	if (!createTexts(100.0f, 3))
		if (!createTexts(150.f, 3))
			createTexts(300.0f);

	redoLayout();
}

void Tooltip::redoBubble() {
	_bubble.reset();

	if (!_showBubble || (_height <= 0.0f))
		return;

	float  height = _height - _lineHeight;
	uint32 lines  = 1;

	while (height > _lineSpacing) {
		height -= (_lineSpacing + _lineHeight);
		lines++;
	}

	Common::UString bubbleModel = getBubbleModel(lines, _width);

	_bubble.reset(loadModelGUI(bubbleModel));
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
		for (Common::PtrVector<Graphics::Aurora::Text>::iterator t = _texts.begin(); t != _texts.end(); ++t)
			(*t)->show();

	GfxMan.unlockFrame();
}

void Tooltip::doHide() {
	GfxMan.lockFrame();

	if (_bubble)
		_bubble->hide();
	if (_portrait)
		_portrait->hide();

	for (Common::PtrVector<Graphics::Aurora::Text>::iterator t = _texts.begin(); t != _texts.end(); ++t)
		(*t)->hide();

	GfxMan.unlockFrame();
}

uint32 Tooltip::doShow(uint32 UNUSED(oldInterval)) {
	Common::StackLock lock(_mutex);
	doShow();
	return 0;
}

uint32 Tooltip::doHide(uint32 UNUSED(oldInterval)) {
	Common::StackLock lock(_mutex);

	if (!_visible)
		return 0;

	_visible = false;

	TimerMan.removeTimer(_timerShow);

	doHide();
	return 0;
}

void Tooltip::getFeedbackMode(bool &showBubble, bool &showText, bool &showPortrait) const {
	uint32 mode = ConfigMan.getInt("feedbackmode", 2);

	showBubble   = mode == 2;
	showText     = mode >= 1;
	showPortrait = mode == 2;

	switch (_type) {
		case kTypeHelp:
			showBubble   = true;
			showText     = true;
			showPortrait = false;
			break;

		case kTypeSpeech:
			showText     = true;
			break;

		default:
			break;
	}
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

uint32 Tooltip::getDefaultDelay() {
	return (uint32) ConfigMan.getInt("tooltipdelay", 100);
}

} // End of namespace NWN

} // End of namespace Engines

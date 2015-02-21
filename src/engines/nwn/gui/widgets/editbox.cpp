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

#include "src/common/system.h"
#include "src/common/error.h"

#include "src/graphics/font.h"
#include "src/graphics/aurora/model.h"
#include "src/graphics/aurora/modelnode.h"
#include "src/graphics/aurora/fontman.h"

#include "src/graphics/aurora/text.h"

#include "src/engines/nwn/gui/widgets/button.h"
#include "src/engines/nwn/gui/widgets/scrollbar.h"
#include "src/engines/nwn/gui/widgets/editbox.h"

namespace Engines {

namespace NWN {

WidgetEditBox::WidgetEditBox(::Engines::GUI &gui, const Common::UString &tag,
                             const Common::UString &model, const Common::UString &font) :
	ModelWidget(gui, tag, model), _font(font) {

	_title = 0;

	getProperties();
	createScrollbar();

	_linesToShow = 18;

	_model->setClickable(true);
}

WidgetEditBox::~WidgetEditBox() {
	delete _title;

	for (std::vector<Graphics::Aurora::Text *>::iterator it = _text.begin(); it != _text.end(); ++it)
		delete *it;
}

void WidgetEditBox::show() {
	Engines::NWN::ModelWidget::show();

	updateLines();

	if (_title)
		_title->show();

	if (_hasScrollbar) {
		_scrollbar->show();
		_down->show();
		_up->show();
	}
}

void WidgetEditBox::hide() {
	Engines::NWN::ModelWidget::hide();

	for (std::vector<Graphics::Aurora::Text *>::iterator it = _text.begin(); it != _text.end(); ++it)
		(*it)->hide();

	if (_title)
		_title->hide();

	if (_hasScrollbar) {
		_scrollbar->hide();
		_down->hide();
		_up->hide();
	}
}

void WidgetEditBox::mouseDown(uint8 UNUSED(state), float x, float y) {
	if (isDisabled())
		return;

	if (_hasScrollbar) {
		float wX, wY, wZ;
		getPosition(wX, wY, wZ);
		// Check if we clicked on the scrollbar area

		if (x > (wX + getWidth() - 20)) {
			if (y > _scrollbar->getBarPosition()) {
				scroll(-1);
			} else {
				scroll(1);
			}

			return;
		}
	}
}

void WidgetEditBox::mouseWheel(uint8 UNUSED(state), int UNUSED(x), int y) {
	if (isDisabled())
		return;

	scroll(-y / abs(y));
}

void WidgetEditBox::getProperties() {
	// Do we have a scroll bar?
	_hasScrollbar = _model->hasNode("scrollmin") && _model->hasNode("scrollmax");
	// Do we have a title
	_hasTitle = _model->hasNode("title0");
}

void WidgetEditBox::setText(const Common::UString & text) {
	//FIXME We should use something like a render-to-texture mechanism and not Graphics::Aurora::Text.

	// Clean old text if any.
	for (std::vector<Graphics::Aurora::Text *>::iterator it = _text.begin(); it != _text.end(); ++it)
		delete *it;

	_text.clear();

	float leftMargin = 0.0;
	if (_hasScrollbar)
		leftMargin += 30.0;

	// Split text and create Graphics::Aurora::Text.
	std::vector<Common::UString> textLine;
	FontMan.get(_font).getFont().split(text, textLine, getWidth() - leftMargin);

	for (std::vector<Common::UString>::iterator it = textLine.begin(); it != textLine.end(); ++it)
		_text.push_back(new Graphics::Aurora::Text(FontMan.get(_font), *it));

	// Get text node position.
	if (!_model->hasNode("text0"))
		throw Common::Exception("WidgetEditBox \"%s\" doesn't have \"text0\" node.", getTag().c_str());

	// Get maximum lines allowed.
	float tX, tY, tZ, wX, wY, wZ;
	getPosition(wX, wY, wZ);
	_model->getNode("text0")->getPosition(tX, tY, tZ);

	pX = wX + tX;
	pY = wY + tY;
	pZ = wZ - tZ;

	float maxTextHeight = tY;
	float fontHeight = FontMan.get(_font).getFont().getHeight();

	pY -= fontHeight;

	_linesToShow = floor(maxTextHeight/fontHeight);

	_firstLine = 0;

	updateScrollbarLength();
	updateScrollbarPosition();

	if (this->isVisible())
		updateLines();
}

void WidgetEditBox::setTitle(const Common::UString & title) {
	if (!_hasTitle)
		return;

	if (!_title)
		_title = new Graphics::Aurora::Text(FontMan.get(_font), title);

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

void WidgetEditBox::subActive(Widget &widget) {
	// TODO Add the ability to continue to scroll when the button Up and Down is still pressed
	if (widget.getTag().endsWith("#Up")) {
		scroll(-1);
		return;
	}

	if (widget.getTag().endsWith("#Down")) {
		scroll(1);
		return;
	}

	if (widget.getTag().endsWith("#Bar")) {
		int futurePosition = _firstLine - floor(_scrollbar->getState() * (_text.size() - _linesToShow + 1));

		scroll(-futurePosition);
	}
}

void WidgetEditBox::createScrollbar() {
	if (!_hasScrollbar)
		return;

	// Get top position
	float minX, minY, minZ;
	_model->getNode("scrollmin")->getPosition(minX, minY, minZ);
	// Create the "up" button
	_up = new WidgetButton(*_gui, getTag() + "#Up", "pb_scrl_up", "gui_scroll");
	_up->setPosition(minX, minY, -100.0);
	addSub(*_up);
	addChild(*_up);

	// Get bottom position
	float maxX, maxY, maxZ;
	_model->getNode("scrollmax")->getPosition(maxX, maxY, maxZ);
	// Create the "down" button
	_down = new WidgetButton(*_gui, getTag() + "#Down", "pb_scrl_down", "gui_scroll");
	_down->setPosition(maxX, maxY - 10, -100.0);
	addSub(*_down);
	addChild(*_down);

	// Scroll bar range (max length)
	float scrollRange = minY - (maxY - 10) - _up->getHeight() - 1;
	// Create the scrollbar
	_scrollbar = new WidgetScrollbar(*_gui, getTag() + "#Bar", Scrollbar::kTypeVertical, scrollRange);
	// Center the bar within the scrollbar area
	float scrollX = maxX + (_up->getWidth() - _scrollbar->getWidth()) / 2;
	// Move it to the base position
	float scrollY = maxY - 10 + _up->getHeight();
	_scrollbar->setPosition(scrollX, scrollY, -100.0);
	addSub(*_scrollbar);
	addChild(*_scrollbar);
}

void WidgetEditBox::updateScrollbarLength() {
	// TODO Add condition to ensure a minimal length
	if (!_scrollbar)
		return;

	if (_text.size() <= _linesToShow)
		_scrollbar->setLength(1.0);
	else
		_scrollbar->setLength((float) _linesToShow / (float) _text.size());
}

void WidgetEditBox::updateScrollbarPosition() {
	if (!_scrollbar)
		return;

	float max = _text.size() - _linesToShow + 1;
	if (max == 1)
		return;

	_scrollbar->setState(_firstLine / (max - 1));
}

void WidgetEditBox::scroll(int n) {
	if (_text.size() <= _linesToShow)
		return;

	if (_firstLine + n > _text.size() - _linesToShow || (int)_firstLine + n < 0)
		return;

	_firstLine += n;

	updateLines();

	if (!_scrollbar->isActive())
		updateScrollbarPosition();
}

void WidgetEditBox::updateLines() {
	float fontHeight = FontMan.get(_font).getFont().getHeight();

	for (uint line = 0; line < _text.size(); ++line) {
		if (line < _firstLine || line > _linesToShow + _firstLine - 1) {
			_text[line]->hide();
			continue;
		}

		_text[line]->setPosition(pX, pY - (line - _firstLine) * fontHeight, pZ);
		_text[line]->show();
	}
}

} // End of namespace NWN

} // End of namespace Engines

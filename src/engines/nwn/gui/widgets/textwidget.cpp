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
 *  A NWN text widget.
 */

#include "src/common/ustring.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/nwn/gui/widgets/textwidget.h"

namespace Engines {

namespace NWN {

TextWidget::TextWidget(::Engines::GUI &gui, const Common::UString &tag,
                       const Common::UString &font, const Common::UString &text) :
	NWNWidget(gui, tag), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f) {

	_text = new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a, 0.5f);
	_text->setTag(tag);
}

TextWidget::~TextWidget() {
	delete _text;
}

void TextWidget::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_text->show();

	NWNWidget::show();
}

void TextWidget::hide() {
	if (!isVisible())
		return;

	_text->hide();
	NWNWidget::hide();
}

void TextWidget::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_text->setPosition(x, y, z);
}

void TextWidget::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_text->setColor(_r, _g, _b, _a);
}

void TextWidget::setText(const Common::UString &text, float align, float maxWidth, float maxHeight) {
	_text->set(text, maxWidth, maxHeight);
	_text->setAlign(align);
}

const Common::UString TextWidget::getText() const {
	return _text->get();
}

float TextWidget::getWidth() const {
	return _text->getWidth();
}

float TextWidget::getHeight() const {
	return _text->getHeight();
}

void TextWidget::setDisabled(bool disabled) {
	if (disabled == isDisabled())
		return;

	_a = disabled ? (_a * 0.6f) : (_a / 0.6f);

	_text->setColor(_r, _g, _b, _a);

	NWNWidget::setDisabled(disabled);
}

} // End of namespace NWN

} // End of namespace Engines

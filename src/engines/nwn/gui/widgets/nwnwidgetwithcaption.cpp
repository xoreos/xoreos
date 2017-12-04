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
 *  A NWN widget with a text caption.
 */

#include "src/common/ustring.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/nwn/gui/widgets/nwnwidgetwithcaption.h"

namespace Engines {

namespace NWN {

NWNWidgetWithCaption::NWNWidgetWithCaption(::Engines::GUI &gui, const Common::UString &tag) :
	NWNWidget(gui, tag), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f) {

}

NWNWidgetWithCaption::~NWNWidgetWithCaption() {
}

void NWNWidgetWithCaption::show() {
	if (isVisible())
		return;

	if (_caption && !isInvisible())
		_caption->show();

	NWNWidget::show();
}

void NWNWidgetWithCaption::hide() {
	if (!isVisible())
		return;

	if (_caption)
		_caption->hide();
	NWNWidget::hide();
}

void NWNWidgetWithCaption::initCaption(const Common::UString &font, const Common::UString &text,
                                       float r, float g, float b, float a, float halign, float valign) {

	_caption.reset(new Graphics::Aurora::Text(FontMan.get(font), getWidth(), getHeight(), text, r, g, b, a, halign, valign));
	_caption->setTag(getTag() + "#Caption");

	float x, y, z;
	getPosition(x, y, z);
	_caption->setPosition(x, y, z - 1.0);
}

void NWNWidgetWithCaption::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	if (_caption)
		_caption->setPosition(x, y, z - 1.0);
}

void NWNWidgetWithCaption::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	if (_caption)
		_caption->setColor(_r, _g, _b, _a);
}

void NWNWidgetWithCaption::setText(const Common::UString &text) {
	if (!_caption)
		return;

	_caption->setText(text);
}

const Common::UString NWNWidgetWithCaption::getText() const {
	if (_caption)
		return _caption->get();
	return "";
}

float NWNWidgetWithCaption::getWidth() const {
	if (_caption)
		return _caption->getWidth();
	return 0.0f;
}

float NWNWidgetWithCaption::getHeight() const {
	if (_caption)
		return _caption->getHeight();
	return 0.0f;
}

float NWNWidgetWithCaption::getHorizontalAlign() const {
	if (_caption)
		return _caption->getHorizontalAlign();
	return Graphics::Aurora::kHAlignLeft;
}

float NWNWidgetWithCaption::getVerticalAlign() const {
	if (_caption)
		return _caption->getVerticalAlign();
	return Graphics::Aurora::kVAlignTop;
}

void NWNWidgetWithCaption::setDisabled(bool disabled) {
	if (disabled == isDisabled())
		return;

	_a = disabled ? (_a * 0.6f) : (_a / 0.6f);

	if (_caption)
		_caption->setColor(_r, _g, _b, _a);

	NWNWidget::setDisabled(disabled);
}

} // End of namespace NWN

} // End of namespace Engines

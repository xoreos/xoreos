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

NWNWidgetWithCaption::NWNWidgetWithCaption(::Engines::GUI &gui, const Common::UString &tag,
                       const Common::UString &font, const Common::UString &text) :
	NWNWidget(gui, tag), _r(1.0f), _g(1.0f), _b(1.0f), _a(1.0f) {

	_caption.reset(new Graphics::Aurora::Text(FontMan.get(font), text, _r, _g, _b, _a, Graphics::Aurora::kHAlignCenter));
	_caption->setTag(tag);
}

NWNWidgetWithCaption::~NWNWidgetWithCaption() {
}

void NWNWidgetWithCaption::show() {
	if (isVisible())
		return;

	if (!isInvisible())
		_caption->show();

	NWNWidget::show();
}

void NWNWidgetWithCaption::hide() {
	if (!isVisible())
		return;

	_caption->hide();
	NWNWidget::hide();
}

void NWNWidgetWithCaption::setPosition(float x, float y, float z) {
	NWNWidget::setPosition(x, y, z);

	getPosition(x, y, z);
	_caption->setPosition(x, y, z);
}

void NWNWidgetWithCaption::setColor(float r, float g, float b, float a) {
	_r = r;
	_g = g;
	_b = b;
	_a = a;

	_caption->setColor(_r, _g, _b, _a);
}

void NWNWidgetWithCaption::setText(const Common::UString &text, float halign, float maxWidth, float maxHeight) {
	_caption->set(text, maxWidth, maxHeight);
	_caption->setHorizontalAlign(halign);
}

const Common::UString NWNWidgetWithCaption::getText() const {
	return _caption->get();
}

float NWNWidgetWithCaption::getWidth() const {
	return _caption->getWidth();
}

float NWNWidgetWithCaption::getHeight() const {
	return _caption->getHeight();
}

void NWNWidgetWithCaption::setDisabled(bool disabled) {
	if (disabled == isDisabled())
		return;

	_a = disabled ? (_a * 0.6f) : (_a / 0.6f);

	_caption->setColor(_r, _g, _b, _a);

	NWNWidget::setDisabled(disabled);
}

} // End of namespace NWN

} // End of namespace Engines

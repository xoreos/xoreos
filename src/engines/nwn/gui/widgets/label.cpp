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
 *  A NWN label widget.
 */
#include "src/graphics/font.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/text.h"

#include "src/engines/nwn/gui/widgets/label.h"

namespace Engines {

namespace NWN {

WidgetLabel::WidgetLabel(::Engines::GUI &gui, const Common::UString &tag) :
	NWNWidgetWithCaption(gui, tag), _width(0.0), _height(0.0) {

}

WidgetLabel::~WidgetLabel() {
}

void WidgetLabel::initCaption(const Common::UString &font, const Common::UString &text,
                              float r, float g, float b, float a, float halign, float valign) {

	// Not all labels are wide enough for their text
	float textWidth = 1.0 + FontMan.get(font).getFont().getLineWidth(text);
	float textHeight = FontMan.get(font).getFont().getHeight();
	float oldWidth = _width;

	// Only for labels that can contain at max 1 line
	if ((2 * textHeight) > _height)
		_width = MAX(_width, textWidth);

	NWNWidgetWithCaption::initCaption(font, text, r, g, b, a, halign, valign);

	float x, y, z;
	getPosition(x, y, z);
	// Anchor for right aligned labels is right X position
	if (getHorizontalAlign() == Graphics::Aurora::kHAlignRight)
		setPosition(x - _width, y, z);
	// If the label is big enough it does not get recentered
	if (oldWidth < _width && getHorizontalAlign() == Graphics::Aurora::kHAlignCenter)
		setPosition(x - (_width / 2), y, z);
}

void WidgetLabel::setText(const Common::UString &text) {
	if (!_caption)
		return;

	// Not all labels are wide enough for their text
	float textWidth = 1.0 + FontMan.get(_caption->getFont()).getFont().getLineWidth(text);
	if (textWidth > _width) {
		float x, y, z;
		getPosition(x, y, z);
		setPosition(x + ((_width - textWidth) * getHorizontalAlign()), y, z);
		setSize(textWidth, _height);
	}

	NWNWidgetWithCaption::setText(text);

}

float WidgetLabel::getWidth () const {
	return _width;
}

float WidgetLabel::getHeight() const {
	return _height;
}

void WidgetLabel::setSize(float width, float height) {
	_width = width;
	_height = height;
	if (_caption)
		_caption->setSize(_width, _height);
}

} // End of namespace NWN

} // End of namespace Engines

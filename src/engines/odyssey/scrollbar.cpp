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
 *  Scrollbar widget for the Odyssey engine.
 */

#include "src/aurora/gff3file.h"

#include "src/graphics/aurora/textureman.h"

#include "src/engines/odyssey/scrollbar.h"

namespace Engines {

namespace Odyssey {

WidgetScrollbar::WidgetScrollbar(GUI &gui, const Common::UString &tag) :
		Widget(gui, tag),
		_arrowHeight(0.0f) {
}

void WidgetScrollbar::load(const Aurora::GFF3Struct &gff) {
	Widget::load(gff);

	float x, y, z;
	getPosition(x, y, z);

	if (gff.hasField("DIR")) {
		const Aurora::GFF3Struct &dir = gff.getStruct("DIR");
		Common::UString texName = dir.getString("IMAGE");
		if (!texName.empty()) {
			Graphics::Aurora::TextureHandle arrowTex = TextureMan.get(dir.getString("IMAGE"));
			_arrowHeight = arrowTex.getTexture().getHeight();

			_upArrow.reset(new Graphics::Aurora::GUIQuad(arrowTex, 0.0f, 0.0f, _width, _arrowHeight));
			_upArrow->setPosition(x, y + _height - _arrowHeight, 0.0f);

			_downArrow.reset(new Graphics::Aurora::GUIQuad(arrowTex, 0.0f, 0.0f, _width, _arrowHeight, 0.0f, 1.0f, 1.0f, 0.0f));
			_downArrow->setPosition(x, y, 0.0f);
		}
	}

	if (gff.hasField("THUMB")) {
		const Aurora::GFF3Struct &thumb = gff.getStruct("THUMB");

		_thumb.reset(new Graphics::Aurora::GUIQuad(thumb.getString("IMAGE"), 0.0f, 0.0f,
				_width - 2 * _borderDimension,
				_height - 2 * (_borderDimension + _arrowHeight)));

		_thumb->setPosition(x + _borderDimension, y + _arrowHeight + _borderDimension, 0.0f);
	}

	if (_highlight) {
		_highlight->setPosition(x, y + _arrowHeight, 0.0f);
		_highlight->setHeight(_height - 2 * _arrowHeight);
	}

	if (_border) {
		_border->setPosition(x, y + _arrowHeight, 0.0f);
		_border->setSize(_width, _height - 2 * _arrowHeight);
	}

	if (_quad) {
		_quad->setPosition(x, y + _borderDimension + _arrowHeight, 0.0f);
		_quad->setHeight(_height - 2 * (_borderDimension + _arrowHeight));
	}
}

void WidgetScrollbar::show() {
	if (isVisible() || isInvisible())
		return;

	Widget::show();

	if (_upArrow)
		_upArrow->show();
	if (_downArrow)
		_downArrow->show();
	if (_thumb)
		_thumb->show();
}

void WidgetScrollbar::hide() {
	if (!isVisible())
		return;

	if (_thumb)
		_thumb->hide();
	if (_downArrow)
		_downArrow->hide();
	if (_upArrow)
		_upArrow->hide();

	Widget::hide();
}

void WidgetScrollbar::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	float dx = x - oX;
	float dy = y - oY;
	float dz = z - oZ;

	Widget::setPosition(x, y, z);

	if (_upArrow) {
		_upArrow->getPosition(x, y, z);
		_upArrow->setPosition(x + dx, y + dy, z + dz);
	}

	if (_downArrow) {
		_downArrow->getPosition(x, y, z);
		_downArrow->setPosition(x + dx, y + dy, z + dz);
	}

	if (_thumb) {
		_thumb->getPosition(x, y, z);
		_thumb->setPosition(x + dx, y + dy, z + dz);
	}
}

void WidgetScrollbar::setWidth(float width) {
	float deltaWidth = width - _width;

	Widget::setWidth(width);

	float x, y, z;
	getPosition(x, y, z);

	if (_upArrow) {
		width = _upArrow->getWidth();
		_upArrow->setWidth(width + deltaWidth);
	}

	if (_downArrow) {
		width = _downArrow->getWidth();
		_downArrow->setWidth(width + deltaWidth);
	}

	if (_thumb) {
		width = _thumb->getWidth();
		_thumb->setWidth(width + deltaWidth);
	}
}

void WidgetScrollbar::setHeight(float height) {
	float deltaHeight = height - _height;

	Widget::setHeight(height);

	float x, y, z;
	getPosition(x, y, z);

	if (_upArrow) {
		float aX, aY, aZ;
		_upArrow->getPosition(aX, aY, aZ);
		_upArrow->setPosition(aX, y + height - _arrowHeight, aZ);
	}

	if (_thumb) {
		height = _thumb->getHeight();
		_thumb->setHeight(height + deltaHeight);
	}
}

} // End of namespace Odyssey

} // End of namespace Engines

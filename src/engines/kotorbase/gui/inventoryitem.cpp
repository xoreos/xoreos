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
 *  Inventory item widget for KotOR games.
 */

#include "src/common/strutil.h"

#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/textureman.h"

#include "src/engines/kotorbase/gui/inventoryitem.h"

namespace Engines {

namespace KotORBase {

WidgetInventoryItem::WidgetInventoryItem(Engines::GUI &gui, const Common::UString &tag) : WidgetProtoItem(gui, tag) {
	setDisableHighlight(true);
}

void WidgetInventoryItem::setContents(const Common::UString &contents) {
	std::vector<Common::UString> tokens;
	Common::UString::split(contents, '|', tokens);
	if (tokens.size() < 3)
		return;

	if (_text)
		_text->setText(tokens[0]);
	if (_icon)
		_icon->setTexture(tokens[1]);

	uint32_t count;
	Common::parseString(tokens[2], count);

	if (count > 1) {
		_iconFrame->setTexture("lbl_hex_4");
		_countText->setText(tokens[2]);
	} else {
		_iconFrame->setTexture("lbl_hex");
		_countText->setText("");
	}
}

void WidgetInventoryItem::load(const Aurora::GFF3Struct &gff) {
	Odyssey::WidgetProtoItem::load(gff);

	float x, y, z;
	getPosition(x, y, z);

	Graphics::Aurora::TextureHandle tex = TextureMan.get("lbl_hex");
	float frameSize = tex.getTexture().getHeight();
	_iconFrame = std::make_unique<Graphics::Aurora::GUIQuad>(tex, 0.0f, 0.0f, frameSize, frameSize);
	_iconFrame->setPosition(x, y + (_height - frameSize) / 2.0f, 0.0f);

	_icon = std::make_unique<Graphics::Aurora::GUIQuad>("", 0.0f, 0.0f, frameSize, frameSize);
	_icon->setPosition(x, y + (_height - frameSize) / 2.0f, 0.0f);

	if (_border) {
		float bX, bY, bZ;
		_border->getPosition(bX, bY, bZ);
		_border->setPosition(bX + frameSize, bY, bZ);

		float width, height;
		_border->getSize(width, height);
		_border->setSize(width - frameSize, height);
	}

	if (_text) {
		float tX, tY, tZ;
		_text->getPosition(tX, tY, tZ);
		_text->setPosition(tX + frameSize, tY, tZ);

		float width = _text->getWidth();
		float height = _text->getHeight();
		_text->setSize(width - frameSize, height);

		_text->setVerticalAlign(Graphics::Aurora::kVAlignMiddle);

		float r, g, b, a;
		_text->getColor(r, g, b, a);

		_countText = std::make_unique<Graphics::Aurora::Text>(FontMan.get("fnt_d16x16"), "", r, g, b, a, Graphics::Aurora::kHAlignRight, Graphics::Aurora::kVAlignBottom);

		_countText->setPosition(x - 4.0f, y + (_height - frameSize) / 2.0f - 2.0f, 0.0f);
		_countText->setSize(frameSize, frameSize);
	}
}

void WidgetInventoryItem::show() {
	if (isVisible() || isInvisible())
		return;

	Odyssey::Widget::show();

	if (_iconFrame)
		_iconFrame->show();
	if (_icon)
		_icon->show();
	if (_countText)
		_countText->show();
}

void WidgetInventoryItem::hide() {
	if (!isVisible())
		return;

	if (_countText)
		_countText->hide();
	if (_icon)
		_icon->hide();
	if (_iconFrame)
		_iconFrame->hide();

	Odyssey::Widget::hide();
}

void WidgetInventoryItem::setPosition(float x, float y, float z) {
	float oX, oY, oZ;
	getPosition(oX, oY, oZ);

	float dx = x - oX;
	float dy = y - oY;
	float dz = z - oZ;

	Odyssey::Widget::setPosition(x, y, z);

	if (_iconFrame) {
		_iconFrame->getPosition(x, y, z);
		_iconFrame->setPosition(x + dx, y + dy, z + dz);
	}

	if (_icon) {
		_icon->getPosition(x, y, z);
		_icon->setPosition(x + dx, y + dy, z + dz);
	}

	if (_countText) {
		_countText->getPosition(x, y, z);
		_countText->setPosition(x + dx, y + dy, z + dz);
	}
}

void WidgetInventoryItem::setWidth(float width) {
	float deltaWidth = _width - width;

	Odyssey::Widget::setWidth(width);

	if (_countText) {
		width = _countText->getWidth();
		_countText->setSize(width + deltaWidth, _countText->getHeight());
	}
}

} // End of namespace KotORBase

} // End of namespace Engines

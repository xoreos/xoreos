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
 *  Inventory item widget.
 */

#include "src/common/strutil.h"

#include "src/graphics/aurora/guiquad.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/engines/jade/gui/widgets/listitem.h"

namespace Engines {

namespace Jade {

WidgetListItem::WidgetListItem(Engines::GUI &gui, const Common::UString &tag)
		: JadeWidget(gui, tag) {
}

void WidgetListItem::load(const Aurora::GFF3Struct &gff) {
	JadeWidget::load(gff);

	Graphics::Aurora::TextureHandle tex = TextureMan.get("lbl_hex");
	float frameSize = tex.getTexture().getHeight();
	_iconFrame.reset(new Graphics::Aurora::GUIQuad(tex, 0.0f, 0.0f, frameSize, frameSize));
	_iconFrame->setPosition(frameSize / 2.0f, frameSize / 2.0f, -1.0f);

	_icon.reset(new Graphics::Aurora::GUIQuad("", 0.0f, 0.0f, frameSize, frameSize));
	_icon->setPosition(frameSize / 2.0f, frameSize / 2.0f, -2.0f);

	float r, g, b, a;
	_text->getColor(r, g, b, a);

	_countText.reset(new Graphics::Aurora::Text(FontMan.get("fnt_d16x16b"),
	                 "", r, g, b, a,
	                 Graphics::Aurora::kHAlignRight,
	                 Graphics::Aurora::kVAlignBottom));

	_countText->setPosition(frameSize / 2.0f - 4.0f, frameSize / 2.0f + 2.0f, -3.0f);
	_countText->setSize(frameSize, frameSize);

	float x, y, z;
	_text->getPosition(x, y, z);
	_text->setPosition(x + frameSize, y, z);
}

void WidgetListItem::setCount(int count) {
	if (count > 1) {
		_iconFrame->setTexture("lbl_hex_4");
		_countText->setText(Common::composeString(count));
	} else {
		_iconFrame->setTexture("lbl_hex");
		_countText->setText("");
	}
}

} // End of namespace Jade

} // End of namespace Engines

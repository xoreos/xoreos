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

#include "src/graphics/aurora/highlightabletext.h"

namespace Graphics {

namespace Aurora {

HighlightableText::HighlightableText(const FontHandle &font, float w, float h, const Common::UString &str, float r, float g, float b, float a, float halign, float valign) :
	Text(font, w, h, str, r, g, b, a, halign, valign) {
}

HighlightableText::~HighlightableText() {
}

void HighlightableText::render(RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	if (isHighlightable() && isHightlighted()) {
		float initialR, initialG, initialB, initialA, r, g, b, a;
		getColor(initialR, initialG, initialB, initialA);
		incrementColor(initialR, initialG, initialB, initialA, r, g, b, a);
		setColor(r, g, b, a);
	}
	Graphics::Aurora::Text::render(pass);
}

} // End of namespace Aurora

} // End of namespace Graphics

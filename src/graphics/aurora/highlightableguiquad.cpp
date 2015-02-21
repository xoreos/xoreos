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

#include "src/graphics/aurora/highlightableguiquad.h"

namespace Graphics {

namespace Aurora {

HighlightableGUIQuad::HighlightableGUIQuad(const Common::UString &texture,
		float x1, float y1, float x2, float y2,
		float tX1, float tY1, float tX2, float tY2) :
	GUIQuad (texture, x1, y1, x2, y2, tX1, tY1, tX2, tY2) {
}

HighlightableGUIQuad::~HighlightableGUIQuad() {
}

void HighlightableGUIQuad::render(RenderPass pass) {
	if (isHighlightable() && isHightlighted()) {
		float initialR, initialG, initialB, initialA, r, g, b, a;
		getColor(initialR, initialG, initialB, initialA);
		incrementColor(initialR, initialG, initialB, initialA, r, g, b, a);
		setColor(r, g, b, a);
	}
	Graphics::Aurora::GUIQuad::render(pass);
}

} // End of namespace Aurora

} // End of namespace Graphics

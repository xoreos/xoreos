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

#include "src/graphics/aurora/textureman.h"
#include "highlightableborder.h"

namespace Graphics {

namespace Aurora {

HighlightableBorder::HighlightableBorder(const Common::UString &edge, const Common::UString &corner, float x,
                                         float y, float w, float h,
                                         const Common::UString &cornerH, const Common::UString &edgeH): BorderQuad(edge, corner, x, y, w, h)
{
    _edge = TextureMan.get(edge);
    _edgeH = TextureMan.get(edgeH);
    _corner = TextureMan.get(corner);
    _cornerH = TextureMan.get(cornerH);

}

HighlightableBorder::~HighlightableBorder() {}

void HighlightableBorder::render(RenderPass pass) {
    if(isHighlightable() && isHightlighted()){
        setCornerTexture(_cornerH);
        setEdgeTexture(_edgeH);
    } else {
        setCornerTexture(_corner);
        setEdgeTexture(_edge);
    }
    Graphics::Aurora::BorderQuad::render(pass);
}

void HighlightableBorder::setHighlightCornerTexture(const TextureHandle &corner) {
    _cornerH = corner;
}

void HighlightableBorder::setHighlightEdgeTexture(const TextureHandle &edge) {
    _edgeH = edge;
}

}

}

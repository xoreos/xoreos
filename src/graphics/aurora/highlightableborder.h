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


#ifndef XOREOS_HIGHLIGHTABLEBORDER_H
#define XOREOS_HIGHLIGHTABLEBORDER_H



#include "src/graphics/aurora/borderquad.h"
#include "src/graphics/aurora/highlightable.h"

namespace Graphics {

namespace Aurora {

class HighlightableBorder : public Highlightable, public BorderQuad {

public:
    HighlightableBorder(const Common::UString &edge, const Common::UString &corner, float x, float y, float w, float h, const Common::UString &cornerH, const Common::UString &edgeH);
    ~HighlightableBorder();

    void render(RenderPass pass);

    void setHighlightCornerTexture(const TextureHandle& corner);
    void setHighlightEdgeTexture(const TextureHandle& edge);

private:
    TextureHandle _cornerH;
    TextureHandle _edgeH;

    TextureHandle _edge;
    TextureHandle _corner;
};


}

}



#endif //XOREOS_HIGHLIGHTABLEBORDER_H

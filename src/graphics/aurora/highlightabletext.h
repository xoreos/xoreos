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

#ifndef GRAPHICS_AURORA_HIGHLIGHTABLETEXT_H
#define GRAPHICS_AURORA_HIGHLIGHTABLETEXT_H

#include "src/graphics/aurora/text.h"
#include "src/graphics/aurora/highlightable.h"

namespace Graphics {

namespace Aurora {

class HighlightableText: public Text, public Highlightable {

  public:
	HighlightableText(const FontHandle &font, float w, float h, const Common::UString &str,
	     float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f,
	     float halign = kHAlignLeft, float valign = kVAlignTop);
	~HighlightableText();

	void render(RenderPass pass);

};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_HIGHLIGHTABLETEXT_H

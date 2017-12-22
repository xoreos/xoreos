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
 *  A textured quad for a GUI element.
 */

#ifndef GRAPHICS_AURORA_GUIQUAD_H
#define GRAPHICS_AURORA_GUIQUAD_H

#include "src/common/maths.h"

#include <src/graphics/guielement.h>

#include "src/graphics/aurora/texturehandle.h"

namespace Common {
	class UString;
}

namespace Graphics {

namespace Aurora {

class GUIQuad : public GUIElement {
public:
	GUIQuad(const Common::UString &texture,
	        float  x1       , float  y1       , float  x2       , float  y2,
	        float tX1 = 0.0f, float tY1 = 0.0f, float tX2 = 1.0f, float tY2 = 1.0f);
	GUIQuad(Graphics::GUIElement::GUIElementType type, const Common::UString &texture,
	        float  x1       , float  y1       , float  x2       , float  y2,
	        float tX1 = 0.0f, float tY1 = 0.0f, float tX2 = 1.0f, float tY2 = 1.0f);
	GUIQuad(TextureHandle texture,
	        float  x1       , float  y1       , float  x2       , float  y2,
	        float tX1 = 0.0f, float tY1 = 0.0f, float tX2 = 1.0f, float tY2 = 1.0f);
	~GUIQuad();

	/** Get the current position of the quad. */
	void getPosition(float &x, float &y, float &z) const;
	/** Set the current position of the quad. */
	void setPosition(float x, float y, float z = -FLT_MAX);

	/** Get the current color of the quad */
	void getColor(float &r, float &g, float &b, float &a) const;
	/** Set the current color of the quad. */
	void setColor(float r, float g, float b, float a);
	/** Set the current texture of the quad. */
	void setTexture(const Common::UString &texture);
	/** Set the current texture of the quad. */
	void setTexture(TextureHandle texture);

	/** Set the scissor test parameters. */
	void setScissor(int x, int y, int width, int height);

	float getWidth () const; ///< Return the quad's width.
	float getHeight() const; ///< Return the quad's height.

	void setWidth (float w); ///< Set the quad's width.
	void setHeight(float h); ///< Set the quad's height.

	void setXOR(bool enabled); ///< Enable/Disable XOR mode.

	void setScissor(bool enabled); ///< Enable/Disable Scissor Test.

	/** Is the point within the quad? */
	bool isIn(float x, float y) const;

	// Renderable
	void calculateDistance();
	void render(RenderPass pass);

private:
	TextureHandle _texture;

	float _r;
	float _g;
	float _b;
	float _a;

	float _x1;
	float _y1;
	float _x2;
	float _y2;

	float _tX1;
	float _tY1;
	float _tX2;
	float _tY2;

	int _scissorX;
	int _scissorY;
	int _scissorWidth;
	int _scissorHeight;

	bool _xor;
	bool _scissor;
	bool _additiveBlending;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_GUIQUAD_H

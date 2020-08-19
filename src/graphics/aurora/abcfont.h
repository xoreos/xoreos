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
 *  An ABC/SBM font, as used by Jade Empire.
 */

#ifndef GRAPHICS_AURORA_ABCFONT_H
#define GRAPHICS_AURORA_ABCFONT_H

#include <map>

#include "src/common/types.h"

#include "src/graphics/font.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/graphics/mesh/meshfont.h"

#include "src/graphics/shader/shaderrenderable.h"

namespace Common {
	class UString;
	class SeekableReadStream;
}

namespace Graphics {

namespace Aurora {

/** An ABC/SBM font, as used by Jade Empire. */
class ABCFont : public Graphics::Font {
public:
	ABCFont(const Common::UString &name);
	~ABCFont();

	float getWidth (uint32_t c) const;
	float getHeight()           const;

	void draw(uint32_t c) const;

	/**
	 * @brief Bind the font for rendering. Must be performed before render is called.
	 * @param transform  Base modelview transform. Under most circumstances this is expected to be the identity matrix.
	 */
	virtual void renderBind(const glm::mat4 &transform) const;

	virtual void render(uint32_t c, float &x, float &y, float *rgba) const;
	virtual void renderUnbind() const;

private:
	/** A font character. */
	struct Char {
		uint32_t dataX;
		uint32_t dataY;
		uint8_t  width;
		int8_t   spaceL;
		int8_t   spaceR;

		float tX[4], tY[4];
		float vX[4], vY[4];
	};

	TextureHandle _texture;
	Mesh::MeshFont *_mesh;
	Shader::ShaderMaterial *_material;
	Shader::ShaderRenderable *_renderable;

	uint8_t _base;

	Char _invalid;
	Char _ascii[128];
	std::map<uint32_t, Char> _extended;

	void load(const Common::UString &name);

	void readCharDesc(Char &c, Common::SeekableReadStream &abc);
	void calcCharVertices(Char &c);

	const Char &findChar(uint32_t c) const;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_ABCFONT_H

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
 *  A texture font, as used by NWN and KotOR/KotOR2.
 */

#include <vector>

#include "src/common/types.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/util.h"
#include "src/common/encoding.h"

#include "src/aurora/language.h"

#include "src/graphics/images/txi.h"

#include "src/graphics/aurora/texturefont.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/mesh/meshman.h"

#include "src/graphics/shader/surfaceman.h"

namespace Graphics {

namespace Aurora {

static uint32_t convertToUTF32(byte c, Common::Encoding encoding) {
	if ((encoding == Common::kEncodingInvalid) || (encoding == Common::kEncodingASCII))
		return (uint32_t) c;

	byte data[2] = { c, 0x00 };

	const Common::UString str = Common::readString(data, 2, encoding);
	if (str.empty())
		return 0;

	return *str.begin();
}


// TODO: Multibyte fonts?
TextureFont::TextureFont(const Common::UString &name) : _height(1.0f), _spaceR(0.0f), _spaceB(0.0f) {
	_texture = TextureMan.get(name);

	load();

	_mesh = static_cast<Mesh::MeshFont *>(MeshMan.getMesh("defaultMeshFont"));
	_material = new Shader::ShaderMaterial(ShaderMan.getShaderObject("default/text.frag", Shader::SHADER_FRAGMENT), "text");
	Shader::ShaderSampler *sampler;
	sampler = (Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	sampler->handle = _texture;
	_renderable = new Shader::ShaderRenderable(SurfaceMan.getSurface("textSurface"), _material, _mesh);
}

TextureFont::~TextureFont() {
	delete _renderable;
	delete _material;
}

float TextureFont::getWidth(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);

	if (cC == _chars.end())
		cC = _chars.find('m');
	if (cC == _chars.end())
		return _spaceR;

	return cC->second.width + _spaceR;
}

float TextureFont::getHeight() const {
	return _height;
}

float TextureFont::getLineSpacing() const {
	return _spaceB;
}

void TextureFont::drawMissing() const {
	TextureMan.set();

	float width = getWidth('m') - _spaceR;

	glBegin(GL_QUADS);
		glVertex2f(0.0f ,    0.0f);
		glVertex2f(width,    0.0f);
		glVertex2f(width, _height);
		glVertex2f(0.0f , _height);
	glEnd();

	glTranslatef(width + _spaceR, 0.0f, 0.0f);
}

void TextureFont::draw(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);

	if (cC == _chars.end()) {
		drawMissing();
		return;
	}

	TextureMan.set(_texture);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC->second.tX[i], cC->second.tY[i]);
		glVertex2f  (cC->second.vX[i], cC->second.vY[i]);
	}
	glEnd();

	glTranslatef(cC->second.width + _spaceR, 0.0f, 0.0f);
}

void TextureFont::renderBind(const glm::mat4 &transform) const {
	glUseProgram(_renderable->getProgram()->glid);
	_material->bindProgram(_renderable->getProgram(), 1.0f);
	_material->bindGLState();
	_renderable->getSurface()->bindProgram(_renderable->getProgram(), &transform);
	_renderable->getSurface()->bindGLState();
	_mesh->renderBind();

	/**
	 * Mesh data will be dynamically updated for each character drawn to the screen
	 * (at least for now), so make sure the mesh VBO is bound. This isn't required
	 * for GL2.1, but is for GL3.2 because using the VAO doesn't automatically bind
	 * the VBO for data updates.
	 */
	glBindBuffer(GL_ARRAY_BUFFER, _mesh->getVertexBuffer()->getVBO());
}

void TextureFont::render(uint32_t c, float &x, float &y, float *rgba) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);

	if (cC == _chars.end()) {
		//drawMissing();
		return;
	}

	float v_pos[12];
	float v_uv[8];
	float v_rgba[4*4];

	for (int i = 0; i < 4; ++i) {
		v_uv[i*2] = cC->second.tX[i];
		v_uv[i*2 +1] = cC->second.tY[i];
		v_pos[i*3] = x + cC->second.vX[i];
		v_pos[i*3 +1] = y + cC->second.vY[i];
		v_pos[i*3 +2] = 0.0f;
		v_rgba[i*4] = rgba[0];
		v_rgba[i*4 +1] = rgba[1];
		v_rgba[i*4 +2] = rgba[2];
		v_rgba[i*4 +3] = rgba[3];
	}
	_mesh->render(v_pos, v_uv, v_rgba);
	x += cC->second.width + _spaceR;
}

void TextureFont::renderUnbind() const {
	_mesh->renderUnbind();

	_renderable->getSurface()->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void TextureFont::load() {
	const Texture &texture = _texture.getTexture();
	const TXI::Features &txiFeatures = texture.getTXI().getFeatures();

	// Number of characters
	uint32_t charCount = txiFeatures.numChars;
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	if ((charCount > 256) || (txiFeatures.fileRange > 1) || (txiFeatures.isDoubleByte))
		throw Common::Exception("TODO: Multibyte texture font");

	// Character coordinates
	const std::vector<TXI::Coords> &uls = txiFeatures.upperLeftCoords;
	const std::vector<TXI::Coords> &lrs = txiFeatures.lowerRightCoords;
	if ((uls.size() < charCount) || (lrs.size() < charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	if ((texture.getWidth() == 0) || (texture.getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", texture.getWidth(), texture.getHeight());

	double textureRatio = ((double) texture.getWidth()) / ((double) texture.getHeight());

	// Get features
	_height = txiFeatures.fontHeight * 100.0f;
	_spaceR = txiFeatures.spacingR   * 100.0f;
	_spaceB = txiFeatures.spacingB   * 100.0f;

	const Common::Encoding encoding = LangMan.getCurrentEncoding();

	// Build the character texture and vertex coordinates
	for (uint32_t i = 0; i < charCount; i++) {
		if ((encoding != Common::kEncodingInvalid) && !Common::isValidCodepoint(encoding, i))
			continue;

		std::pair<std::map<uint32_t, Char>::iterator,bool> result;

		result = _chars.insert(std::make_pair(convertToUTF32((byte) i, encoding), Char()));
		if (!result.second)
			continue;

		TXI::Coords ul = uls[i];
		TXI::Coords lr = lrs[i];
		Char &c = result.first->second;

		// Texture coordinates, directly out of the TXI
		c.tX[0] = ul.x; c.tY[0] = lr.y;
		c.tX[1] = lr.x; c.tY[1] = lr.y;
		c.tX[2] = lr.x; c.tY[2] = ul.y;
		c.tX[3] = ul.x; c.tY[3] = ul.y;

		double height = ABS(lr.y - ul.y);
		double width  = ABS(lr.x - ul.x);
		double ratio  = ((height != 0.0f) ? (width / height) : 0.0f) * textureRatio;

		// Vertex coordinates. Fixed height, width to fit the texture ratio
		c.vX[0] = 0.00f;           c.vY[0] = 0.00f;
		c.vX[1] = _height * ratio; c.vY[1] = 0.00f;
		c.vX[2] = _height * ratio; c.vY[2] = _height;
		c.vX[3] = 0.00f;           c.vY[3] = _height;

		c.width = c.vX[1] - c.vX[0];
	}
}

} // End of namespace Aurora

} // End of namespace Graphics

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
#include "src/aurora/resman.h"

#include "src/graphics/images/txi.h"

#include "src/graphics/aurora/texturefont.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/mesh/meshman.h"

#include "src/graphics/shader/surfaceman.h"

namespace Graphics {

namespace Aurora {

namespace {

uint32_t convertToUTF32(byte c, Common::Encoding encoding) {
	if ((encoding == Common::kEncodingInvalid) || (encoding == Common::kEncodingASCII))
		return (uint32_t) c;

	byte data[2] = { c, 0x00 };

	const Common::UString str = Common::readString(data, 2, encoding);
	if (str.empty())
		return 0;

	return *str.begin();
}

uint32_t convertToUTF32(byte c1, byte c2, Common::Encoding encoding) {
	if (c1 == 0)
		return convertToUTF32(c2, encoding);

	byte data[3] = { c1, c2, 0 };

	const Common::UString str = Common::readString(data, sizeof(data), encoding);
	if (str.empty())
		return 0;

	return *str.begin();
}

Common::Encoding mapCodePage(uint32_t codepage) {
	switch (codepage) {
	case 0:
		// Many fonts don't have a codepage, so try to take a guess
		switch (LangMan.getCurrentEncoding()) {
		case Common::kEncodingCP932:
		case Common::kEncodingCP936:
		case Common::kEncodingCP949:
		case Common::kEncodingCP950:
			// For native CJK games, most use a codepage. Some are leftover European
			// languages, so use CP1252 for those.
			return Common::kEncodingCP1252;
		default:
			// For non-CJK games, go with the current encoding.
			return LangMan.getCurrentEncoding();
		}
	case 932:
		return Common::kEncodingCP932;
	case 936:
		return Common::kEncodingCP936;
	case 949:
		return Common::kEncodingCP949;
	default:
		throw Common::Exception("Unhandled texture font codepage %u", codepage);
	}
}

} // End of anonymous namespace


TextureFont::TextureFont(const Common::UString &name) : _height(1.0f), _spaceR(0.0f), _spaceB(0.0f), _mesh() {
	load(name);
}

TextureFont::~TextureFont() {
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

	TextureMan.set(_textures[cC->second.sheet]);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC->second.tX[i], cC->second.tY[i]);
		glVertex2f  (cC->second.vX[i], cC->second.vY[i]);
	}
	glEnd();

	glTranslatef(cC->second.width + _spaceR, 0.0f, 0.0f);
}

void TextureFont::renderBind(const glm::mat4 &transform) const {
	if (!_renderable || !_material)
		return;

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
	if (!_mesh)
		return;

	// FIXME: This does not support multiple sheets (see constructor)

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
	if (!_mesh || !_renderable || !_material)
		return;

	_mesh->renderUnbind();

	_renderable->getSurface()->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void TextureFont::load(const Common::UString &name) {
	// See if we have a standalone TXI first
	std::unique_ptr<Common::SeekableReadStream> txiStream(ResMan.getResource(name, ::Aurora::kFileTypeTXI));
	if (txiStream)
		loadTXI(name, std::make_unique<TXI>(*txiStream));
	else
		loadSingleTexture(name);
}

void TextureFont::loadSingleTexture(const Common::UString &name, std::unique_ptr<TXI> srcTxi) {
	// Look for the texture
	if (srcTxi)
		_textures.push_back(TextureMan.get(name, *srcTxi));
	else
		_textures.push_back(TextureMan.get(name));

	const Texture &texture0 = _textures.front().getTexture();

	const TXI::Features &txiFeatures = texture0.getTXI().getFeatures();
	const std::vector<TXI::Coords> &uls = txiFeatures.upperLeftCoords;
	const std::vector<TXI::Coords> &lrs = txiFeatures.lowerRightCoords;

	// Number of characters
	const uint32_t charCount = txiFeatures.numChars;
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// We can't support >=256 chars or multiple sheets
	if (txiFeatures.fileRange > 1)
		throw Common::Exception("Single byte texture only supports a single sheet");
	if (charCount > 256)
		throw Common::Exception("Single file texture fonts only support 256 characters");
	// Character coordinates
	if ((uls.size() < charCount) || (lrs.size() < charCount))
		throw Common::Exception("Texture defines not enough character coordinates");

	if ((texture0.getWidth() == 0) || (texture0.getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", texture0.getWidth(), texture0.getHeight());

	const double textureRatio = ((double) texture0.getWidth()) / ((double) texture0.getHeight());

	// Get features
	_height = txiFeatures.fontHeight * 100.0f;
	_spaceR = txiFeatures.spacingR   * 100.0f;
	_spaceB = txiFeatures.spacingB   * 100.0f;

	_mesh = static_cast<Mesh::MeshFont *>(MeshMan.getMesh("defaultMeshFont"));
	_material.reset(new Shader::ShaderMaterial(ShaderMan.getShaderObject("default/text.frag", Shader::SHADER_FRAGMENT), "text"));
	Shader::ShaderSampler *sampler;
	sampler = (Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	sampler->handle = _textures[0];
	_renderable.reset(new Shader::ShaderRenderable(SurfaceMan.getSurface("textSurface"), _material.get(), _mesh));

	const Common::Encoding encoding = mapCodePage(txiFeatures.codepage);

	// Build the character texture and vertex coordinates
	for (uint32_t i = 0; i < charCount; i++) {
		if ((encoding != Common::kEncodingInvalid) && !Common::isValidCodepoint(encoding, i))
			continue;

		std::pair<std::map<uint32_t, Char>::iterator, bool> result;

		result = _chars.insert(std::make_pair(convertToUTF32(static_cast<byte>(i), encoding), Char()));
		if (!result.second)
			return;

		const TXI::Coords &ul = uls[i];
		const TXI::Coords &lr = lrs[i];
		Char &c = result.first->second;

		c.sheet = 0;

		// Texture coordinates, directly out of the TXI
		c.tX[0] = ul.x; c.tY[0] = lr.y;
		c.tX[1] = lr.x; c.tY[1] = lr.y;
		c.tX[2] = lr.x; c.tY[2] = ul.y;
		c.tX[3] = ul.x; c.tY[3] = ul.y;

		const double height = ABS(lr.y - ul.y);
		const double width  = ABS(lr.x - ul.x);
		const double ratio  = ((height != 0.0f) ? (width / height) : 0.0f) * textureRatio;

		// Vertex coordinates. Fixed height, width to fit the texture ratio
		c.vX[0] = 0.00f;           c.vY[0] = 0.00f;
		c.vX[1] = _height * ratio; c.vY[1] = 0.00f;
		c.vX[2] = _height * ratio; c.vY[2] = _height;
		c.vX[3] = 0.00f;           c.vY[3] = _height;

		c.width = c.vX[1] - c.vX[0];
	}
}

void TextureFont::loadTXI(const Common::UString &name, std::unique_ptr<TXI> txi) {
	const TXI::Features &txiFeatures = txi->getFeatures();
	if (!txiFeatures.isDoubleByte) {
		loadSingleTexture(name, std::move(txi));
		return;
	}

	// Number of characters
	const uint32_t charCount = txiFeatures.numChars;
	if (charCount == 0)
		throw Common::Exception("Texture defines no characters");

	// Validate the data
	if (txiFeatures.numCharsPerSheet == 0)
		throw Common::Exception("Double byte texture font has no chars per sheet");
	if (txiFeatures.rows == 0)
		throw Common::Exception("Font rows is missing");
	if (txiFeatures.cols == 0)
		throw Common::Exception("Font cols is missing");
	if (txiFeatures.cols * txiFeatures.rows != txiFeatures.numCharsPerSheet)
		throw Common::Exception("Font has mismatched number of characters per sheet");
	if (txiFeatures.fileRange > 16) // Sanity check
		throw Common::Exception("Too many font sheets %u", static_cast<unsigned int>(txiFeatures.fileRange));
	if (txiFeatures.numCharsPerSheet * txiFeatures.fileRange < charCount)
		throw Common::Exception("Not enough font sheets available");
	if (txiFeatures.fontWidth == 0.0f)
		throw Common::Exception("Font width is missing");
	if (txiFeatures.fontHeight == 0.0f)
		throw Common::Exception("Font height is missing");

	// Load each of the textures
	for (unsigned int i = 0; i < txiFeatures.fileRange; i++)
		_textures.push_back(TextureMan.get(Common::String::format("%s%u", name.c_str(), i)));

	const Texture &texture0 = _textures.front().getTexture();

	if ((texture0.getWidth() == 0) || (texture0.getHeight() == 0))
		throw Common::Exception("Invalid texture dimensions (%dx%d)", texture0.getWidth(), texture0.getHeight());

	const double textureRatio = ((double) texture0.getWidth()) / ((double) texture0.getHeight());

	// Get features
	_height = txiFeatures.fontHeight * 100.0f;
	_spaceR = txiFeatures.spacingR   * 100.0f;
	_spaceB = txiFeatures.spacingB   * 100.0f;

	// FIXME: Support with multiple textures (this does not currently compile)
#if 0
	_mesh = static_cast<Mesh::MeshFont *>(MeshMan.getMesh("defaultMeshFont"));
	_material.reset(new Shader::ShaderMaterial(ShaderMan.getShaderObject("default/text.frag", Shader::SHADER_FRAGMENT), "text"));
	Shader::ShaderSampler *sampler;
	sampler = (Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	sampler->handle = _texture;
	_renderable.reset(new Shader::ShaderRenderable(SurfaceMan.getSurface("textSurface"), _material.get(), _mesh));
#endif

	const Common::Encoding encoding = mapCodePage(txiFeatures.codepage);

	// Build the character texture and vertex coordinates
	for (uint32_t i = 0; i < txiFeatures.dbMapping.size(); i++) {
		const uint16_t charIndex = txiFeatures.dbMapping[i];
		if (charIndex == 0)
			continue;

		if (charIndex > charCount)
			throw Common::Exception("Found character %u > count %u", charIndex, charCount);

		std::pair<std::map<uint32_t, Char>::iterator, bool> result;

		result = _chars.insert(std::make_pair(convertToUTF32(i >> 8, i & 0xFF, encoding), Char()));
		if (!result.second)
			continue;

		Char &c = result.first->second;

		c.sheet                         = charIndex / txiFeatures.numCharsPerSheet;
		const uint16_t charIndexInSheet = charIndex % txiFeatures.numCharsPerSheet;

		const double height = txiFeatures.fontHeight / txiFeatures.textureWidth;
		const double width  = txiFeatures.fontWidth / txiFeatures.textureWidth;

		// Calculate the row/col
		const uint16_t row = charIndexInSheet / txiFeatures.cols;
		const uint16_t col = charIndexInSheet % txiFeatures.cols;

		// Texture coordinates, calculated
		c.tX[0] = col * (txiFeatures.fontWidth / txiFeatures.textureWidth);
		c.tY[0] = 1.0f - ((row + 1) * (txiFeatures.fontHeight / txiFeatures.textureWidth));

		c.tX[1] = (col + 1) * (txiFeatures.fontWidth / txiFeatures.textureWidth);
		c.tY[1] = 1.0f - ((row + 1) * (txiFeatures.fontHeight / txiFeatures.textureWidth));

		c.tX[2] = (col + 1) * (txiFeatures.fontWidth / txiFeatures.textureWidth);
		c.tY[2] = 1.0f - (row * (txiFeatures.fontHeight / txiFeatures.textureWidth));

		c.tX[3] = col * (txiFeatures.fontWidth / txiFeatures.textureWidth);
		c.tY[3] = 1.0f - (row * (txiFeatures.fontHeight / txiFeatures.textureWidth));

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

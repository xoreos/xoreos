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

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/resman.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"
#include "src/graphics/aurora/abcfont.h"

#include "src/graphics/mesh/meshman.h"
#include "src/graphics/shader/surfaceman.h"

namespace Graphics {

namespace Aurora {

ABCFont::ABCFont(const Common::UString &name) : _base(0) {
	_texture = TextureMan.get(name);

	load(name);

	_mesh = static_cast<Mesh::MeshFont *>(MeshMan.getMesh("defaultMeshFont"));
	_material = new Shader::ShaderMaterial(ShaderMan.getShaderObject("default/text.frag", Shader::SHADER_FRAGMENT), "text");
	Shader::ShaderSampler *sampler;
	sampler = (Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	sampler->handle = _texture;
	_renderable = new Shader::ShaderRenderable(SurfaceMan.getSurface("textSurface"), _material, _mesh);
}

ABCFont::~ABCFont() {
	delete _renderable;
	delete _material;
}

float ABCFont::getHeight() const {
	return 32.0f;
}

float ABCFont::getWidth(uint32 c) const {
	const Char &cC = findChar(c);

	return cC.spaceL + cC.width + cC.spaceR;
}

void ABCFont::draw(uint32 c) const {
	TextureMan.set(_texture);

	const Char &cC = findChar(c);

	glTranslatef(cC.spaceL, 0.0f, 0.0f);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC.tX[i], cC.tY[i]);
		glVertex2f  (cC.vX[i], cC.vY[i]);
	}
	glEnd();

	glTranslatef(cC.width + cC.spaceR, 0.0f, 0.0f);
}

void ABCFont::renderBind(const glm::mat4 &transform) const {
	glUseProgram(_renderable->getProgram()->glid);
	_material->bindProgram(_renderable->getProgram(), 1.0f);
	_material->bindGLState();
	_renderable->getSurface()->bindProgram(_renderable->getProgram(), &transform);
	_renderable->getSurface()->bindGLState();
	_mesh->renderBind();
}

void ABCFont::render(uint32 c, float &x, float &y, float *rgba) const {
	const Char &cC = findChar(c);

	x += cC.spaceL;

	float v_pos[12];
	float v_uv[8];
	float v_rgba[4*4];

	for (int i = 0; i < 4; ++i) {
		v_uv[i*2] = cC.tX[i];
		v_uv[i*2 +1] = cC.tY[i];
		v_pos[i*3] = x + cC.vX[i];
		v_pos[i*3 +1] = y + cC.vY[i];
		v_pos[i*3 +2] = 0.0f;
		v_rgba[i*4] = rgba[0];
		v_rgba[i*4 +1] = rgba[1];
		v_rgba[i*4 +2] = rgba[2];
		v_rgba[i*4 +3] = rgba[3];
	}
	_mesh->render(v_pos, v_uv, v_rgba);
	x += cC.width + cC.spaceR;
}

void ABCFont::renderUnbind() const {
	_mesh->renderUnbind();

	_renderable->getSurface()->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void ABCFont::load(const Common::UString &name) {
	Common::ScopedPtr<Common::SeekableReadStream> abc(ResMan.getResource(name, ::Aurora::kFileTypeABC));
	if (!abc)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	// Init the invalid char
	_invalid.dataX  = 0;
	_invalid.dataY  = 0;
	_invalid.width  = 0;
	_invalid.spaceL = 0;
	_invalid.spaceR = 0;

	_invalid.tX[0] = 0.0f; _invalid.tY[0] = 0.0f;
	_invalid.tX[1] = 0.0f; _invalid.tY[1] = 0.0f;
	_invalid.tX[2] = 0.0f; _invalid.tY[2] = 0.0f;
	_invalid.tX[3] = 0.0f; _invalid.tY[3] = 0.0f;
	_invalid.vX[0] = 0.0f; _invalid.vY[0] = 0.0f;
	_invalid.vX[1] = 0.0f; _invalid.vY[1] = 0.0f;
	_invalid.vX[2] = 0.0f; _invalid.vY[2] = 0.0f;
	_invalid.vX[3] = 0.0f; _invalid.vY[3] = 0.0f;

	bool hasInvalid = false;

	if (abc->size() != 524280)
		throw Common::Exception("Invalid font (%u)", (uint)abc->size());

	_base = abc->readByte();

	abc->skip(7); // Probably random garbage

	// Read the ASCII character
	for (int i = 1; i < 128; i++) {
		Char &c = _ascii[i];

		readCharDesc(c, *abc);
		calcCharVertices(c);

		// Points to the "invalid character"
		if (!hasInvalid && (c.dataX == 0) && (c.dataY == 0)) {
			_invalid   = c;
			hasInvalid = true;
		}
	}

	// Read the UTF16 extended characters
	for (int i = 128; i < 65535; i++) {
		Char c;

		readCharDesc(c, *abc);

		// Points to the "invalid character"
		if ((c.dataX == 0) && (c.dataY == 0)) {
			if (!hasInvalid) {
				calcCharVertices(c);
				_invalid   = c;
				hasInvalid = true;
			}

			continue;
		}

		calcCharVertices(c);
		_extended.insert(std::make_pair(Common::UString::fromUTF16((uint16) i), c));
	}
}

void ABCFont::readCharDesc(Char &c, Common::SeekableReadStream &abc) {
	uint32 offset = abc.readUint32LE();
	byte   plane  = abc.readByte();

	c.spaceL = abc.readByte();
	c.width  = abc.readByte();
	c.spaceR = abc.readByte();

	if (((offset % 1024) != 0) || (plane > 3))
		throw Common::Exception("Invalid char data (%d, %d)", offset, plane);

	c.dataX =  plane          * 32;
	c.dataY = (offset / 1024) * 32;
}

void ABCFont::calcCharVertices(Char &c) {
	const float w = _texture.getTexture().getWidth();
	const float h = _texture.getTexture().getHeight();

	const float vW = c.width;
	const float vH = 32.0f;
	const float tW = vW / w;
	const float tH = vH / h;

	const float tX = c.dataX / w;
	const float tY = c.dataY / h;

	c.tX[0] = tX     ; c.tY[0] = tY + tH;
	c.tX[1] = tX + tW; c.tY[1] = tY + tH;
	c.tX[2] = tX + tW; c.tY[2] = tY     ;
	c.tX[3] = tX     ; c.tY[3] = tY     ;

	c.vX[0] = 0.0f     ; c.vY[0] = 0.0f     ;
	c.vX[1] = 0.0f + vW; c.vY[1] = 0.0f     ;
	c.vX[2] = 0.0f + vW; c.vY[2] = 0.0f + vH;
	c.vX[3] = 0.0f     ; c.vY[3] = 0.0f + vH;
}

const ABCFont::Char &ABCFont::findChar(uint32 c) const {
	if (Common::UString::isASCII(c))
		return _ascii[c];

	std::map<uint32, Char>::const_iterator ch = _extended.find(c);
	if (ch == _extended.end())
		return _invalid;

	return ch->second;
}

} // End of namespace Aurora

} // End of namespace Graphics

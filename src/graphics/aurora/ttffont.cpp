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
 *  A TrueType font.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"

#include "src/aurora/resman.h"

#include "src/graphics/texture.h"
#include "src/graphics/ttf.h"

#include "src/graphics/images/surface.h"

#include "src/graphics/aurora/ttffont.h"
#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/graphics/mesh/meshman.h"
#include "src/graphics/shader/surfaceman.h"

static const uint32_t kPageWidth  = 256;
static const uint32_t kPageHeight = 256;

namespace Graphics {

namespace Aurora {

TTFFont::Page::Page() : needRebuild(false),
		curX(0), curY(0), heightLeft(kPageHeight), widthLeft(kPageWidth) {

	surface = new Surface(kPageWidth, kPageHeight);
	surface->fill(0x00, 0x00, 0x00, 0x00);

	texture = TextureMan.add(Texture::create(surface));
}

void TTFFont::Page::rebuild() {
	if (!needRebuild)
		return;

	texture.getTexture().rebuild();
	needRebuild = false;
}


TTFFont::TTFFont(Common::SeekableReadStream *ttf, int height) {
	load(ttf, height);
}

TTFFont::TTFFont(const Common::UString &name, int height) {
	Common::SeekableReadStream *ttf = ResMan.getResource(name, ::Aurora::kFileTypeTTF);
	if (!ttf)
		throw Common::Exception("No such font \"%s\"", name.c_str());

	load(ttf, height);
}

TTFFont::~TTFFont() {
	delete _renderable;
	delete _material;
}

void TTFFont::load(Common::SeekableReadStream *ttf, int height) {
	std::unique_ptr<Common::SeekableReadStream> ttfStream(ttf);

	_ttf = std::make_unique<TTFRenderer>(*ttfStream, height);

	_height = _ttf->getHeight();
	if (_height > kPageHeight)
		throw Common::Exception("Font height too big (%d)", _height);

	// Add all ASCII characters
	for (uint32_t i = 0; i < 128; i++)
		addChar(i);

	// Add the Unicode "replacement character" character
	addChar(0xFFFD);
	_missingChar = _chars.find(0xFFFD);

	// Find an appropriate width for a "missing character" character
	if (_missingChar == _chars.end()) {
		// This font doesn't have the Unicode "replacement character"

		// Try to find the width of an m. Alternatively, take half of a line's height.
		std::map<uint32_t, Char>::const_iterator m = _chars.find('m');
		if (m != _chars.end())
			_missingWidth = m->second.width;
		else
			_missingWidth = MAX<float>(2.0f, _height / 2);

	} else
		_missingWidth = _missingChar->second.width;

	rebuildPages();

	_mesh = static_cast<Mesh::MeshFont *>(MeshMan.getMesh("defaultMeshFont"));
	_material = new Shader::ShaderMaterial(ShaderMan.getShaderObject("default/text.frag", Shader::SHADER_FRAGMENT), "text");
	Shader::ShaderSampler *sampler;
	sampler = (Shader::ShaderSampler *)(_material->getVariableData("sampler_0_id"));
	if (_pages.size() > 0) {
		// At the moment this doesn't matter too much - just need a valid texture of some kind.
		sampler->handle = _pages[0]->texture;
	}
	_renderable = new Shader::ShaderRenderable(SurfaceMan.getSurface("textSurface"), _material, _mesh);
}

float TTFFont::getWidth(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end())
		return _missingWidth;

	return cC->second.width;
}

float TTFFont::getHeight() const {
	return _height;
}

void TTFFont::drawMissing() const {
	TextureMan.set();

	const float width = _missingWidth - 1.0f;

	glBegin(GL_QUADS);
		glVertex2f(0.0f ,    0.0f);
		glVertex2f(width,    0.0f);
		glVertex2f(width, _height);
		glVertex2f(0.0f , _height);
	glEnd();

	glTranslatef(width + 1.0f, 0.0f, 0.0f);
}

void TTFFont::draw(uint32_t c) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);
	if (cC == _chars.end()) {
		cC = _missingChar;

		if (cC == _chars.end()) {
			drawMissing();
			return;
		}
	}

	size_t page = cC->second.page;
	assert(page < _pages.size());

	TextureMan.set(_pages[page]->texture);

	glBegin(GL_QUADS);
	for (int i = 0; i < 4; i++) {
		glTexCoord2f(cC->second.tX[i], cC->second.tY[i]);
		glVertex2f  (cC->second.vX[i], cC->second.vY[i]);
	}
	glEnd();

	glTranslatef(cC->second.width, 0.0f, 0.0f);
}

void TTFFont::buildChars(const Common::UString &str) {
	for (Common::UString::iterator c = str.begin(); c != str.end(); ++c)
		addChar(*c);

	rebuildPages();
}

void TTFFont::renderBind(const glm::mat4 &transform) const {
	glUseProgram(_renderable->getProgram()->glid);
	_material->bindProgram(_renderable->getProgram(), 1.0f);
	_material->bindGLState();
	_renderable->getSurface()->bindProgram(_renderable->getProgram(), &transform);
	_renderable->getSurface()->bindGLState();
	_mesh->renderBind();

	/* Mesh data will be dynamically updated for each character drawn to the screen
	 * (at least for now), so make sure the mesh VBO is bound. This isn't required
	 * for GL2.1, but is for GL3.2 because using the VAO doesn't automatically bind
	 * the VBO for data updates.
	 */
	glBindBuffer(GL_ARRAY_BUFFER, _mesh->getVertexBuffer()->getVBO());
}

void TTFFont::render(uint32_t c, float &x, float &y, float *rgba) const {
	std::map<uint32_t, Char>::const_iterator cC = _chars.find(c);

	if (cC == _chars.end()) {
		// Nothing is rendered for missing characters. Maybe one day use a placeholder instead.
		x += _missingWidth - 1.0f;
		return;
	}


	float v_pos[12];
	float v_uv[8];
	float v_rgba[4*4];

	size_t page = cC->second.page;
	assert(page < _pages.size());

	/* The assumption here is that the material will have been bound, and either only
	 * constains the single sampler, or the sampler was bound last. In either case, the
	 * active texture unit is properly set and the appropriate texture can be easily
	 * bound on the fly.
	 * todo: It would be better if the textures were place entirely into one large texture
	 * and the UV coordinates offset as appropriate. It's also likely better if instead of
	 * multiple render calls here, if instead everything can be batched together into a
	 * single render call.
	 */
	TextureMan.set(_pages[page]->texture);

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
	x += cC->second.width;
}

void TTFFont::renderUnbind() const {
	_mesh->renderUnbind();

	_renderable->getSurface()->unbindGLState();
	_material->unbindGLState();
	glUseProgram(0);
}

void TTFFont::rebuildPages() {
	for (std::vector<Page *>::iterator p = _pages.begin(); p != _pages.end(); ++p)
		(*p)->rebuild();
}

void TTFFont::addChar(uint32_t c) {
	std::map<uint32_t, Char>::iterator cC = _chars.find(c);
	if (cC != _chars.end())
		return;

	if (!_ttf->hasChar(c))
		return;

	try {

		uint32_t cWidth = _ttf->getCharWidth(c);
		if (cWidth > kPageWidth)
			return;

		if (_pages.empty()) {
			_pages.push_back(new Page);
			_pages.back()->heightLeft -= _height;
		}

		if (_pages.back()->widthLeft < cWidth) {
			// The current character doesn't fit into the current line

			if (_pages.back()->heightLeft >= _height) {
				// Create a new line

				_pages.back()->curX  = 0;
				_pages.back()->curY += _height;

				_pages.back()->heightLeft -= _height;
				_pages.back()->widthLeft   = kPageWidth;

			} else {
				// Create a new page

				_pages.push_back(new Page);
				_pages.back()->heightLeft -= _height;
			}

		}

		_ttf->drawCharacter(c, *_pages.back()->surface, _pages.back()->curX, _pages.back()->curY);

		std::pair<std::map<uint32_t, Char>::iterator, bool> result;

		result = _chars.insert(std::make_pair(c, Char()));

		cC = result.first;

		Char &ch   = cC->second;
		Page &page = *_pages.back();

		ch.width = cWidth;
		ch.page  = _pages.size() - 1;

		ch.vX[0] = 0.00f;  ch.vY[0] = 0.00f;
		ch.vX[1] = cWidth; ch.vY[1] = 0.00f;
		ch.vX[2] = cWidth; ch.vY[2] = _height;
		ch.vX[3] = 0.00f;  ch.vY[3] = _height;

		const float tX = (float) page.curX / (float) kPageWidth;
		const float tY = (float) page.curY / (float) kPageHeight;
		const float tW = (float) cWidth    / (float) kPageWidth;
		const float tH = (float) _height   / (float) kPageHeight;

		ch.tX[0] = tX;      ch.tY[0] = tY + tH;
		ch.tX[1] = tX + tW; ch.tY[1] = tY + tH;
		ch.tX[2] = tX + tW; ch.tY[2] = tY;
		ch.tX[3] = tX;      ch.tY[3] = tY;

		_pages.back()->widthLeft  -= cWidth;
		_pages.back()->curX       += cWidth;
		_pages.back()->needRebuild = true;

	} catch (...) {
		if (cC != _chars.end())
			_chars.erase(cC);

		Common::exceptionDispatcherWarning();
	}
}

} // End of namespace Aurora

} // End of namespace Graphics

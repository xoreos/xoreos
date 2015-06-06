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
 *  The scrolling background image panel.
 */

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/stream.h"
#include "src/common/maths.h"

#include "src/aurora/resman.h"

#include "src/graphics/camera.h"

#include "src/graphics/images/cbgt.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/engines/sonic/areabackground.h"

static const float kWidth  = 256.0f;
static const float kHeight = 192.0f;

static const float kVertexX1 = -128.0f;
static const float kVertexY1 = -192.0f;
static const float kVertexX2 = kVertexX1 + kWidth;
static const float kVertexY2 = kVertexY1 + kHeight;

namespace Engines {

namespace Sonic {

AreaBackground::AreaBackground(const Common::UString &name) {
	_distance = FLT_MAX;

	loadTexture(name);

	setPosition(0.0f, 0.0f, 0.0f);
}

AreaBackground::~AreaBackground() {
	hide();
}

uint32 AreaBackground::getImageWidth() const {
	return _texture.getTexture().getWidth();
}

uint32 AreaBackground::getImageHeight() const {
	return _texture.getTexture().getHeight();
}

void AreaBackground::calculateDistance() {
}

void AreaBackground::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassTransparent)
		return;

	TextureMan.set(_texture);

	glBegin(GL_QUADS);
		glTexCoord2f(_textureX1, _textureY1);
		glVertex2f(kVertexX1, kVertexY1);
		glTexCoord2f(_textureX2, _textureY1);
		glVertex2f(kVertexX2, kVertexY1);
		glTexCoord2f(_textureX2, _textureY2);
		glVertex2f(kVertexX2, kVertexY2);
		glTexCoord2f(_textureX1, _textureY2);
		glVertex2f(kVertexX1, kVertexY2);
	glEnd();
}

void AreaBackground::loadTexture(const Common::UString &name) {
	Common::SeekableReadStream *cbgt = 0, *pal = 0, *twoda = 0;
	Graphics::CBGT *image = 0;

	try {
		if (!(cbgt  = ResMan.getResource(name, Aurora::kFileTypeCBGT)))
			throw Common::Exception("No such CBGT");
		if (!(pal   = ResMan.getResource(name, Aurora::kFileTypePAL)))
			throw Common::Exception("No such PAL");
		if (!(twoda = ResMan.getResource(name, Aurora::kFileType2DA)))
			throw Common::Exception("No such 2DA");

		image    = new Graphics::CBGT(*cbgt, *pal, *twoda);
		_texture = TextureMan.add(Graphics::Aurora::Texture::create(image, Aurora::kFileTypeCBGT), name);

	} catch (Common::Exception &e) {
		delete image;
		delete cbgt;
		delete pal;
		delete twoda;

		e.add("Failed loading area background \"%s\"", name.c_str());
		throw;
	}

	delete cbgt;
	delete pal;
	delete twoda;
}

void AreaBackground::getCameraLimits(float z, float &minX, float &minY, float &minZ,
                                              float &maxX, float &maxY, float &maxZ) const {

	const float tWidth  = _texture.getTexture().getWidth();
	const float tHeight = _texture.getTexture().getHeight();

	minX = kWidth / 2.0f;
	minY = z;
	minZ = -(tHeight - kHeight / 2.0f + z);

	maxX = tWidth - kWidth / 2.0f;
	maxY = z;
	maxZ = -(kHeight/ 2.0f + z);
}

void AreaBackground::getCameraPosition(float x, float y, float z,
                                       float &cameraX, float &cameraY, float &cameraZ) const {
	cameraX = x;
	cameraY = z;
	cameraZ = y - z;
}

void AreaBackground::setPosition(float x, float y, float z) {
	const float tWidth  = _texture.getTexture().getWidth();
	const float tHeight = _texture.getTexture().getHeight();

	x = CLIP<float>(x     - kWidth  / 2.0f, 0.0f, tWidth  - kWidth);
	y = CLIP<float>(y - z - kHeight / 2.0f, 0.0f, tHeight - kHeight);

	_textureX1 =  x            / tWidth;
	_textureY1 = (y + kHeight) / tHeight;
	_textureX2 = (x + kWidth ) / tWidth;
	_textureY2 =  y            / tHeight;
}

void AreaBackground::notifyCameraMoved() {
	const float *pos = CameraMan.getPosition();

	setPosition(pos[0], -pos[2], pos[1]);
}

} // End of namespace Sonic

} // End of namespace Engines

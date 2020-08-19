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
 *  The scrolling background image panel in Sonic Chronicles: The Dark Brotherhood.
 */

#include <memory>

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/error.h"
#include "src/common/readstream.h"
#include "src/common/maths.h"

#include "src/aurora/resman.h"

#include "src/graphics/camera.h"

#include "src/graphics/images/cbgt.h"

#include "src/graphics/aurora/textureman.h"
#include "src/graphics/aurora/texture.h"

#include "src/engines/sonic/areabackground.h"
#include "src/engines/sonic/types.h"

namespace Engines {

namespace Sonic {

static const float kVertexX1 = kBottomScreenX;
static const float kVertexY1 = kBottomScreenY;
static const float kVertexX2 = kVertexX1 + kScreenWidth;
static const float kVertexY2 = kVertexY1 + kScreenHeight;

AreaBackground::AreaBackground(const Common::UString &name) :
	Graphics::GUIElement(Graphics::GUIElement::kGUIElementBack) {
	_distance = FLT_MAX;

	loadTexture(name);

	setPosition(0.0f, 0.0f);
}

AreaBackground::~AreaBackground() {
	hide();
}

uint32_t AreaBackground::getImageWidth() const {
	return _texture.getTexture().getWidth();
}

uint32_t AreaBackground::getImageHeight() const {
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
	try {
		std::unique_ptr<Common::SeekableReadStream> cbgt(ResMan.getResource(name, Aurora::kFileTypeCBGT));
		if (!cbgt)
			throw Common::Exception("No such CBGT");

		std::unique_ptr<Common::SeekableReadStream> pal(ResMan.getResource(name, Aurora::kFileTypePAL));
		if (!pal)
			throw Common::Exception("No such PAL");

		std::unique_ptr<Common::SeekableReadStream> twoda(ResMan.getResource(name, Aurora::kFileType2DA));
		if (!twoda)
			throw Common::Exception("No such 2DA");

		std::unique_ptr<Graphics::CBGT> image = std::make_unique<Graphics::CBGT>(*cbgt, *pal, *twoda);

		_texture = TextureMan.add(Graphics::Aurora::Texture::create(image.get(), Aurora::kFileTypeCBGT), name);
		image.release();

	} catch (Common::Exception &e) {
		e.add("Failed loading area background \"%s\"", name.c_str());
		throw;
	}
}

void AreaBackground::setPosition(float x, float y) {
	const float textureWidth  = _texture.getTexture().getWidth();
	const float textureHeight = _texture.getTexture().getHeight();

	y = floor(y * sin(Common::deg2rad(kCameraAngle)));

	x = CLIP<float>(x - kScreenWidth  / 2.0f, 0.0f, textureWidth  - kScreenWidth);
	y = CLIP<float>(y - kScreenHeight / 2.0f, 0.0f, textureHeight - kScreenHeight);

	_textureX1 =  x                  / textureWidth;
	_textureY1 = (y + kScreenHeight) / textureHeight;
	_textureX2 = (x + kScreenWidth ) / textureWidth;
	_textureY2 =  y                  / textureHeight;
}

void AreaBackground::notifyCameraMoved() {
	const float *pos = CameraMan.getPosition();

	setPosition(pos[0], pos[2]);
}

} // End of namespace Sonic

} // End of namespace Engines

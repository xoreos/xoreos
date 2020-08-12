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
 *  The menu background.
 */

#include <cfloat>

#include "src/common/debug.h"
#include "src/common/util.h"
#include "src/common/ustring.h"

#include "src/aurora/resman.h"
#include "src/aurora/types.h"

#include "src/graphics/graphics.h"
#include "src/graphics/resolution.h"
#include "src/graphics/aurora/textureman.h"

#include "src/engines/jade/gui/guibackground.h"

namespace Engines {

namespace Jade {

GUIBackground::GUIBackground(const Common::UString &type, bool front) :
	Graphics::GUIElement(front ? Graphics::GUIElement::kGUIElementFront : Graphics::GUIElement::kGUIElementBack), _type(type) {

	_distance = FLT_MAX;

	_screenWidth = WindowMan.getWindowWidth();
	_screenHeight = WindowMan.getWindowHeight();

	update();
}

GUIBackground::~GUIBackground() {
	hide();
}

void GUIBackground::setType(const Common::UString &type) {
	_type = type;
}

void GUIBackground::calculateDistance() {
}

void GUIBackground::render(Graphics::RenderPass pass) {
	if (pass == Graphics::kRenderPassTransparent || !_render)
		return;

	TextureMan.set(_texture);

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0.0, _screenWidth, 0.0, _screenHeight, -1.0, 1.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glPushMatrix();

	glBegin(GL_QUADS);
		glTexCoord2f(0.0, 0.0);
		glVertex2i(_vertexX1, _vertexY1);
		glTexCoord2f(1.0, 0.0);
		glVertex2i(_vertexX2, _vertexY1);
		glTexCoord2f(1.0, 1.0);
		glVertex2i(_vertexX2, _vertexY2);
		glTexCoord2f(0.0, 1.0);
		glVertex2i(_vertexX1, _vertexY2);
	glEnd();

	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);

	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
}

void GUIBackground::update() {
	if (tryBackground(_screenWidth, _screenHeight)) {
		return;
	}

	for (size_t i = 0; i < ARRAYSIZE(Graphics::kResolutions); i++) {
		if (Graphics::kResolutions[i].width <= _screenWidth && Graphics::kResolutions[i].height <= _screenHeight) {
			if (tryBackground(Graphics::kResolutions[i].width, Graphics::kResolutions[i].height)) {
				return;
			}
		}
	}
}

bool GUIBackground::tryBackground(int width, int height) {
	Common::UString name = Common::String::format("%ux%u%s", width, height, _type.c_str());
	if (ResMan.hasResource(name, Aurora::kResourceImage)) {
		_texture = TextureMan.get(name);

		_vertexX1 = (_screenWidth - width) / 2;
		_vertexX2 = _vertexX1 + width;
		_vertexY1 = (_screenHeight - height) / 2;
		_vertexY2 = _vertexY1 + height;

		_render = true;
		debugC(Common::kDebugEngineGraphics, 1, "found background %s", _texture.getName().c_str());
	} else {
		_render = false;
		debugC(Common::kDebugEngineGraphics, 1, "no background %s", name.c_str());
	}
	return _render;
}

void GUIBackground::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight), int newWidth, int newHeight) {
	_screenHeight = newHeight;
	_screenWidth = newWidth;

	update();
}

} // End of namespace Jade

} // End of namespace Engines

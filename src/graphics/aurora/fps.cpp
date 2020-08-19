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
 *  A text object displaying the current FPS.
 */

#include "src/common/system.h"
#include "src/common/ustring.h"

#include "src/graphics/graphics.h"
#include "src/graphics/font.h"

#include "src/graphics/aurora/fps.h"

namespace Graphics {

namespace Aurora {

FPS::FPS(const FontHandle &font) : Text(Graphics::GUIElement::kGUIElementConsole, font, WindowMan.getWindowWidth(), WindowMan.getWindowHeight(), "0 fps"), _fps(0) {
	init();
}

FPS::~FPS() {
	hide();
}

void FPS::init() {
	setTag("FPS");
	notifyResized(0, 0, WindowMan.getWindowWidth(), WindowMan.getWindowHeight());
}

void FPS::render(RenderPass pass) {
	// Text objects should always be transparent
	if (pass == kRenderPassOpaque)
		return;

	uint32_t fps = GfxMan.getFPS();

	if (fps != _fps) {
		_fps = fps;

		setText(Common::UString::format("%d fps", _fps));
	}

	Text::render(pass);
}

void FPS::renderImmediate(const glm::mat4 &parentTransform) {
	uint32_t fps = GfxMan.getFPS();

	if (fps != _fps) {
		_fps = fps;

		setText(Common::UString::format("%d fps", _fps));
	}

	Text::renderImmediate(parentTransform);
}

void FPS::notifyResized(int UNUSED(oldWidth), int UNUSED(oldHeight), int newWidth, int newHeight) {
	float posX = -(newWidth  / 2.0f);
	float posY = -(newHeight / 2.0f);

	setPosition(posX, posY);
	setSize(newWidth, newHeight);
}

} // End of namespace Aurora

} // End of namespace Graphics

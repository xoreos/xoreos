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

#ifndef GRAPHICS_AURORA_FPS_H
#define GRAPHICS_AURORA_FPS_H

#include "src/events/notifyable.h"

#include "src/graphics/aurora/text.h"

namespace Graphics {

namespace Aurora {

/** An autonomous FPS display. */
class FPS : public Text, public Events::Notifyable {
public:
	FPS(const FontHandle &font);
	~FPS();

	// Renderable
	void render(RenderPass pass);

	void renderImmediate(const glm::mat4 &parentTransform);

private:
	uint32_t _fps;

	void init();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_FPS_H

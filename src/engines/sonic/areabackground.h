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

#ifndef ENGINES_SONIC_AREABACKGROUND_H
#define ENGINES_SONIC_AREABACKGROUND_H

#include "src/graphics/guielement.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/events/notifyable.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace Sonic {

class AreaBackground : public Graphics::GUIElement, public Events::Notifyable {
public:
	AreaBackground(const Common::UString &name);
	~AreaBackground();

	uint32_t getImageWidth()  const;
	uint32_t getImageHeight() const;

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	Graphics::Aurora::TextureHandle _texture;

	float _textureX1;
	float _textureY1;
	float _textureX2;
	float _textureY2;

	void loadTexture(const Common::UString &name);
	void setPosition(float x, float y);

	void notifyCameraMoved();
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_AREABACKGROUND_H

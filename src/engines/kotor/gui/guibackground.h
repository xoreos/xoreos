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

#ifndef ENGINES_KOTOR_GUI_GUIBACKGROUND_H
#define ENGINES_KOTOR_GUI_GUIBACKGROUND_H

#include "src/graphics/guielement.h"

#include "src/graphics/aurora/texturehandle.h"

#include "src/events/notifyable.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace KotOR {

static const Common::UString &kBackgroundTypeMenu   = "back";
static const Common::UString &kBackgroundTypeComp0  = "comp0";
static const Common::UString &kBackgroundTypeComp1  = "comp1";
static const Common::UString &kBackgroundTypeLoad   = "load";
static const Common::UString &kBackgroundTypeMap    = "map";
static const Common::UString &kBackgroundTypePazaak = "pazaak";
static const Common::UString &kBackgroundTypeStore  = "store";

class GUIBackground : public Graphics::GUIElement, public Events::Notifyable {
public:
	GUIBackground(const Common::UString &type = kBackgroundTypeMenu, bool front = false);
	~GUIBackground();

	void setType(const Common::UString &type);

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	Graphics::Aurora::TextureHandle _texture;

	Common::UString _type;

	int _screenWidth;
	int _screenHeight;

	int _vertexX1;
	int _vertexX2;
	int _vertexY1;
	int _vertexY2;

	bool _render;

	void update();
	bool tryBackground(int x, int y);

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_GUI_GUIBACKGROUND_H

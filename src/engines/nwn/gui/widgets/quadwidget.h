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
 *  A NWN quad widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H

#include "src/common/scopedptr.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/nwn/gui/widgets/nwnwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class GUI;

/** A NWN quad widget.
 *
 *  One of the base NWN widget classes, the QuadWidget consists of a
 *  single Aurora GUIQuad.
 */
class QuadWidget : public NWNWidget {
public:
	QuadWidget(::Engines::GUI &gui, const Common::UString &tag,
	           const Common::UString &texture,
	           float  x1       , float  y1       , float  x2       , float  y2,
	           float tX1 = 0.0f, float tY1 = 0.0f, float tX2 = 1.0f, float tY2 = 1.0f);
	~QuadWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setColor(float r, float g, float b, float a);
	void setTexture(const Common::UString &texture);

	void setWidth (float w);
	void setHeight(float h);

	float getWidth () const;
	float getHeight() const;

private:
	float _width;
	float _height;

	Common::ScopedPtr<Graphics::Aurora::GUIQuad> _quad;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H

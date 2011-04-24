/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/gui/widgets/quadwidget.h
 *  A NWN quad widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"

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
	           float  x1      , float  y1      , float  x2      , float  y2,
	           float tX1 = 0.0, float tY1 = 0.0, float tX2 = 1.0, float tY2 = 1.0);
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

	Graphics::Aurora::GUIQuad *_quad;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H

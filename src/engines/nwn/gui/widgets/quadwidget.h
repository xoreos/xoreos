/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/quadwidget.h
 *  A NWN quad widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H
#define ENGINES_NWN_GUI_WIDGETS_QUADWIDGET_H

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

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
class QuadWidget : public Widget {
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

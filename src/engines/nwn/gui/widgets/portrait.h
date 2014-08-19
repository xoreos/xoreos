/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
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
 */

/** @file engines/nwn/gui/widgets/portrait.h
 *  A portrait model and widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H
#define ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H

#include "graphics/guifrontelement.h"

#include "graphics/aurora/textureman.h"

#include "engines/nwn/gui/widgets/nwnwidget.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN portrait model. */
class Portrait : public Graphics::GUIFrontElement {
public:
	enum Size {
		kSizeHuge   = 0,
		kSizeLarge     ,
		kSizeMedium    ,
		kSizeSmall     ,
		kSizeTiny      ,
		kSizeMAX
	};

	Portrait(const Common::UString &name, Size size, float border = 0.0,
	         float bR = 1.0, float bG = 1.0, float bB = 1.0, float bA = 1.0);
	~Portrait();

	float getWidth () const; ///< Get the scrollbar's width.
	float getHeight() const; ///< Get the scrollbar's height.

	/** Set the current portrait. */
	void setPortrait(const Common::UString &name);

	/** Set the current position of the portrait. */
	void setPosition(float x, float y, float z);

	/** Get the current position of the portrait. */
	void getPosition(float &x, float &y, float &z) const;

	/** Is the point within the portrait? */
	bool isIn(float x, float y) const;

	// Renderable
	void calculateDistance();
	void render(Graphics::RenderPass pass);

private:
	struct Quad {
		float vX[4], vY[4];
		float tX[4], tY[4];
	};

	Graphics::Aurora::TextureHandle _texture;

	Size _size;

	float _border;

	float _bR;
	float _bG;
	float _bB;
	float _bA;

	Quad _qPortrait;
	std::vector<Quad> _qBorder;

	void setSize();
	void createBorder();
};

/** A NWN portrait widget. */
class PortraitWidget : public NWNWidget {
public:
	PortraitWidget(::Engines::GUI &gui, const Common::UString &tag,
	               const Common::UString &name, Portrait::Size size, float border = 0.0,
	               float bR = 1.0, float bG = 1.0, float bB = 1.0, float bA = 1.0);
	~PortraitWidget();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	void setPortrait(const Common::UString &name);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

private:
	Portrait _portrait;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_PORTRAIT_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file graphics/aurora/guiquad.h
 *  A textured quad for a GUI element.
 */

#ifndef GRAPHICS_AURORA_GUIQUAD_H
#define GRAPHICS_AURORA_GUIQUAD_H

#include "common/maths.h"

#include "graphics/guifrontelement.h"

#include "graphics/aurora/textureman.h"

namespace Common {
	class UString;
}

namespace Graphics {

namespace Aurora {

class GUIQuad : public GUIFrontElement {
public:
	GUIQuad(const Common::UString &texture,
	        float  x1      , float  y1      , float  x2      , float  y2,
			    float tX1 = 0.0, float tY1 = 0.0, float tX2 = 1.0, float tY2 = 1.0);
	~GUIQuad();

	/** Set the current position of the quad. */
	void getPosition(float &x, float &y, float &z) const;
	/** Set the current position of the quad. */
	void setPosition(float x, float y, float z = -FLT_MAX);

	/** Set the current color of the quad. */
	void setColor(float r, float g, float b, float a);
	/** Set the current texture of the quad. */
	void setTexture(const Common::UString &texture);

	float getWidth () const; ///< Return the quad's width.
	float getHeight() const; ///< Return the quad's height.

	void setWidth (float w); ///< Set the quad's width.
	void setHeight(float h); ///< Set the quad's height.

	/** Is the point within the quad? */
	bool isIn(float x, float y) const;

	// Renderable
	void calculateDistance();
	void render(RenderPass pass);

private:
	TextureHandle _texture;

	float _x1;
	float _y1;
	float _x2;
	float _y2;

	float _tX1;
	float _tY1;
	float _tX2;
	float _tY2;

	float _r;
	float _g;
	float _b;
	float _a;
};

} // End of namespace Aurora

} // End of namespace Graphics

#endif // GRAPHICS_AURORA_GUIQUAD_H

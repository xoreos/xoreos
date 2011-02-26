/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/scrollbar.h
 *  The GUI scrollbar model.
 */

#ifndef ENGINES_NWN_MENU_SCROLLBAR_H
#define ENGINES_NWN_MENU_SCROLLBAR_H

#include <vector>

#include "graphics/guifrontelement.h"

#include "graphics/aurora/textureman.h"

namespace Engines {

namespace NWN {

class Scrollbar : public Graphics::GUIFrontElement {
public:
	enum Type {
		kTypeVertical,
		kTypeHorizontal
	};

	Scrollbar(Type type);
	~Scrollbar();

	/** Set the current position of the scrollbar. */
	void setPosition(float x, float y, float z);

	/** Is the point within the scrollbar? */
	bool isIn(float x, float y) const;

	void setLength(float length);

	void show(); ///< The scrollbar should be rendered.
	void hide(); ///< The scrollbar should not be rendered.

	bool isVisible(); ///< Is the scrollbar visible?

	float getWidth () const;
	float getHeight() const;

	// Renderable
	void newFrame();
	void render();

private:
	struct Quad {
		float vX[4], vY[4];
		float tX[4], tY[4];
	};

	Type _type;

	float _x;
	float _y;
	float _z;

	float _length;

	std::vector<Quad> _quads;

	Graphics::Aurora::TextureHandle _texture;

	void createV();
	void createH();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_SCROLLBAR_H

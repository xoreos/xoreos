/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/widgets/scrollbar.h
 *  A NWN scrollbar model and widget.
 */

#ifndef ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H
#define ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H

#include <vector>

#include "graphics/guifrontelement.h"

#include "graphics/aurora/textureman.h"

#include "engines/aurora/widget.h"

namespace Engines {

namespace NWN {

class GUI;

/** A NWN scrollbar model. */
class Scrollbar : public Graphics::GUIFrontElement {
public:
	enum Type {
		kTypeVertical,  ///< Vertial scrollbar.
		kTypeHorizontal ///< Horizontal scrollbar.
	};

	Scrollbar(Type type);
	~Scrollbar();

	/** Set the current position of the scrollbar. */
	void setPosition(float x, float y, float z);

	/** Get the current position of the scrollbar. */
	void getPosition(float &x, float &y, float &z) const;

	/** Is the point within the scrollbar? */
	bool isIn(float x, float y) const;

	/** Set the scrollbar length */
	void setLength(float length);

	void show(); ///< The scrollbar should be rendered.
	void hide(); ///< The scrollbar should not be rendered.

	bool isVisible(); ///< Is the scrollbar visible?

	float getWidth () const; ///< Get the scrollbar's width.
	float getHeight() const; ///< Get the scrollbar's height.

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

	void createV(); ///< Create a vertical scrollbar.
	void createH(); ///< Create a horizontal scrollbar.
};

/** A NWN scrollbar widget. */
class WidgetScrollbar : public Widget {
public:
	WidgetScrollbar(::Engines::GUI &gui, const Common::UString &tag,
	                Scrollbar::Type type, float range);
	~WidgetScrollbar();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	/** Set the length of the scrollbar, as a fraction of the range. */
	void setLength(float length);

	/** Get the current state, as a fraction of the range. */
	float getState() const;
	/** Set the current state, as a fraction of the range. */
	void setState(float state);

	float getWidth () const;
	float getHeight() const;

	float getBarPosition() const;

	void mouseDown(uint8 state, float x, float y);
	void mouseMove(uint8 state, float x, float y);

private:
	Scrollbar::Type _type;

	bool _full;

	float _range;
	float _length;
	float _state;

	float _dragX;
	float _dragY;
	float _dragState;

	Scrollbar _scrollbar;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_WIDGETS_SCROLLBAR_H

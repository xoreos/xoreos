/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/quickbar.h
 *  The ingame quickbar.
 */

#ifndef ENGINES_NWN_GUI_INGAME_QUICKBAR_H
#define ENGINES_NWN_GUI_INGAME_QUICKBAR_H

#include "common/types.h"

#include "events/notifyable.h"

#include "graphics/aurora/types.h"

#include "engines/aurora/widget.h"

#include "engines/nwn/gui/gui.h"

namespace Engines {

namespace NWN {

/** A button within the NWN quickbar. */
class QuickbarButton : public Widget {
public:
	QuickbarButton(::Engines::GUI &gui, uint n);
	~QuickbarButton();

	void show();
	void hide();

	void setPosition(float x, float y, float z);

	float getWidth () const;
	float getHeight() const;

	void setTag(const Common::UString &tag);

private:
	Graphics::Aurora::Model *_model;

	uint _buttonNumber;
};

/** The NWN ingame quickbar. */
class Quickbar : public GUI, public Events::Notifyable {
public:
	Quickbar();
	~Quickbar();

protected:
	void callbackActive(Widget &widget);

private:
	float _slotWidth;
	float _slotHeight;

	void getSlotSize();

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_QUICKBAR_H

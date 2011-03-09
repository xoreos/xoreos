/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/menu/partybar.h
 *  The ingame player bar.
 */

#ifndef ENGINES_NWN_MENU_PLAYERBAR_H
#define ENGINES_NWN_MENU_PLAYERBAR_H

#include "events/notifyable.h"

#include "engines/nwn/menu/gui.h"

namespace Engines {

namespace NWN {

/** The NWN ingame player bar. */
class Partybar : public GUI, public Events::Notifyable {
public:
	Partybar();
	~Partybar();

	/** Set the portrait image. */
	void setPortrait(const Common::UString &portrait);

	/** Set the health bar color. */
	void setHealthColor(float r, float g, float b, float a);
	/** Set the health bar length (as a fraction of "full health") */
	void setHealthLength(float length);

	/** Set the health bar color to "healthy" (red). */
	void setHealthHealthy();
	/** Set the health bar color to "sick" (brown). */
	void setHealthSick();
	/** Set the health bar color to "poisoned" (green). */
	void setHealthPoisoned();

protected:
	void callbackActive(Widget &widget);

private:
	NWNQuadWidget *_portrait;
	NWNQuadWidget *_health;

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_MENU_PLAYERBAR_H

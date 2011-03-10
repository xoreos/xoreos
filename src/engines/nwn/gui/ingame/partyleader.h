/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/partyleader.h
 *  The NWN ingame party leader panel.
 */

#ifndef ENGINES_NWN_GUI_INGAME_PARTYLEADER_H
#define ENGINES_NWN_GUI_INGAME_PARTYLEADER_H

#include "events/notifyable.h"

#include "engines/nwn/gui/ingame/charinfo.h"

namespace Engines {

namespace NWN {

/** The NWN ingame party leader bar. */
class PartyLeader : public CharacterInfo, public Events::Notifyable {
public:
	PartyLeader();
	~PartyLeader();

	/** Set the portrait image. */
	void setPortrait(const Common::UString &portrait);

	/** Set the health bar color. */
	void setHealthColor(float r, float g, float b, float a);
	/** Set the health bar length (as a fraction of "full health") */
	void setHealthLength(float length);

protected:
	void callbackActive(Widget &widget);

private:
	QuadWidget *_portrait;
	QuadWidget *_health;

	void notifyResized(int oldWidth, int oldHeight, int newWidth, int newHeight);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_PARTYLEADER_H

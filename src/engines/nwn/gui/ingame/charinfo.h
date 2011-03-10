/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/charinfo.h
 *  A NWN character info panel.
 */

#ifndef ENGINES_NWN_GUI_INGAME_CHARINFO_H
#define ENGINES_NWN_GUI_INGAME_CHARINFO_H

#include "engines/nwn/gui/gui.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

/** A NWN character info panel. */
class CharacterInfo : public GUI {
public:
	CharacterInfo();
	virtual ~CharacterInfo();

	/** Set the portrait image. */
	virtual void setPortrait(const Common::UString &portrait) = 0;

	/** Set the health bar color. */
	virtual void setHealthColor(float r, float g, float b, float a) = 0;
	/** Set the health bar length (as a fraction of "full health") */
	virtual void setHealthLength(float length) = 0;
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_CHARINFO_H

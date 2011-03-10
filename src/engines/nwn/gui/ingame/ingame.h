/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/gui/ingame/ingame.h
 *  The NWN ingame GUI elements.
 */

#ifndef ENGINES_NWN_GUI_INGAME_INGAME_H
#define ENGINES_NWN_GUI_INGAME_INGAME_H

#include <vector>

#include "common/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class IngameMainMenu;

class CharacterInfo;
class Quickbar;

/** The NWN ingame GUI elements. */
class IngameGUI {
public:
	IngameGUI();
	~IngameGUI();

	int showMain(); ///< Show the ingame main menu.

	void show(); ///< Show the ingame GUI elements.
	void hide(); ///< Hide the ingame GUI elements.

	/** Set the party member's portrait. */
	void setPortrait(uint partyMember, const Common::UString &portrait);

	/** Set the party member's health. */
	void setHealth(uint partyMember, float health);

	/** Set party member to "healthy" (red health bar). */
	void setHealthy (uint partyMember);
	/** Set party member to "sick" (brown health bar). */
	void setSick    (uint partyMember);
	/** Set party member to "poisoned" (green health bar). */
	void setPoisoned(uint partyMember);

private:
	IngameMainMenu *_main; ///< The ingame main menu.

	Quickbar *_quickbar; ///< The quick bar.

	std::vector<CharacterInfo *> _party; ///< The party member character panels.
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_INGAME_H

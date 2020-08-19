/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names
 * can be found in the AUTHORS file distributed with this source
 * distribution.
 *
 * xoreos is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * xoreos is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xoreos. If not, see <http://www.gnu.org/licenses/>.
 */

/** @file
 *  The NWN ingame GUI elements.
 */

#ifndef ENGINES_NWN_GUI_INGAME_INGAME_H
#define ENGINES_NWN_GUI_INGAME_INGAME_H

#include <vector>
#include <memory>

#include "src/common/types.h"
#include "src/common/ptrvector.h"

#include "src/events/types.h"

namespace Common {
	class UString;
}

namespace Engines {

namespace NWN {

class Module;

class IngameMainMenu;

class Object;
class Creature;

class CharacterInfo;
class Quickbar;
class Quickchat;
class Compass;
class Dialog;

/** The NWN ingame GUI elements. */
class IngameGUI {
public:
	IngameGUI(Module &module, ::Engines::Console *console = 0);
	~IngameGUI();

	uint32_t showMain(); ///< Show the ingame main menu.
	void  abortMain(); ///< Abort the ingame main menu.

	void show(); ///< Show the ingame GUI elements.
	void hide(); ///< Hide the ingame GUI elements.

	void addEvent(const Events::Event &event);
	void processEventQueue();

	/** Set the current area. */
	void setArea(const Common::UString &area);

	/** Update the party member. */
	void updatePartyMember(size_t partyMember, const Creature &creature, bool force = false);

	/** Do we have a currently running conversation? */
	bool hasRunningConversation() const;

	/** Start a conversation. */
	bool startConversation(const Common::UString &conv,
	                       Creature &pc, Object &obj, bool playHello = true);
	/** Stop a conversation. */
	void stopConversation();

private:
	Module *_module;

	std::unique_ptr<IngameMainMenu> _main; ///< The ingame main menu.

	std::unique_ptr<Quickbar>  _quickbar;  ///< The quick bar.
	std::unique_ptr<Quickchat> _quickchat; ///< The quick chat.
	std::unique_ptr<Compass>   _compass;   ///< The compass.

	std::unique_ptr<Dialog> _dialog; ///< The current dialog.

	/** The time the compass was changed last. */
	uint32_t _lastCompassChange;
	/** The time that party member was changed last. */
	std::vector<uint32_t> _lastPartyMemberChange;

	Common::PtrVector<CharacterInfo> _party; ///< The party member character panels.


	/** Set the party member's portrait. */
	void setPortrait(size_t partyMember, const Common::UString &portrait);

	/** Set the party member's name. */
	void setName(size_t partyMember, const Common::UString &name);

	/** Set the party member's health. */
	void setHealth(size_t partyMember, uint32_t current, uint32_t max);

	/** Set party member to "healthy" (red health bar). */
	void setHealthy (size_t partyMember);
	/** Set party member to "sick" (brown health bar). */
	void setSick    (size_t partyMember);
	/** Set party member to "poisoned" (green health bar). */
	void setPoisoned(size_t partyMember);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_GUI_INGAME_INGAME_H

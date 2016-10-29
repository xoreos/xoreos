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
 *  The context managing and running the Dragon Age: Origins campaigns.
 */

#ifndef ENGINES_DRAGONAGE_CAMPAIGNS_H
#define ENGINES_DRAGONAGE_CAMPAIGNS_H

#include <vector>
#include <list>

#include "src/common/scopedptr.h"
#include "src/common/ptrvector.h"
#include "src/common/ustring.h"

#include "src/events/types.h"

namespace Engines {

class Console;

namespace DragonAge {

class Game;
class Campaign;
class Creature;

class Campaigns {
public:
	Campaigns(::Engines::Console &console, Game &game);
	~Campaigns();

	// .--- Available campaigns and addins
	typedef Common::PtrVector<Campaign> PlayableCampaigns;
	typedef Common::PtrVector<Campaign> AddinContent;

	/** Return all playable campaigns. */
	const PlayableCampaigns &getCampaigns() const;
	/** Return all add-in content for other campaigns. */
	const AddinContent &getAddins() const;

	/** Find a specific campaign by UID and return it, or 0 if not found. */
	const Campaign *findCampaign(const Common::UString &uid) const;
	/** Find a specific add-in by UID and return it, or 0 if not found. */
	const Campaign *findAddin(const Common::UString &uid) const;
	// '---

	// .--- Campaign management
	/** Is a campaign currently loaded and ready to run? */
	bool isLoaded() const;
	/** Is a campaign currently running? */
	bool isRunning() const;

	/** Load a campaign. */
	void load(const Campaign &campaign);
	/** Use this character as the player character. */
	void usePC(Creature *pc);
	/** Exit the currently running campaign. */
	void exit();
	/** Completely unload the currently loaded campaign. */
	void unload();
	// '---

	// .--- The current campaign
	/** Return the currently running campaign. */
	Campaign *getCurrentCampaign() const;
	/** Return the currently playing PC. */
	Creature *getPC() const;
	// '---

	// .--- Interact with the current campaign
	/** Refresh all localized strings. */
	void refreshLocalized();
	// '---

	// .--- Campaign main loop (called by the Game class)
	/** Enter the loaded campaign, starting it. */
	void enter();
	/** Leave the running campaign, quitting it. */
	void leave();

	/** Add a single event for consideration into the event queue. */
	void addEvent(const Events::Event &event);
	/** Process the current event queue. */
	void processEventQueue();
	// '---

private:
	typedef std::list<Events::Event> EventQueue;


	::Engines::Console *_console;
	Game *_game;

	bool _hasCampaign; ///< Do we have a campaign?
	bool _running;     ///< Are we currently running a campaign?
	bool _exit;        ///< Should we exit the campaign?

	/** All campaigns we know about. */
	PlayableCampaigns _campaigns;
	/** All add-in content we know about. */
	AddinContent _addins;

	/** The currently loaded campaign. */
	Campaign *_currentCampaign;

	/** The player character we use. */
	Common::ScopedPtr<Creature> _pc;

	/** The UID of the campaign we should change to. */
	Common::UString _newCampaign;

	EventQueue _eventQueue;


	void clean();

	/** Unload the whole shebang.
	 *
	 *  @param completeUnload Also unload the PC
	 *                        true:  completely quit the current campaign.
	 *                        false: the PC can be transferred to a new campaign.
	 */
	void unload(bool completeUnload);

	void unloadPC();

	/** Load a new campaign. */
	void loadCampaign(const Campaign &campaign);
	/** Schedule a change to a new campaign. */
	void changeCampaign(const Campaign &campaign);
	/** Actually replace the currently running campaign. */
	void replaceCampaign();

	void findCampaigns();
	void addCampaign(Campaign *campaign);
	Campaign *readCampaign(const Common::UString &cifPath = "", const Common::UString &manifestPath = "",
	                       const Common::UString &addinBase = "");

	Campaign *getCampaign(const Common::UString &uid);
	Campaign *getAddin(const Common::UString &uid);

	void handleEvents();
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_CAMPAIGNS_H

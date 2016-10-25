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
 *  The context holding a The Witcher campaign.
 */

#ifndef ENGINES_WITCHER_CAMPAIGN_H
#define ENGINES_WITCHER_CAMPAIGN_H

#include <list>

#include "src/common/scopedptr.h"
#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

#include "src/events/types.h"

namespace Common {
	class SeekableReadStream;
}

namespace Engines {

class Console;

namespace Witcher {

class Module;
class Creature;

class Campaign {
public:
	Campaign(::Engines::Console &console);
	~Campaign();

	/** Clear the whole context. */
	void clear();

	// .--- Campaign management
	/** Is a campaign currently loaded and ready to run? */
	bool isLoaded() const;
	/** Is a campaign currently running? */
	bool isRunning() const;

	/** Load a campaign. */
	void load(const Common::UString &campaign);
	/** Load a stand-alone module as a campaign. */
	void loadModule(const Common::UString &module);
	/** Use this character as the player character. */
	void usePC(const Common::UString &utc);
	/** Exit the currently running campaign. */
	void exit();
	// '---

	// .--- Information about the current campaign
	/** Return the name of the current campaign. */
	const Aurora::LocString &getName() const;
	/** Return the description of the current campaign. */
	const Aurora::LocString &getDescription() const;
	// '---

	// .--- Elements of the current campaign
	/** Return the currently running module. */
	Module &getModule();
	// '---

	// .--- Interact with the current campaign
	/** Refresh all localized strings. */
	void refreshLocalized();
	// '---

	// .--- Static utility methods
	static Common::UString getName(const Common::UString &campaign);
	static Common::UString getDescription(const Common::UString &campaign);
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

	bool _hasCampaign; ///< Do we have a campaign?
	bool _running;     ///< Are we currently running a campaign?
	bool _exit;        ///< Should we exit the campaign?

	/** The name of the currently loaded campaign. */
	Aurora::LocString _name;
	/** The description of the currently loaded campaign. */
	Aurora::LocString _description;

	/** All modules used by the current campaign. */
	std::list<Common::UString> _modules;
	/** The module the current campaign starts in. */
	Common::UString _startModule;

	/** The current module of the current campaign. */
	Common::ScopedPtr<Module> _module;

	/** The player character we use. */
	Common::ScopedPtr<Creature> _pc;

	/** The campaign we should change to. */
	Common::UString _newCampaign;
	/** Is the campaign to change to a stand-alone module? */
	bool _newCampaignStandalone;

	EventQueue _eventQueue;


	/** Unload the whole shebang.
	 *
	 *  @param completeUnload Also unload the PC
	 *                        true:  completely quit the module
	 *                        false: the PC can be transferred to a new module.
	 */
	void unload(bool completeUnload = true);

	void unloadPC();

	/** Load a new campaign. */
	void loadCampaign(const Common::UString &campaign, bool standalone);
	/** Schedule a change to a new campaign. */
	void changeCampaign(const Common::UString &campaign, bool standalone);
	/** Actually replace the currently running campaign. */
	void replaceCampaign();

	/** Load the actual campaign resources. */
	void loadCampaignFile(const Common::UString &campaign);
	/** Set up the loading of a singular, stand-alone module. */
	void setupStandaloneModule(const Common::UString &module);

	void handleEvents();


	/** Return the actual real directory for this campaign. */
	static Common::UString getDirectory(const Common::UString &campaign);
	/** Open the MMD campaign file for this campaign. */
	static Common::SeekableReadStream *openMMD(const Common::UString &campaign);
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_CAMPAIGN_H

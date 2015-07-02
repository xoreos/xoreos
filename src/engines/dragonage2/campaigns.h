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
 *  The context managing and running the Dragon Age II campaigns.
 */

#ifndef ENGINES_DRAGONAGE2_CAMPAIGNS_H
#define ENGINES_DRAGONAGE2_CAMPAIGNS_H

#include <vector>

#include "src/common/ustring.h"

namespace Engines {

class Console;

namespace DragonAge2 {

class DragonAge2Engine;
class Campaign;

class Campaigns {
public:
	typedef std::vector<Campaign *> PlayableCampaigns;
	typedef std::vector<Campaign *> AddinContent;

	Campaigns(::Engines::Console &console, DragonAge2Engine &engine);
	~Campaigns();

	/** Return all playable campaigns. */
	const PlayableCampaigns &getCampaigns() const;
	/** Return all add-in content for other campaigns. */
	const AddinContent &getAddins() const;

	/** Find a specific campaign by UID and return it, or 0 if not found. */
	const Campaign *findCampaign(const Common::UString &uid) const;
	/** Find a specific add-in by UID and return it, or 0 if not found. */
	const Campaign *findAddin(const Common::UString &uid) const;

	/** Load a campaign. */
	void load(const Campaign &campaign);
	/** Run the currently loaded campaign. */
	void run();

	/** Is a campaign currently running? */
	bool isRunning() const;

	/** Return the currently running campaign. */
	Campaign *getCurrentCampaign() const;

	/** Refresh all localized strings. */
	void refreshLocalized();


private:
	::Engines::Console *_console;
	DragonAge2Engine *_engine;

	/** All campaigns we know about. */
	PlayableCampaigns _campaigns;
	/** All add-in content we know about. */
	AddinContent _addins;

	/** The currently loaded campaign. */
	Campaign *_currentCampaign;

	/** Are we currently running a module? */
	bool _running;

	/** The UID of the campaign we should change to. */
	Common::UString _newCampaign;


	void unload();
	void clean();

	/** Load a new campaign. */
	void loadCampaign(const Campaign &campaign);
	/** Schedule a change to a new campaign. */
	void changeCampaign(const Campaign &campaign);

	void findCampaigns();
	void addCampaign(Campaign *campaign);
	Campaign *readCampaign(const Common::UString &cifPath = "", const Common::UString &manifestPath = "",
	                       const Common::UString &addinBase = "");

	Campaign *getCampaign(const Common::UString &uid);
	Campaign *getAddin(const Common::UString &uid);

	/** Actually replace the currently running campaign. */
	void replaceCampaign();

	void handleEvents();
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_CAMPAIGNS_H

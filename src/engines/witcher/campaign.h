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
 *  The context holding a Witcher campaign.
 */

#ifndef ENGINES_WITCHER_CAMPAIGN_H
#define ENGINES_WITCHER_CAMPAIGN_H

#include <list>

#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

#include "src/engines/witcher/module.h"

namespace Engines {

class Console;

namespace Witcher {

struct CampaignDescription {
	Common::UString tag;

	Aurora::LocString name;
	Aurora::LocString description;

	Common::UString file;
	Common::UString directory;

	void clear();
};

class Campaign {
public:
	Campaign(::Engines::Console &console);
	~Campaign();

	const std::list<CampaignDescription> &getCampaigns() const;

	/** Clear the whole context. */
	void clear();

	/** Load a campaign. */
	void load(const CampaignDescription &desc);
	/** Run the currently loaded campaign. */
	void run();

	/** Is a campaign currently running? */
	bool isRunning() const;

	/** Return the name of the current campaign. */
	const Aurora::LocString &getName() const;
	/** Return the description of the current campaign. */
	const Aurora::LocString &getDescription() const;

	/** Refresh all localized strings. */
	void refreshLocalized();

	/** Return the currently running module. */
	Module *getModule();


private:
	/** All campaigns we know about. */
	std::list<CampaignDescription> _campaigns;

	/** The currently loaded campaign. */
	CampaignDescription _currentCampaign;

	/** Are we currently running a module? */
	bool _running;

	/** All modules used by the current campaign. */
	std::list<Common::UString> _modules;
	/** The module the current campaign starts in. */
	Common::UString _startModule;

	/** The current module of the current campaign. */
	Module _module;

	/** The campaign we should change to. */
	const CampaignDescription *_newCampaign;


	/** Load a new campaign. */
	void loadCampaign(const CampaignDescription &desc);
	/** Schedule a change to a new campaign. */
	void changeCampaign(const CampaignDescription &desc);
	/** Load the actual campaign resources. */
	void loadCampaignFile(const CampaignDescription &desc);

	void findCampaigns();
	bool readCampaign(const Common::UString &mmdFile, CampaignDescription &desc);


	// Methods called by the module

	/** Actually replace the currently running campaign. */
	void replaceCampaign();

	friend class Module;
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_CAMPAIGN_H

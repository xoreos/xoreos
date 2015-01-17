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

/** @file engines/nwn2/campaign.h
 *  The context holding an NWN2 campaign.
 */

#ifndef ENGINES_NWN2_CAMPAIGN_H
#define ENGINES_NWN2_CAMPAIGN_H

#include <list>

#include "common/ustring.h"

#include "aurora/locstring.h"
#include "aurora/resman.h"

#include "engines/nwn2/module.h"

namespace Engines {

namespace NWN2 {

struct CampaignDescription {
	Common::UString directory;

	Aurora::LocString name;
	Aurora::LocString description;
};

class Campaign {
public:
	Campaign();
	~Campaign();

	const std::list<CampaignDescription> &getCampaigns() const;

	/** Clear the whole context. */
	void clear();

	/** Load a campaign. */
	void loadCampaign(const CampaignDescription &desc);

	/** Return the name of the current module. */
	const Common::UString &getName() const;

	/** Return the description of the current module. */
	const Common::UString &getDescription() const;

private:
	std::list<CampaignDescription> _campaigns;

	Aurora::ResourceManager::ChangeID _resCampaign;

	CampaignDescription _currentCampaign;

	std::list<Common::UString> _modules;
	Common::UString _startModule;

	Module _module;


	void findCampaigns();
	bool readCampaign(const Common::UString &camFile, CampaignDescription &desc);
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_CAMPAIGN_H

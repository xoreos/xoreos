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

#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

#include "src/engines/witcher/module.h"

namespace Common {
	class SeekableReadStream;
}

namespace Engines {

class Console;

namespace Witcher {

class Campaign {
public:
	Campaign(::Engines::Console &console);
	~Campaign();

	/** Clear the whole context. */
	void clear();

	/** Load a campaign. */
	void load(const Common::UString &campaign);
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
	Module &getModule();

	static Common::UString getName(const Common::UString &campaign);
	static Common::UString getDescription(const Common::UString &campaign);


private:
	/** Are we currently running a module? */
	bool _running;

	/** The name of the currently loaded campaign. */
	Aurora::LocString _name;
	/** The description of the currently loaded campaign. */
	Aurora::LocString _description;

	/** All modules used by the current campaign. */
	std::list<Common::UString> _modules;
	/** The module the current campaign starts in. */
	Common::UString _startModule;

	/** The current module of the current campaign. */
	Module _module;

	/** The campaign we should change to. */
	Common::UString _newCampaign;


	/** Load a new campaign. */
	void loadCampaign(const Common::UString &campaign);
	/** Schedule a change to a new campaign. */
	void changeCampaign(const Common::UString &campaign);
	/** Load the actual campaign resources. */
	void loadCampaignFile(const Common::UString &campaign);

	static Common::UString getDirectory(const Common::UString &campaign);
	static Common::SeekableReadStream *openMMD(const Common::UString &campaign);


	// Methods called by the module

	/** Actually replace the currently running campaign. */
	void replaceCampaign();

	friend class Module;
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_CAMPAIGN_H

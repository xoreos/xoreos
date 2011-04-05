/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/console.h
 *  NWN (debug) console.
 */

#ifndef ENGINES_NWN_CONSOLE_H
#define ENGINES_NWN_CONSOLE_H

#include <list>
#include <map>

#include "common/types.h"
#include "common/ustring.h"

#include "engines/aurora/console.h"

namespace Engines {

namespace NWN {

class Module;

class Console : public ::Engines::Console {
public:
	Console();
	~Console();

	void setModule(Module *module = 0);

private:
	typedef std::map<Common::UString, int, Common::UString::iless> CampaignMap;


	Module *_module;

	std::list<Common::UString> _campaigns;
	std::list<Common::UString> _modules;
	std::list<Common::UString> _areas;
	std::list<Common::UString> _music;

	CampaignMap _campaignModules;

	uint32 _maxSizeMusic;


	void updateCaches();

	void updateCampaigns();
	void updateModules();
	void updateAreas();
	void updateMusic();


	void cmdQuitModule   (const CommandLine &cl);
	void cmdListCampaigns(const CommandLine &cl);
	void cmdLoadCampaign (const CommandLine &cl);
	void cmdListModules  (const CommandLine &cl);
	void cmdLoadModule   (const CommandLine &cl);
	void cmdListAreas    (const CommandLine &cl);
	void cmdGotoArea     (const CommandLine &cl);
	void cmdListMusic    (const CommandLine &cl);
	void cmdStopMusic    (const CommandLine &cl);
	void cmdPlayMusic    (const CommandLine &cl);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_CONSOLE_H

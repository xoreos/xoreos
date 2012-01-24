/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
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

	// Caches
	std::list<Common::UString> _campaigns; ///< All known campaigns modules.
	std::list<Common::UString> _modules;   ///< All known modules.
	std::list<Common::UString> _areas;     ///< All known areas in the current module.
	std::list<Common::UString> _music;     ///< All known music resources.

	CampaignMap _campaignModules; ///< Mapping campaign module file -> campaign module name.

	uint32 _maxSizeMusic;


	// Updating the caches

	void updateCaches();

	void updateCampaigns();
	void updateModules();
	void updateAreas();
	void updateMusic();

	// The commands
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

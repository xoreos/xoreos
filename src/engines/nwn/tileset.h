/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/tileset.h
 *  NWN tileset.
 */

#ifndef ENGINES_NWN_TILESET_H
#define ENGINES_NWN_TILESET_H

#include "common/types.h"
#include "common/ustring.h"

namespace Common {
	class ConfigFile;
	class ConfigDomain;
}

namespace Engines {

namespace NWN {

class Tileset {
public:
	Tileset(const Common::UString &resRef);
	~Tileset();

	const Common::UString &getName() const;

private:
	Common::UString _resRef;
	Common::UString _name;

	void load(const Common::ConfigFile &set);

	void loadGeneral(const Common::ConfigDomain &general);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_TILESET_H

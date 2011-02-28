/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/ifofile.h
 *  NWN module.ifo loader.
 */

#ifndef ENGINES_NWN_IFOFILE_H
#define ENGINES_NWN_IFOFILE_H

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/locstring.h"

namespace Engines {

namespace NWN {

class IFOFile {
public:
	IFOFile();
	~IFOFile();

	void load();   ///< Load the currently available module.ifo.
	void unload(); ///< Unload a currently loaded IFO.

	void loadTLK(); ///< Load the module's custom TLK.

	bool isSave() const; ///< Is the module a save file?

	/** Return the name of the module. */
	const Aurora::LocString &getName() const;
	/** Return the description of the module. */
	const Aurora::LocString &getDescription() const;

	/** Return the minimum game version the module needs to run. */
	void getMinVersion(int &major, int &minor) const;

	/** Return the list of required expansions. */
	uint16 getExpansions() const;

	/** Return the starting movie. */
	const Common::UString &getStartMovie() const;

	/** Return the entry area. */
	const Common::UString &getEntryArea() const;
	/** Return the entry position. */
	void getEntryPosition (float &x, float &y, float &z) const;
	/** Return the entry direction. */
	void getEntryDirection(float &x, float &y) const;

	/** Return the list of required HAK files. */
	const std::vector<Common::UString> &getHAKs() const;

	/** Return the list of areas in the module. */
	const std::vector<Common::UString> &getAreas() const;

	/** Return the list of NSS (script) files that should be cached. */
	const std::vector<Common::UString> &getNSSCache() const;

	/** Return the module's starting time. */
	void getStartTime(uint8 &hour, uint8 &day, uint8 &month, uint32 &year) const;

	uint8 getDawnHour() const; ///< Return the hour dawn starts.
	uint8 getDuskHour() const; ///< Return the hour dusk starts.

	/** Return the number of real time minutes per game hour. */
	uint32 getMinutesPerHour() const;

	/** Get the number creature kill XP is multiplied by. */
	float getXPScale() const;

private:
	byte _id[32];

	Aurora::LocString _name;
	Aurora::LocString _description;

	int _minVersionMajor;
	int _minVersionMinor;

	uint16 _expansions;

	bool _isSave;

	Common::UString _customTLK;

	Common::UString _startMovie;

	Common::UString _entryArea;
	float _entryX;
	float _entryY;
	float _entryZ;
	float _entryDirX;
	float _entryDirY;

	std::vector<Common::UString> _haks;
	std::vector<Common::UString> _areas;
	std::vector<Common::UString> _nssCache;

	uint8 _hourDawn;
	uint8 _hourDusk;

	uint8 _minutesPerHour;

	uint8  _startHour;
	uint8  _startDay;
	uint8  _startMonth;
	uint32 _startYear;

	float _xpScale;

	void clear();

	void parseVersion(const Common::UString &version);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_IFOFILE_H

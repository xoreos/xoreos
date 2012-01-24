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

/** @file engines/nwn/ifofile.h
 *  NWN module.ifo loader.
 */

#ifndef ENGINES_NWN_IFOFILE_H
#define ENGINES_NWN_IFOFILE_H

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/locstring.h"

#include "engines/nwn/script/container.h"

namespace Engines {

namespace NWN {

class IFOFile : public ScriptContainer {
public:
	IFOFile();
	~IFOFile();

	void load();   ///< Load the currently available module.ifo.
	void unload(); ///< Unload a currently loaded IFO.

	void loadTLK(); ///< Load the module's custom TLK.

	// General properties

	bool isSave() const; ///< Is the module a save file?

	/** Return the module's tag. */
	const Common::UString &getTag() const;

	/** Return the name of the module. */
	const Aurora::LocString &getName() const;
	/** Return the description of the module. */
	const Aurora::LocString &getDescription() const;

	// General module requirements

	/** Return the minimum game version the module needs to run. */
	void getMinVersion(int &major, int &minor) const;
	/** Return the list of required expansions. */
	uint16 getExpansions() const;

	/** Return the list of required HAK files. */
	const std::vector<Common::UString> &getHAKs() const;

	// Entry behaviour

	/** Return the starting movie. */
	const Common::UString &getStartMovie() const;

	/** Return the entry area. */
	const Common::UString &getEntryArea() const;
	/** Return the entry position. */
	void getEntryPosition (float &x, float &y, float &z) const;
	/** Return the entry direction. */
	void getEntryDirection(float &x, float &y) const;


	/** Return the list of areas in the module. */
	const std::vector<Common::UString> &getAreas() const;
	/** Return the list of NSS (script) files that should be cached. */
	const std::vector<Common::UString> &getNSSCache() const;

	// Time managment

	/** Return the module's starting time. */
	void getStartTime(uint8 &hour, uint8 &day, uint8 &month, uint32 &year) const;

	uint8 getDawnHour() const; ///< Return the hour dawn starts.
	uint8 getDuskHour() const; ///< Return the hour dusk starts.

	/** Return the number of real time minutes per game hour. */
	uint32 getMinutesPerHour() const;


	/** Get the number creature kill XP is multiplied by. */
	float getXPScale() const;

private:
	byte _id[32]; ///< The module's unique ID.

	Common::UString _tag; ///< The module's tag.

	Aurora::LocString _name;        ///< The module's localized name.
	Aurora::LocString _description; ///< The module's localized description.

	int _minVersionMajor; ///< Minimum major game version this module needs.
	int _minVersionMinor; ///< Minimum minor game version this module needs.

	uint16 _expansions; ///< Bitfield of required expansions.

	bool _isSave; ///< Is this module a save?

	Common::UString _customTLK; ///< The custom TLK the module uses.

	Common::UString _startMovie; ///< The movie the module starts with.

	// Entry location
	Common::UString _entryArea; ///< The area the PC starts in.
	float _entryX;    ///< The X position the PC starts in.
	float _entryY;    ///< The Y position the PC starts in.
	float _entryZ;    ///< The Z position the PC starts in.
	float _entryDirX; ///< The X orientation the PC starts in.
	float _entryDirY; ///< The Y orientation the PC starts in.

	std::vector<Common::UString> _haks;     ///< HAKs required by the module.
	std::vector<Common::UString> _areas;    ///< Areas found in the module.
	std::vector<Common::UString> _nssCache; ///< Scripts that should be cached.

	uint8 _hourDawn; ///< The hour dawn starts.
	uint8 _hourDusk; ///< The hour dusk starts.

	uint8 _minutesPerHour; ///< Number of real time minutes per game hour.

	uint8  _startHour;  ///< Hour the module starts.
	uint8  _startDay;   ///< Day the module starts.
	uint8  _startMonth; ///< Month the module starts.
	uint32 _startYear;  ///< Year the module starts.

	float _xpScale; ///< The number creature kill XP is multiplied by.

	void clear();

	void parseVersion(const Common::UString &version);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_IFOFILE_H

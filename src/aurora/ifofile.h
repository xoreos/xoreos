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
 *  Loader for the module.ifo file.
 */

#ifndef AURORA_IFOFILE_H
#define AURORA_IFOFILE_H

#include <vector>
#include <memory>

#include <boost/noncopyable.hpp>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

namespace Common {
	class SeekableReadStream;
}

namespace Aurora {

class GFF3File;
class GFF3Struct;

/** An IFO (module information) file, describing global module properties
 *  in many Aurora games.
 *
 *  Information commonly found in IFO files include: the name and description
 *  - Name and description
 *  - Requirements
 *  - List of all areas
 *  - Starting area and position
 *  - Time and date
 *
 *  The base file format of an IFO file is a GFF3.
 *
 *  Especially for Neverwinter Nights, Neverwinter Nights 2 and The Witcher,
 *  this file is serves as the global description of the module file as a
 *  whole, even more so if this is a standalone adventure module.
 *
 *  In the two Knights of the Old Republic games, the IFO file is less
 *  important, but still contains vital information.
 *
 *  Jade Empire does not use an IFO file. Neither do Sonic Chronicles or
 *  the two Dragon Age games.
 */
class IFOFile : boost::noncopyable {
public:
	IFOFile();
	~IFOFile();

	/** Take over this stream and load an IFO out of it.
	 *
	 *  Since this is a GFF3 file, which might be found in a Neverwinter
	 *  Nights premium module and therefore mangled, the parameter
	 *  repairNWNPremium indicates whether we want to try to repair such
	 *  mangled module.ifo files.
	 */
	void load(Common::SeekableReadStream *stream, bool repairNWNPremium = false);
	/** Load the currently available module.ifo.
	 *
	 *  Since this is a GFF3 file, which might be found in a Neverwinter
	 *  Nights premium module and therefore mangled, the parameter
	 *  repairNWNPremium indicates whether we want to try to repair such
	 *  mangled module.ifo files.
	 */
	void load(bool repairNWNPremium = false);
	/** Unload a currently loaded IFO. */
	void unload();

	/** Return the IFO's GFF struct. */
	const GFF3Struct *getGFF() const;

	// .--- General properties
	/** Return the version of this IFO file. */
	uint32_t getVersion() const;
	/** Return the ID of the IFO file creator. */
	uint32_t getCreatorID() const;

	bool isSave() const; ///< Is the module a save file?

	/** Return the module's tag. */
	const Common::UString &getTag() const;

	/** Return the name of the module. */
	const LocString &getName() const;
	/** Return the description of the module. */
	const LocString &getDescription() const;

	/** Return the custom TLK table this module uses. */
	const Common::UString &getTLK() const;
	// '---

	// .--- Module requirements
	/** Return the minimum game version the module needs to run. */
	void getMinVersion(int &major, int &minor) const;

	/** Return the list of required expansions.
	 *
	 *  This is a bitfield, storing the information whether expansion 1
	 *  to expansion 16 is required, from LSB to MSB. If the Nth bit is
	 *  set, the expansion N is required. Otherwise, it isn't.
	 */
	uint16_t getExpansions() const;

	/** Return the list of required HAK files.
	 *
	 *  A HAK file is an extra ERF resource archive that contains
	 *  resources used by this module.
	 */
	const std::vector<Common::UString> &getHAKs() const;
	// '---

	// .--- Entry behaviour
	/** Return the starting movie. */
	const Common::UString &getStartMovie() const;

	/** Return the entry area. */
	const Common::UString &getEntryArea() const;
	/** Return the entry position. */
	void getEntryPosition (float &x, float &y, float &z) const;
	/** Return the entry direction. */
	void getEntryDirection(float &x, float &y) const;
	// '---

	// .--- Module elements
	/** Return the list of areas in the module. */
	const std::vector<Common::UString> &getAreas() const;
	/** Return the list of NSS (script) files that should be cached. */
	const std::vector<Common::UString> &getNSSCache() const;

	/** Returns the list of quests used in the module. */
	const std::vector<Common::UString> &getQuests() const;
	/** Returns the list of quest databases used in the module. */
	const std::vector<Common::UString> &getQuestDBs() const;

	/** Returns the list of story NPCs used in the module. */
	const std::vector<Common::UString> &getStoryNPCs() const;
	/** Returns the list of monster NPCs used in the module. */
	const std::vector<Common::UString> &getMonsterNPCs() const;
	// '---

	// .--- Time management
	/** Return the module's starting time. */
	void getStartTime(uint8_t &hour, uint8_t &day, uint8_t &month, uint32_t &year) const;

	uint8_t getDawnHour() const; ///< Return the hour dawn starts.
	uint8_t getDuskHour() const; ///< Return the hour dusk starts.

	/** Return the number of real time minutes per game hour. */
	uint32_t getMinutesPerHour() const;
	// '---

	// .--- Weather
	/** Return the minimum weather intensity. */
	int32_t getMinWeatherIntensity() const;
	/** Return the maximum weather intensity. */
	int32_t getMaxWeatherIntensity() const;
	/** Return the chance that it's going to rain. */
	int32_t getRainChance() const;
	/** Return the chance that it's going to snow. */
	int32_t getSnowChance() const;
	// '---

	// .--- Combat behaviour
	/** Get the number creature kill XP is multiplied by. */
	float getXPScale() const;
	// '---

private:
	std::unique_ptr<GFF3File> _gff; ///< The module.ifo GFF.

	byte _id[32]; ///< The module's unique ID.

	Common::UString _tag; ///< The module's tag.

	LocString _name;        ///< The module's localized name.
	LocString _description; ///< The module's localized description.

	uint32_t _version;   ///< Version of this IFO file.
	uint32_t _creatorID; ///< ID of the IFO file creator.

	int _minVersionMajor; ///< Minimum major game version this module needs.
	int _minVersionMinor; ///< Minimum minor game version this module needs.

	uint16_t _expansions; ///< Bitfield of required expansions.

	bool _isSave; ///< Is this module a save?

	Common::UString  _customTLK; ///< The custom TLK the module uses.

	Common::UString _startMovie; ///< The movie the module starts with.

	Common::UString _entryArea; ///< The area the PC starts in.

	float _entryX;    ///< The X position the PC starts in.
	float _entryY;    ///< The Y position the PC starts in.
	float _entryZ;    ///< The Z position the PC starts in.
	float _entryDirX; ///< The X orientation the PC starts in.
	float _entryDirY; ///< The Y orientation the PC starts in.

	std::vector<Common::UString> _haks;     ///< HAKs required by the module.
	std::vector<Common::UString> _areas;    ///< Areas found in the module.
	std::vector<Common::UString> _nssCache; ///< Scripts that should be cached.

	std::vector<Common::UString> _quests;   ///< List of quests used in the module.
	std::vector<Common::UString> _questDBs; ///< List of quest databases used in the module.

	std::vector<Common::UString> _storyNPCs;   ///< List of story NPCs used in the module.
	std::vector<Common::UString> _monsterNPCs; ///< List of monster NPCs used in the module.

	uint8_t _hourDawn; ///< The hour dawn starts.
	uint8_t _hourDusk; ///< The hour dusk starts.

	uint8_t _minutesPerHour; ///< Number of real time minutes per game hour.

	uint8_t  _startHour;  ///< Hour the module starts.
	uint8_t  _startDay;   ///< Day the module starts.
	uint8_t  _startMonth; ///< Month the module starts.
	uint32_t _startYear;  ///< Year the module starts.

	int32_t _minWeatherIntensity; ///< Minimum weather intensity.
	int32_t _maxWeatherIntensity; ///< Maximum weather intensity.
	int32_t _rainChance;          ///< Chance for rain.
	int32_t _snowChance;          ///< Chance for snow.

	float _xpScale; ///< The number creature kill XP is multiplied by.

	void clear();

	void parseVersion(const Common::UString &version);
};

} // End of namespace Aurora

#endif // AURORA_IFOFILE_H

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

/* See BioWare's own specs released for Neverwinter Nights modding
 * (<https://github.com/xoreos/xoreos-docs/tree/master/specs/bioware>)
 */

#include <cassert>
#include <cstdio>
#include <cstring>

#include <algorithm>

#include "src/common/error.h"
#include "src/common/readstream.h"

#include "src/aurora/ifofile.h"
#include "src/aurora/gff3file.h"
#include "src/aurora/resman.h"

namespace Aurora {

IFOFile::IFOFile() {
	clear();
}

IFOFile::~IFOFile() {
}

void IFOFile::clear() {
	_gff.reset();

	std::memset(_id, 0, sizeof(_id));

	_tag.clear();

	_name.clear();
	_description.clear();

	_minVersionMajor = 0;
	_minVersionMinor = 0;

	_expansions = 0;

	_isSave = false;

	_customTLK.clear();

	_startMovie.clear();

	_entryArea.clear();

	_entryX    = 0.0f;
	_entryY    = 0.0f;
	_entryZ    = 0.0f;
	_entryDirX = 0.0f;
	_entryDirY = 0.0f;

	_haks.clear();
	_areas.clear();
	_nssCache.clear();

	_hourDawn = 0;
	_hourDusk = 0;

	_minutesPerHour = 1;

	_startHour  = 0;
	_startDay   = 1;
	_startMonth = 1;
	_startYear  = 0;

	_xpScale = 1.0f;
}

void IFOFile::unload() {
	clear();
}

void IFOFile::load(Common::SeekableReadStream *stream, bool repairNWNPremium) {
	unload();

	assert(stream);
	_gff = std::make_unique<GFF3File>(stream, MKTAG('I', 'F', 'O', ' '), repairNWNPremium);

	const GFF3Struct &ifoTop = _gff->getTopLevel();

	// IFO version
	_version   = ifoTop.getUint("Mod_Version");
	_creatorID = ifoTop.getUint("Mod_Creator_ID");

	// Game version
	Common::UString version = ifoTop.getString("Mod_MinGameVer");
	parseVersion(version);

	// Expansions
	_expansions = ifoTop.getUint("Expansion_Pack");

	// Is Save?
	_isSave = ifoTop.getUint("Mod_IsSaveGame") == 1;

	// Tag
	_tag = ifoTop.getString("Mod_Tag");

	// Name and description
	ifoTop.getLocString("Mod_Name"       , _name);
	ifoTop.getLocString("Mod_Description", _description);

	// ID
	const size_t idSize = _isSave ? 32 : 16;
	std::unique_ptr<Common::SeekableReadStream> id(ifoTop.getData("Mod_ID"));

	if (id && (id->read(_id, idSize) != idSize))
		throw Common::Exception("Can't read MOD ID");

	// TLK
	_customTLK = ifoTop.getString("Mod_CustomTlk");

	// Starting movie
	_startMovie = ifoTop.getString("Mod_StartMovie");

	// HAK List
	if (ifoTop.hasField("Mod_HakList")) {
		const GFF3List &haks = ifoTop.getList("Mod_HakList");

		for (GFF3List::const_iterator h = haks.begin(); h != haks.end(); ++h)
			_haks.insert(_haks.begin(), (*h)->getString("Mod_Hak"));
	}

	// Singular HAK
	Common::UString hak = ifoTop.getString("Mod_Hak");
	if (!hak.empty())
		_haks.insert(_haks.begin(), hak);

	// Areas
	if (ifoTop.hasField("Mod_Area_list")) {
		const GFF3List &areas = ifoTop.getList("Mod_Area_list");

		for (GFF3List::const_iterator a = areas.begin(); a != areas.end(); ++a)
			_areas.push_back((*a)->getString("Area_Name"));
	}

	std::sort(_areas.begin(), _areas.end(), Common::UString::iless());

	// NSS files that should be cached
	if (ifoTop.hasField("Mod_CacheNSSList")) {
		const GFF3List &nss = ifoTop.getList("Mod_CacheNSSList");

		for (GFF3List::const_iterator n = nss.begin(); n != nss.end(); ++n)
			_nssCache.push_back((*n)->getString("ResRef"));
	}

	// Quests
	if (ifoTop.hasField("Quest_list")) {
		const GFF3List &quests = ifoTop.getList("Quest_list");

		for (GFF3List::const_iterator q = quests.begin(); q != quests.end(); ++q)
			_quests.push_back((*q)->getString("Quest"));
	}
	if (ifoTop.hasField("QuestBase_list")) {
		const GFF3List &quests = ifoTop.getList("QuestBase_list");

		for (GFF3List::const_iterator q = quests.begin(); q != quests.end(); ++q)
			_questDBs.push_back((*q)->getString("QuestBase"));
	}

	// NPCs
	if (ifoTop.hasField("StoryNPC_list")) {
		const GFF3List &npcs = ifoTop.getList("StoryNPC_list");

		for (GFF3List::const_iterator n = npcs.begin(); n != npcs.end(); ++n)
			_storyNPCs.push_back((*n)->getString("StoryNPC"));
	}
	if (ifoTop.hasField("MonsterNPC_list")) {
		const GFF3List &npcs = ifoTop.getList("MonsterNPC_list");

		for (GFF3List::const_iterator n = npcs.begin(); n != npcs.end(); ++n)
			_monsterNPCs.push_back((*n)->getString("MonsterNPC"));
	}

	// Entry
	_entryArea = ifoTop.getString("Mod_Entry_Area");
	_entryX    = ifoTop.getDouble("Mod_Entry_X");
	_entryY    = ifoTop.getDouble("Mod_Entry_Y");
	_entryZ    = ifoTop.getDouble("Mod_Entry_Z");
	_entryDirX = ifoTop.getDouble("Mod_Entry_Dir_X");
	_entryDirY = ifoTop.getDouble("Mod_Entry_Dir_Y");

	// Time settings
	_hourDawn       = ifoTop.getUint("Mod_DawnHour"  , 0);
	_hourDusk       = ifoTop.getUint("Mod_DuskHour"  , 0);
	_minutesPerHour = ifoTop.getUint("Mod_MinPerHour", 1);
	_startHour      = ifoTop.getUint("Mod_StartHour" , 0);
	_startDay       = ifoTop.getUint("Mod_StartDay"  , 1);
	_startMonth     = ifoTop.getUint("Mod_StartMonth", 1);
	_startYear      = ifoTop.getUint("Mod_StartYear" , 0);

	// Weather
	_minWeatherIntensity = ifoTop.getSint("Mod_MinWeatInt",   0);
	_maxWeatherIntensity = ifoTop.getSint("Mod_MaxWeatInt", 100);
	_rainChance          = ifoTop.getSint("Mod_RainChance",   0);
	_snowChance          = ifoTop.getSint("Mod_SnowChance",   0);

	// XP Scale
	_xpScale = ifoTop.getUint("Mod_XPScale", 100) / 100.0f;
}

void IFOFile::load(bool repairNWNPremium) {
	unload();

	Common::SeekableReadStream *stream = ResMan.getResource("module", kFileTypeIFO);
	if (!stream)
		throw Common::Exception("No module.ifo available");

	load(stream, repairNWNPremium);
}

void IFOFile::parseVersion(const Common::UString &version) {
	if (std::sscanf(version.c_str(), "%d.%d", &_minVersionMajor, &_minVersionMinor) != 2)
		_minVersionMajor = _minVersionMinor = 0;
}

const GFF3Struct *IFOFile::getGFF() const {
	if (!_gff)
		return 0;

	return &_gff->getTopLevel();
}

uint32_t IFOFile::getVersion() const {
	return _version;
}

uint32_t IFOFile::getCreatorID() const {
	return _creatorID;
}

bool IFOFile::isSave() const {
	return _isSave;
}

const Common::UString &IFOFile::getTag() const {
	return _tag;
}

const LocString &IFOFile::getName() const {
	return _name;
}

const LocString &IFOFile::getDescription() const {
	return _description;
}

const Common::UString &IFOFile::getTLK() const {
	return _customTLK;
}

void IFOFile::getMinVersion(int &major, int &minor) const {
	major = _minVersionMajor;
	minor = _minVersionMinor;
}

uint16_t IFOFile::getExpansions() const {
	return _expansions;
}

const Common::UString &IFOFile::getStartMovie() const {
	return _startMovie;
}

const Common::UString &IFOFile::getEntryArea() const {
	return _entryArea;
}

void IFOFile::getEntryPosition(float &x, float &y, float &z) const {
	x = _entryX;
	y = _entryY;
	z = _entryZ;
}

void IFOFile::getEntryDirection(float &x, float &y) const {
	x = _entryDirX;
	y = _entryDirY;
}

const std::vector<Common::UString> &IFOFile::getHAKs() const {
	return _haks;
}

const std::vector<Common::UString> &IFOFile::getAreas() const {
	return _areas;
}

const std::vector<Common::UString> &IFOFile::getNSSCache() const {
	return _nssCache;
}

const std::vector<Common::UString> &IFOFile::getQuests() const {
	return _quests;
}

const std::vector<Common::UString> &IFOFile::getQuestDBs() const {
	return _questDBs;
}

const std::vector<Common::UString> &IFOFile::getStoryNPCs() const {
	return _storyNPCs;
}

const std::vector<Common::UString> &IFOFile::getMonsterNPCs() const {
	return _monsterNPCs;
}

void IFOFile::getStartTime(uint8_t &hour, uint8_t &day, uint8_t &month, uint32_t &year) const {
	hour  = _startHour;
	day   = _startDay;
	month = _startMonth;
	year  = _startYear;
}

uint8_t IFOFile::getDawnHour() const {
	return _hourDawn;
}

uint8_t IFOFile::getDuskHour() const {
	return _hourDusk;
}

uint32_t IFOFile::getMinutesPerHour() const {
	return _minutesPerHour;
}

int32_t IFOFile::getMinWeatherIntensity() const {
	return _minWeatherIntensity;
}

int32_t IFOFile::getMaxWeatherIntensity() const {
	return _maxWeatherIntensity;
}

int32_t IFOFile::getRainChance() const {
	return _rainChance;
}

int32_t IFOFile::getSnowChance() const {
	return _snowChance;
}

float IFOFile::getXPScale() const {
	return _xpScale;
}

} // End of namespace Aurora

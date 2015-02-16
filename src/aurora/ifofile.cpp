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

/** @file aurora/ifofile.cpp
 *  Loader for the module.ifo file.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/ifofile.h"
#include "aurora/gfffile.h"
#include "aurora/talkman.h"

namespace Aurora {

IFOFile::IFOFile() : _gff(0) {
	clear();
}

IFOFile::~IFOFile() {
	delete _gff;
}

void IFOFile::clear() {
	delete _gff;
	_gff = 0;

	memset(_id, 0, sizeof(_id));

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

	_entryX    = 0.0;
	_entryY    = 0.0;
	_entryZ    = 0.0;
	_entryDirX = 0.0;
	_entryDirY = 0.0;

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

	_xpScale = 1.0;
}

void IFOFile::unload() {
	TalkMan.removeAltTable();

	clear();
}

void IFOFile::load() {
	unload();

	_gff = new GFFFile("module", kFileTypeIFO, MKTAG('I', 'F', 'O', ' '));

	const GFFStruct &ifoTop = _gff->getTopLevel();

	// Sanity checks
	if (ifoTop.getUint("Mod_Creator_ID") != 2)
		throw Common::Exception("Invalid Mod_Creator_ID");
	if (ifoTop.getUint("Mod_Version") != 3)
		throw Common::Exception("Invalid Mod_Version");

	// Version
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
	uint idSize = _isSave ? 32 : 16;
	Common::SeekableReadStream *id = ifoTop.getData("Mod_ID");
	if (id && (id->read(_id, idSize) != idSize))
		throw Common::Exception("Can't read MOD ID");
	delete id;

	// TLK
	_customTLK = ifoTop.getString("Mod_CustomTlk");

	// Starting movie
	_startMovie = ifoTop.getString("Mod_StartMovie");

	// HAK List
	if (ifoTop.hasField("Mod_HakList")) {
		const GFFList &haks = ifoTop.getList("Mod_HakList");

		for (GFFList::const_iterator h = haks.begin(); h != haks.end(); ++h)
			_haks.insert(_haks.begin(), (*h)->getString("Mod_Hak"));
	}

	// Singular HAK
	Common::UString hak = ifoTop.getString("Mod_Hak");
	if (!hak.empty())
		_haks.insert(_haks.begin(), hak);

	// Areas
	if (ifoTop.hasField("Mod_Area_list")) {
		const GFFList &areas = ifoTop.getList("Mod_Area_list");

		for (GFFList::const_iterator a = areas.begin(); a != areas.end(); ++a)
			_areas.push_back((*a)->getString("Area_Name"));
	}

	// NSS files that should be cached
	if (ifoTop.hasField("Mod_CacheNSSList")) {
		const GFFList &nss = ifoTop.getList("Mod_CacheNSSList");

		for (GFFList::const_iterator n = nss.begin(); n != nss.end(); ++n)
			_nssCache.push_back((*n)->getString("ResRef"));
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

	// XP Scale
	_xpScale = ifoTop.getUint("Mod_XPScale", 100) / 100.0;
}

void IFOFile::parseVersion(const Common::UString &version) {
	if (sscanf(version.c_str(), "%d.%d", &_minVersionMajor, &_minVersionMinor) != 2)
		_minVersionMajor = _minVersionMinor = 0;
}

void IFOFile::loadTLK() {
	if (_customTLK.empty())
		return;

	TalkMan.addAltTable(_customTLK);
}

const GFFStruct *IFOFile::getGFF() const {
	if (!_gff)
		return 0;

	return &_gff->getTopLevel();
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

void IFOFile::getMinVersion(int &major, int &minor) const {
	major = _minVersionMajor;
	minor = _minVersionMinor;
}

uint16 IFOFile::getExpansions() const {
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

void IFOFile::getStartTime(uint8 &hour, uint8 &day, uint8 &month, uint32 &year) const {
	hour  = _startHour;
	day   = _startDay;
	month = _startMonth;
	year  = _startYear;
}

uint8 IFOFile::getDawnHour() const {
	return _hourDawn;
}

uint8 IFOFile::getDuskHour() const {
	return _hourDusk;
}

uint32 IFOFile::getMinutesPerHour() const {
	return _minutesPerHour;
}

float IFOFile::getXPScale() const {
	return _xpScale;
}

} // End of namespace Aurora

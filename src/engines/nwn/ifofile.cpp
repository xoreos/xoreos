/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file engines/nwn/ifofile.cpp
 *  NWN module.ifo loader.
 */

#include "common/error.h"
#include "common/stream.h"

#include "aurora/gfffile.h"
#include "aurora/talkman.h"

#include "engines/aurora/util.h"

#include "engines/nwn/ifofile.h"

// We behave like NWN 1.69
static const int kGameVersionMajor =  1;
static const int kGameVersionMinor = 69;

#define TOVER(x,y) ((((uint64) (x)) << 32) | ((uint64) (y)))

namespace Engines {

namespace NWN {

IFOFile::IFOFile() {
	clear();
}

IFOFile::~IFOFile() {
}

void IFOFile::clear() {
	memset(_id, 0, sizeof(_id));

	_tag.clear();

	_name.clear();
	_description.clear();

	_minVersionMajor = 1;
	_minVersionMinor = 22;

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

	Aurora::GFFFile ifo;
	loadGFF(ifo, "module", Aurora::kFileTypeIFO, MKID_BE('IFO '));

	const Aurora::GFFStruct &ifoTop = ifo.getTopLevel();

	// Sanity checks
	if (ifoTop.getUint("Mod_Creator_ID") != 2)
		throw Common::Exception("Invalid Mod_Creator_ID");
	if (ifoTop.getUint("Mod_Version") != 3)
		throw Common::Exception("Invalid Mod_Version");

	// Version
	Common::UString version = ifoTop.getString("Mod_MinGameVer", "1.22");
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
		const Aurora::GFFList &haks = ifoTop.getList("Mod_HakList");

		for (Aurora::GFFList::const_iterator h = haks.begin(); h != haks.end(); ++h)
			_haks.insert(_haks.begin(), (*h)->getString("Mod_Hak"));
	}

	// Singular HAK
	Common::UString hak = ifoTop.getString("Mod_Hak");
	if (!hak.empty())
		_haks.insert(_haks.begin(), hak);

	// Areas
	if (ifoTop.hasField("Mod_Area_list")) {
		const Aurora::GFFList &areas = ifoTop.getList("Mod_Area_list");

		for (Aurora::GFFList::const_iterator a = areas.begin(); a != areas.end(); ++a)
			_areas.push_back((*a)->getString("Area_Name"));
	}

	// NSS files that should be cached
	if (ifoTop.hasField("Mod_CacheNSSList")) {
		const Aurora::GFFList &nss = ifoTop.getList("Mod_CacheNSSList");

		for (Aurora::GFFList::const_iterator n = nss.begin(); n != nss.end(); ++n)
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

	/* TODO: Module scripts
	ifoTop.getString("Mod_OnHeartbeat");
	ifoTop.getString("Mod_OnModLoad");
	ifoTop.getString("Mod_OnModStart");
	ifoTop.getString("Mod_OnClientEntr");
	ifoTop.getString("Mod_OnClientLeav");
	ifoTop.getString("Mod_OnActvtItem");
	ifoTop.getString("Mod_OnAcquirItem");
	ifoTop.getString("Mod_OnUsrDefined");
	ifoTop.getString("Mod_OnUnAqreItem");
	ifoTop.getString("Mod_OnPlrDeath");
	ifoTop.getString("Mod_OnPlrDying");
	ifoTop.getString("Mod_OnPlrEqItm");
	ifoTop.getString("Mod_OnPlrLvlUp");
	ifoTop.getString("Mod_OnSpawnBtnDn");
	ifoTop.getString("Mod_OnPlrRest");
	ifoTop.getString("Mod_OnPlrUnEqItm");
	ifoTop.getString("Mod_OnCutsnAbort");
	ifoTop.getString("Mod_OnPlrChat");
	*/
}

void IFOFile::parseVersion(const Common::UString &version) {
	if (sscanf(version.c_str(), "%d.%d", &_minVersionMajor, &_minVersionMinor) != 2)
		throw Common::Exception("Broken minimum game version string");

	if ((_minVersionMajor < 0) || (_minVersionMinor < 0))
		throw Common::Exception("Broken minimum game version string");

	if (TOVER(kGameVersionMajor, kGameVersionMinor) < TOVER(_minVersionMajor, _minVersionMinor))
		throw Common::Exception("Game version requirements not met (want %d.%d, got %d.%d",
				_minVersionMajor, _minVersionMinor, kGameVersionMajor, kGameVersionMinor);
}

void IFOFile::loadTLK() {
	if (_customTLK.empty())
		return;

	TalkMan.addAltTable(_customTLK);
}

bool IFOFile::isSave() const {
	return _isSave;
}

const Common::UString &IFOFile::getTag() const {
	return _tag;
}

const Aurora::LocString &IFOFile::getName() const {
	return _name;
}

const Aurora::LocString &IFOFile::getDescription() const {
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

} // End of namespace NWN

} // End of namespace Engines

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
 *  The context holding a Dragon Age: Origins campaign.
 */

#include <memory>

#include "src/common/util.h"
#include "src/common/strutil.h"
#include "src/common/error.h"
#include "src/common/filepath.h"
#include "src/common/readfile.h"
#include "src/common/xml.h"

#include "src/aurora/2dareg.h"
#include "src/aurora/gff4file.h"

#include "src/graphics/camera.h"

#include "src/engines/dragonage/game.h"
#include "src/engines/dragonage/campaign.h"
#include "src/engines/dragonage/area.h"
#include "src/engines/dragonage/creature.h"

namespace Engines {

namespace DragonAge {

static const uint32_t kCIFID     = MKTAG('C', 'I', 'F', ' ');
static const uint32_t kVersion01 = MKTAG('V', '0', '.', '1');

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;


Campaign::RIMNode::RIMNode(const RIMNode *p) : parent(p) {
}


Campaign::Campaign(Game &game, const Common::UString &cifPath,
                   const Common::UString &manifestPath, const Common::UString &addinBase) :
	Object(kObjectTypeModule), _game(&game), _cifPath(cifPath),
	_addinBase(addinBase), _enabled(false), _bioware(false), _needsAuth(false),
	_priority(0xFFFFFFFF), _format(0xFFFFFFFF), _state(0xFFFFFFFF),
	_loaded(false), _pc(0) {

	_entryPosition[0] = 0.0;
	_entryPosition[1] = 0.0;
	_entryPosition[2] = 0.0;

	_entryOrientation[0] = 0.0;
	_entryOrientation[1] = 0.0;
	_entryOrientation[2] = 0.0;

	read(cifPath, manifestPath);
}

Campaign::~Campaign() {
	try {
		unload();
	} catch (...) {
	}
}

const Common::UString &Campaign::getUID() const {
	return _uid;
}

const Aurora::LocString &Campaign::getName() const {
	return _name;
}

const Aurora::LocString &Campaign::getDescription() const {
	return _description;
}

const Common::UString &Campaign::getExtendsUID() const {
	return _extends;
}

void Campaign::read(const Common::UString &cifPath, const Common::UString &manifestPath) {
	if (Common::FilePath::getStem(cifPath) == "singleplayer") {
		// Original campaign

		if (!manifestPath.empty())
			throw Common::Exception("Original campaign with a manifest.xml?!?");

		_uid = "Single Player";

		_enabled = true;

		_state  = 2;
		_format = 1;

	} else {
		// Expansion, DLC or community module

		if (manifestPath.empty())
			throw Common::Exception("DLC without a manifest.xml");
	}

	readCIFStatic(cifPath);
	readManifest(manifestPath);

	if (_uid.empty())
		throw Common::Exception("No UID");

	if ((_state != 2) || (_format != 1))
		throw Common::Exception("Unsupported manifest format (%u.%u)", _format, _state);

	if (_extends.empty() && _entryArea.empty())
		throw Common::Exception("Playable campaign without an entry area");

	if (_needsAuth)
		throw Common::Exception("TODO: Needs authorization");
}

void Campaign::readCIFStatic(const Common::UString &path) {
	Common::ReadFile *cifFile = new Common::ReadFile(path);

	GFF4File cif(cifFile, kCIFID);
	if (cif.getTypeVersion() != kVersion01)
		throw Common::Exception("Unsupported CIF version %s", Common::debugTag(cif.getTypeVersion()).c_str());

	const GFF4Struct &cifTop = cif.getTopLevel();

	_name.setString(Aurora::kLanguageEnglish  , cifTop.getString(kGFF4CampaignCIFDisplayNameENUS));
	_name.setString(Aurora::kLanguageFrench   , cifTop.getString(kGFF4CampaignCIFDisplayNameFRFR));
	_name.setString(Aurora::kLanguageGerman   , cifTop.getString(kGFF4CampaignCIFDisplayNameDEDE));
	_name.setString(Aurora::kLanguagePolish   , cifTop.getString(kGFF4CampaignCIFDisplayNamePLPL));
	_name.setString(Aurora::kLanguageRussian  , cifTop.getString(kGFF4CampaignCIFDisplayNameRURU));
	_name.setString(Aurora::kLanguageItalian  , cifTop.getString(kGFF4CampaignCIFDisplayNameITIT));
	_name.setString(Aurora::kLanguageSpanish  , cifTop.getString(kGFF4CampaignCIFDisplayNameESES));
	_name.setString(Aurora::kLanguageHungarian, cifTop.getString(kGFF4CampaignCIFDisplayNameHUHU));
	_name.setString(Aurora::kLanguageCzech    , cifTop.getString(kGFF4CampaignCIFDisplayNameCSCZ));

	_description.setString(Aurora::kLanguageEnglish  , cifTop.getString(kGFF4CampaignCIFDescriptionENUS));
	_description.setString(Aurora::kLanguageFrench   , cifTop.getString(kGFF4CampaignCIFDescriptionFRFR));
	_description.setString(Aurora::kLanguageGerman   , cifTop.getString(kGFF4CampaignCIFDescriptionDEDE));
	_description.setString(Aurora::kLanguagePolish   , cifTop.getString(kGFF4CampaignCIFDescriptionPLPL));
	_description.setString(Aurora::kLanguageRussian  , cifTop.getString(kGFF4CampaignCIFDescriptionRURU));
	_description.setString(Aurora::kLanguageItalian  , cifTop.getString(kGFF4CampaignCIFDescriptionITIT));
	_description.setString(Aurora::kLanguageSpanish  , cifTop.getString(kGFF4CampaignCIFDescriptionESES));
	_description.setString(Aurora::kLanguageHungarian, cifTop.getString(kGFF4CampaignCIFDescriptionHUHU));
	_description.setString(Aurora::kLanguageCzech    , cifTop.getString(kGFF4CampaignCIFDescriptionCSCZ));

	_entryArea         = cifTop.getString(kGFF4CampaignCIFEntryArea);
	_entryAreaList     = cifTop.getString(kGFF4CampaignCIFEntryAreaList);
	_entryScript       = cifTop.getString(kGFF4CampaignCIFEntryScript);
	_entryClientScript = cifTop.getString(kGFF4CampaignCIFEntryClientScript);

	cifTop.getVector3(kGFF4CampaignCIFEntryPosition,
			_entryPosition[0]   , _entryPosition[1]   , _entryPosition[2]);
	cifTop.getVector3(kGFF4CampaignCIFEntryOrientation,
			_entryOrientation[0], _entryOrientation[1], _entryOrientation[2]);

	cifTop.getString(kGFF4CampaignCIFPackagesList, _packages);
}

Campaign::RIMNode *Campaign::readRIMs(const GFF4Struct &node, const RIMNode *parent) {
	std::unique_ptr<RIMNode> rim = std::make_unique<RIMNode>(parent);

	// General node information
	rim->tag  = node.getString(kGFF4RimTreeNodeTag);
	rim->area = node.getString(kGFF4RimTreeNodeResRef);

	// RIM files for this node
	std::vector<Common::UString> rims;
	node.getString(kGFF4RimTreeRimList, rims);

	if (!rims.empty()) {
		if (rims.size() != 2)
			throw Common::Exception("RIMList of node \"%s\" (\"%s\") with length != 2 (%u)",
			                        rim->tag.c_str(), rim->area.c_str(), (uint) rims.size());

		rim->environment = rims[0];
		rim->rim         = rims[1];
	}

	// Decend into child nodes
	const GFF4List &children = node.getList(kGFF4RimTreeChildList);
	rim->children.reserve(children.size());

	for (GFF4List::const_iterator c = children.begin(); c != children.end(); ++c)
		if (*c)
			rim->children.push_back(readRIMs(**c, rim.get()));

	// If this node defines an area, remember it
	if (!rim->area.empty()) {
		_areaMap.insert(std::make_pair(rim->area, rim.get()));

		_areas.push_back(rim->area);
	}

	return rim.release();
}

void Campaign::readManifest(const Common::UString &path) {
	if (path.empty())
		return;

	Common::ReadFile  manifest(path);
	Common::XMLParser xml(manifest, true, path);

	const Common::XMLNode &root = xml.getRoot();

	if ((root.getName() != "manifest") || (root.getProperty("type") != "AddIn"))
		throw Common::Exception("manifest.xml is not an AddIn manifest: \"%s\", \"%s\"",
				root.getName().c_str(), root.getProperty("type").c_str());

	const Common::XMLNode *addinList = root.findChild("addinslist");
	if (!addinList)
		throw Common::Exception("Manifest has no AddInList");

	const Common::XMLNode *addinItem = 0;

	const Common::XMLNode::Children &addinItems = addinList->getChildren();
	for (Common::XMLNode::Children::const_iterator c = addinItems.begin(); c != addinItems.end(); ++c) {
		if ((*c)->getName() == "addinitem") {
			if (addinItem)
				throw Common::Exception("Manifest has more than one AddInItem");

			addinItem = *c;
		}
	}

	if (!addinItem)
		throw Common::Exception("Manifest has no AddInItem");

	_uid     = addinItem->getProperty("uid");
	_tag     = addinItem->getProperty("name");
	_extends = addinItem->getProperty("extendedmoduleuid");

	Common::parseString(addinItem->getProperty("enabled"              , "0"), _enabled);
	Common::parseString(addinItem->getProperty("bioware"              , "0"), _bioware);
	Common::parseString(addinItem->getProperty("requiresauthorization", "0"), _needsAuth);

	Common::parseString(addinItem->getProperty("priority", "0xFFFFFFFF"), _priority);

	Common::parseString(addinItem->getProperty("state" , "0xFFFFFFFF"), _state);
	Common::parseString(addinItem->getProperty("format", "0xFFFFFFFF"), _format);
}

bool Campaign::isEnabled() const {
	return _enabled;
}

bool Campaign::isBioWare() const {
	return _bioware;
}

bool Campaign::needsAuth() const {
	return _needsAuth;
}

const Common::UString &Campaign::getAreaRIM(const Common::UString &area) const {
	static const Common::UString kEmptyString;

	AreaMap::const_iterator a = _areaMap.find(area);
	if (a != _areaMap.end())
		return a->second->rim;

	return kEmptyString;
}

const std::vector<Common::UString> &Campaign::getAreas() const {
	return _areas;
}

Area *Campaign::getCurrentArea() const {
	return _currentArea.get();
}

Creature *Campaign::getPC() const {
	return _pc;
}

bool Campaign::isLoaded() const {
	return _loaded;
}

void Campaign::loadResources() {
	_game->loadTexturePack("/packages/core"        ,   0, _resources, kTextureQualityHigh);
	_game->loadTexturePack("/modules/single player", 500, _resources, kTextureQualityHigh);

	for (size_t i = 0; i < _packages.size(); i++) {
		const Common::UString dir = "/packages/" + _packages[i];

		_game->loadResources  (dir, 1000 + i * 500, _resources);
		_game->loadTalkTables (dir, 1000 + i * 500, _tlks);
		_game->loadTexturePack(dir, 1000 + i * 500, _resources, kTextureQualityHigh);
	}

	if (!_addinBase.empty()) {
		_game->loadResources  ("/addins/" + _addinBase + "/core"   , 10000, _resources);
		_game->loadTalkTables ("/addins/" + _addinBase + "/core"   , 10000, _tlks);
		_game->loadTexturePack("/addins/" + _addinBase + "/core"   , 10000, _resources, kTextureQualityHigh);

		_game->loadResources  ("/addins/" + _addinBase + "/module", 10500, _resources);
		_game->loadTalkTables ("/addins/" + _addinBase + "/module", 10500, _tlks);
		_game->loadTexturePack("/addins/" + _addinBase + "/module", 10500, _resources, kTextureQualityHigh);
	}
}

void Campaign::readCIFDynamic(const Common::UString &path) {
	Common::ReadFile *cifFile = new Common::ReadFile(path);

	GFF4File cif(cifFile, kCIFID);
	if (cif.getTypeVersion() != kVersion01)
		throw Common::Exception("Unsupported CIF version %s", Common::debugTag(cif.getTypeVersion()).c_str());

	const GFF4Struct &cifTop = cif.getTopLevel();

	const GFF4Struct *rimRoot = cifTop.getStruct(kGFF4RimTreeRootNode);
	if (rimRoot)
		_rimRoot.reset(readRIMs(*rimRoot));

	readVarTable(cifTop);
	readScript(cifTop);
	enableEvents(true);
}

void Campaign::load() {
	status("Loading campaign \"%s\" (\"%s\", \"%s\")", _tag.c_str(), _uid.c_str(), _name.getString().c_str());

	loadResources();
	readCIFDynamic(_cifPath);

	_loaded = true;

	_newArea = _entryArea;
	loadArea();
}

void Campaign::unload() {
	leave();

	_currentArea.reset();
	_rimRoot.reset();

	_areaMap.clear();
	_areas.clear();

	clearObjects();
	TwoDAReg.clear();

	_game->unloadTalkTables(_tlks);

	deindexResources(_resources);

	_loaded = false;
}

void Campaign::enterArea(bool startArea) {
	if (!_currentArea)
		return;

	float entryPosX, entryPosY, entryPosZ, entryOrientX, entryOrientY, entryOrientZ;

	if (startArea) {
		entryPosX = _entryPosition[0];
		entryPosY = _entryPosition[1];
		entryPosZ = _entryPosition[2];

		entryOrientX = _entryOrientation[0];
		entryOrientY = _entryOrientation[1];
		entryOrientZ = _entryOrientation[2];
	} else {
		float orientX, orientY, orientZ, orientAngle;

		_currentArea->getEntryLocation(entryPosX, entryPosY, entryPosZ, orientX, orientY, orientZ, orientAngle);

		entryOrientX = 0.0f;
		entryOrientY = 0.0f;
		entryOrientZ = 0.0f;
	}

	CameraMan.reset();
	CameraMan.setPosition(entryPosX, entryPosY, entryPosZ + 1.8f);
	CameraMan.setOrientation(entryOrientX + 90.0f, entryOrientY, entryOrientZ);
	CameraMan.update();

	_eventQueue.clear();

	_currentArea->runScript(kEventTypeEnter              , _currentArea.get(), _pc);
	_currentArea->runScript(kEventTypeAreaLoadPreLoadExit, _currentArea.get());

	_currentArea->show();

	_currentArea->runScript(kEventTypeAreaLoadPostLoadExit, _currentArea.get());
	_currentArea->runScript(kEventTypeAreaLoadSpecial     , _currentArea.get());

	status("Entered area \"%s\" (\"%s\")", _currentArea->getTag().c_str(), _currentArea->getName().getString().c_str());
}

void Campaign::enter(Creature &pc) {
	if (!isLoaded())
		throw Common::Exception("Campaign::enter(): Not loaded?!?");

	runScript(kEventTypeModuleStart      , this);
	runScript(kEventTypeGameObjectsLoaded, this);

	_pc = &pc;

	runScript(kEventTypeEnter, this, _pc);

	enterArea(true);
}

void Campaign::leave() {
	leaveArea();

	if (_pc)
		runScript(kEventTypeExit, this, _pc);

	_pc = 0;
}

void Campaign::leaveArea() {
	if (!_currentArea)
		return;

	if (_pc)
		_currentArea->runScript(kEventTypeExit, _currentArea.get(), _pc);

	_currentArea->hide();
}

void Campaign::unloadArea() {
	leaveArea();

	_currentArea.reset();

	clearObjects();
}

void Campaign::loadArea() {
	unloadArea();
	if (_newArea.empty())
		return;

	AreaMap::const_iterator area = _areaMap.find(_newArea);
	if (area == _areaMap.end())
		throw Common::Exception("Area \"%s\" does not exist in this campaign", _newArea.c_str());

	_currentArea = std::make_unique<Area>(*this, area->second->area, area->second->environment, area->second->rim);
}

bool Campaign::changeArea() {
	if (_currentArea && (_currentArea->getResRef() == _newArea))
		return true;

	loadArea();
	if (!_currentArea)
		return false;

	enterArea();
	return true;
}

void Campaign::addEvent(const Events::Event &event) {
	_eventQueue.push_back(event);
}

void Campaign::processEventQueue() {
	if (!isLoaded())
		return;

	changeArea();

	if (!isLoaded())
		return;

	handleEvents();
}

void Campaign::handleEvents() {
	for (EventQueue::const_iterator event = _eventQueue.begin(); event != _eventQueue.end(); ++event)
		_currentArea->addEvent(*event);

	_eventQueue.clear();

	_currentArea->processEventQueue();
}

void Campaign::movePC(const Common::UString &area) {
	_newArea = area;
}

void Campaign::movePC(float x, float y, float z) {
	if (_pc)
		_pc->setPosition(x, y, z);

	// Roughly head position
	CameraMan.setPosition(x, y, z + 1.8f);
	CameraMan.update();
}

void Campaign::movePC(const Common::UString &area, float x, float y, float z) {
	movePC(area);
	movePC(x, y, z);
}

} // End of namespace DragonAge

} // End of namespace Engines

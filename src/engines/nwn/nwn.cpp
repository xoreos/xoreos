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
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/nwn.cpp
 *  Engine class handling Neverwinter Nights.
 */

#include <list>

#include "common/util.h"
#include "common/filelist.h"
#include "common/filepath.h"
#include "common/stream.h"
#include "common/configman.h"

#include "aurora/resman.h"
#include "aurora/talkman.h"

#include "sound/sound.h"

#include "events/events.h"

#include "engines/aurora/util.h"
#include "engines/aurora/tokenman.h"
#include "engines/aurora/resources.h"

#include "engines/nwn/nwn.h"

namespace Engines {

namespace NWN {

const NWNEngineProbe kNWNEngineProbe;

const Common::UString NWNEngineProbe::kGameName = "Neverwinter Nights";

NWNEngineProbe::NWNEngineProbe() {
}

NWNEngineProbe::~NWNEngineProbe() {
}

Aurora::GameID NWNEngineProbe::getGameID() const {
	return Aurora::kGameIDNWN;
}

const Common::UString &NWNEngineProbe::getGameName() const {
	return kGameName;
}

bool NWNEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// Don't accidentally trigger on NWN2
	if (rootFiles.contains(".*/nwn2.ini", true))
		return false;
	if (rootFiles.contains(".*/nwn2main.exe", true))
		return false;

	// If either the ini file or a binary is found, this should be a valid path

	if (rootFiles.contains(".*/nwn.ini", true))
		return true;

	if (rootFiles.contains(".*/(nw|nwn)main.exe", true))
		return true;

	if (rootFiles.contains(".*/(nw|nwn)main", true))
		return true;

	return false;
}

bool NWNEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *NWNEngineProbe::createEngine() const {
	return new NWNEngine;
}


NWNEngine::NWNEngine() : _hasXP1(false), _hasXP2(false), _hasXP3(false) {
}

NWNEngine::~NWNEngine() {
}

void NWNEngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();
	if (EventMan.quitRequested())
		return;

	status("Successfully initialized the engine");

	playMenuMusic();

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	stopMenuMusic();
	deinit();
}

void NWNEngine::init() {
	initConfig();
	checkConfig();

	if (EventMan.quitRequested())
		return;

	initResources();

	if (EventMan.quitRequested())
		return;

	initCursors();

	if (EventMan.quitRequested())
		return;

	initGameConfig();
}

void NWNEngine::initResources() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);
	indexMandatoryDirectory("", 0, 0, 1);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "nwm");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "hak");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "texturepacks");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 1);

	status("Loading expansions and patch KEYs");

	// Base game patch
	indexOptionalArchive(Aurora::kArchiveKEY, "patch.key", 2);

	// Expansion 1: Shadows of Undrentide (SoU)
	_hasXP1 = indexOptionalArchive(Aurora::kArchiveKEY, "xp1.key", 3);
	indexOptionalArchive(Aurora::kArchiveKEY, "xp1patch.key", 4);

	// Expansion 2: Hordes of the Underdark (HotU)
	_hasXP2 = indexOptionalArchive(Aurora::kArchiveKEY, "xp2.key", 5);
	indexOptionalArchive(Aurora::kArchiveKEY, "xp2patch.key", 6);

	// Expansion 3: Kingmaker (resources also included in the final 1.69 patch)
	_hasXP3 = indexOptionalArchive(Aurora::kArchiveKEY, "xp3.key", 7);
	indexOptionalArchive(Aurora::kArchiveKEY, "xp3patch.key", 8);

	status("Loading GUI textures");
	indexMandatoryArchive(Aurora::kArchiveERF, "gui_32bit.erf"   , 10);
	indexOptionalArchive (Aurora::kArchiveERF, "xp1_gui.erf"     , 11);
	indexOptionalArchive (Aurora::kArchiveERF, "xp2_gui.erf"     , 12);

	status("Indexing extra sound resources");
	indexMandatoryDirectory("ambient"   , 0, 0, 20);
	status("Indexing extra music resources");
	indexMandatoryDirectory("music"     , 0, 0, 21);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"    , 0, 0, 22);
	status("Indexing extra image resources");
	indexOptionalDirectory ("portraits" , 0, 0, 23);
	status("Indexing extra talktables");
	indexOptionalDirectory ("tlk"       , 0, 0, 25);
	status("Indexing databases");
	indexOptionalDirectory ("database"  , 0, 0, 26);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 1000);

	if (EventMan.quitRequested())
		return;

	// Blacklist the DDS version of the galahad14 font, because in versions of NWN coming
	// with a Cyrillic one, the DDS file is still Latin.
	ResMan.blacklist("fnt_galahad14", Aurora::kFileTypeDDS);

	status("Loading main talk table");
	TalkMan.addMainTable("dialog");
}

void NWNEngine::initCursors() {
}

void NWNEngine::initConfig() {
	ConfigMan.setInt(Common::kConfigRealmDefault, "menufogcount" ,   4);
	ConfigMan.setInt(Common::kConfigRealmDefault, "texturepack"  ,   1);
	ConfigMan.setInt(Common::kConfigRealmDefault, "difficulty"   ,   0);
	ConfigMan.setInt(Common::kConfigRealmDefault, "feedbackmode" ,   2);
	ConfigMan.setInt(Common::kConfigRealmDefault, "tooltipdelay" , 100);

	ConfigMan.setBool(Common::kConfigRealmDefault, "largefonts"       , false);
	ConfigMan.setBool(Common::kConfigRealmDefault, "mouseoverfeedback", true);
}

void NWNEngine::initGameConfig() {
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP1", _hasXP1);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP2", _hasXP2);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN_hasXP3", _hasXP3);

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_extraModuleDir",
		Common::FilePath::findSubDirectory(_baseDirectory, "modules", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_campaignDir",
		Common::FilePath::findSubDirectory(_baseDirectory, "nwm", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_localPCDir",
		Common::FilePath::findSubDirectory(_baseDirectory, "localvault", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN_serverPCDir",
		Common::FilePath::findSubDirectory(_baseDirectory, "servervault", true));

	TokenMan.set("<StartCheck>"    , "<cFF0000FF>");
	TokenMan.set("<StartAction>"   , "<c00FF00FF>");
	TokenMan.set("<StartHighlight>", "<c0000FFFF>");
	TokenMan.set("</Start>"        , "</c>");

	// TODO: <PlayerName>
}

void NWNEngine::checkConfig() {
	checkConfigInt("menufogcount" ,   0,    5,   4);
	checkConfigInt("texturepack"  ,   0,    3,   1);
	checkConfigInt("difficulty"   ,   0,    3,   0);
	checkConfigInt("feedbackmode" ,   0,    2,   2);
	checkConfigInt("tooltipdelay" , 100, 2700, 100);
}

void NWNEngine::deinit() {
}

void NWNEngine::playIntroVideos() {
	playVideo("atarilogo");
	playVideo("biowarelogo");
	playVideo("wotclogo");
	playVideo("fge_logo_black");
	playVideo("nwnintro");
}

void NWNEngine::playMenuMusic() {
	if (SoundMan.isPlaying(_menuMusic))
		return;

	_menuMusic = _hasXP2 ?
		playSound("mus_x2theme"   , Sound::kSoundTypeMusic, true) :
		playSound("mus_theme_main", Sound::kSoundTypeMusic, true);
}

void NWNEngine::stopMenuMusic() {
	SoundMan.stopChannel(_menuMusic);
}

void NWNEngine::mainMenuLoop() {
	playMenuMusic();

	// Start sound
	playSound("gui_prompt", Sound::kSoundTypeSFX);

	stopMenuMusic();
}

void NWNEngine::getModules(std::vector<Common::UString> &modules) {
	modules.clear();

	Common::UString moduleDir = ConfigMan.getString("NWN_extraModuleDir");
	if (moduleDir.empty())
		return;

	Common::FileList moduleDirList;
	moduleDirList.addDirectory(moduleDir);

	std::list<Common::UString> mods;
	uint n = moduleDirList.getFileNames(mods);

	mods.sort(Common::UString::iless());

	modules.reserve(n);
	for (std::list<Common::UString>::const_iterator m = mods.begin(); m != mods.end(); ++m) {
		if (!Common::FilePath::getExtension(*m).equalsIgnoreCase(".mod"))
			continue;

		modules.push_back(Common::FilePath::getStem(*m));
	}
}

bool NWNEngine::hasModule(Common::UString &module) {
	const Common::UString nwmFile = module + ".nwm";
	const Common::UString modFile = module + ".mod";

	if (ResMan.hasArchive(Aurora::kArchiveERF, nwmFile)) {
		module = nwmFile;
		return true;
	}

	if (ResMan.hasArchive(Aurora::kArchiveERF, modFile)) {
		module = modFile;
		return true;
	}

	return false;
}

void NWNEngine::getCharacters(std::vector<Common::UString> &characters, bool local) {
	characters.clear();

	Common::UString pcDir = ConfigMan.getString(local ? "NWN_localPCDir" : "NWN_serverPCDir");
	if (pcDir.empty())
		return;

	Common::FileList pcDirList;
	pcDirList.addDirectory(pcDir);

	std::list<Common::UString> chars;
	uint n = pcDirList.getFileNames(chars);

	characters.reserve(n);
	for (std::list<Common::UString>::const_iterator c = chars.begin(); c != chars.end(); ++c) {
		if (!Common::FilePath::getExtension(*c).equalsIgnoreCase(".bic"))
			continue;

		characters.push_back(Common::FilePath::getStem(*c));
	}
}

} // End of namespace NWN

} // End of namespace Engines

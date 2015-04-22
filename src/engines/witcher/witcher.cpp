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
 *  Engine class handling The Witcher
 */

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"
#include "src/common/stream.h"
#include "src/common/configman.h"

#include "src/aurora/talkman.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"
#include "src/graphics/aurora/fps.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/language.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/witcher/witcher.h"
#include "src/engines/witcher/modelloader.h"
#include "src/engines/witcher/console.h"
#include "src/engines/witcher/campaign.h"

namespace Engines {

namespace Witcher {

const WitcherEngineProbe kWitcherEngineProbe;

const Common::UString WitcherEngineProbe::kGameName = "The Witcher";

WitcherEngineProbe::WitcherEngineProbe() {
}

WitcherEngineProbe::~WitcherEngineProbe() {
}

Aurora::GameID WitcherEngineProbe::getGameID() const {
	return Aurora::kGameIDWitcher;
}

const Common::UString &WitcherEngineProbe::getGameName() const {
	return kGameName;
}

bool WitcherEngineProbe::probe(const Common::UString &directory,
                               const Common::FileList &UNUSED(rootFiles)) const {

	// There should be a system directory
	Common::UString systemDir = Common::FilePath::findSubDirectory(directory, "system", true);
	if (systemDir.empty())
		return false;

	// The system directory has to be readable
	Common::FileList systemFiles;
	if (!systemFiles.addDirectory(systemDir))
		return false;

	// If either witcher.ini or witcher.exe exists, this should be a valid path
	return systemFiles.containsGlob(".*/witcher.(exe|ini)", true);
}

bool WitcherEngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *WitcherEngineProbe::createEngine() const {
	return new WitcherEngine;
}


WitcherEngine::WitcherEngine() : _fps(0) {
}

WitcherEngine::~WitcherEngine() {
}

void WitcherEngine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	if (ConfigMan.getBool("showfps", false)) {
		_fps = new Graphics::Aurora::FPS(FontMan.get(Graphics::Aurora::kSystemFontMono, 13));
		_fps->show();
	}

	main();

	deinit();
}

void WitcherEngine::init() {
	LoadProgress progress(14);

	progress.step("Loading user game config");
	initConfig();

	progress.step("Declare string encodings");
	declareEncodings();

	initResources(progress);
	if (EventMan.quitRequested())
		return;

	progress.step("Loading game cursors");
	initCursors();
	if (EventMan.quitRequested())
		return;

	progress.step("Initializing internal game config");
	initGameConfig();

	progress.step("Successfully initialized the engine");
}

void WitcherEngine::declareEncodings() {
	static const LanguageEncoding kLanguageEncodings[] = {
		{ Aurora::kLanguageEnglish           , Common::kEncodingUTF8 },
		{ Aurora::kLanguagePolish            , Common::kEncodingUTF8 },
		{ Aurora::kLanguageGerman            , Common::kEncodingUTF8 },
		{ Aurora::kLanguageFrench            , Common::kEncodingUTF8 },
		{ Aurora::kLanguageSpanish           , Common::kEncodingUTF8 },
		{ Aurora::kLanguageItalian           , Common::kEncodingUTF8 },
		{ Aurora::kLanguageRussian           , Common::kEncodingUTF8 },
		{ Aurora::kLanguageCzech             , Common::kEncodingUTF8 },
		{ Aurora::kLanguageHungarian         , Common::kEncodingUTF8 },
		{ Aurora::kLanguageKorean            , Common::kEncodingUTF8 },
		{ Aurora::kLanguageChineseTraditional, Common::kEncodingUTF8 },
		{ Aurora::kLanguageChineseSimplified , Common::kEncodingUTF8 }
	};

	Engines::declareEncodings(_game, kLanguageEncodings, ARRAYSIZE(kLanguageEncodings));
}

void WitcherEngine::initResources(LoadProgress &progress) {
	progress.step("Setting base directory");
	ResMan.registerDataBaseDir(_target);

	progress.step("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveEXE, "system");
	ResMan.addArchiveDir(Aurora::kArchiveKEY, "data");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data/voices");

	ResMan.addArchiveDir(Aurora::kArchiveERF, "data/modules", true);

	progress.step("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "main.key", 1);

	progress.step("Loading the localized base KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "localized.key", 10);

	progress.step("Indexing extra resources");
	indexOptionalDirectory("data/movies"   , 0, -1, 30);
	indexOptionalDirectory("data/music"    , 0, -1, 31);
	indexOptionalDirectory("data/sounds"   , 0, -1, 32);
	indexOptionalDirectory("data/cutscenes", 0, -1, 33);
	indexOptionalDirectory("data/dialogues", 0, -1, 34);
	indexOptionalDirectory("data/fx"       , 0, -1, 35);
	indexOptionalDirectory("data/meshes"   , 0, -1, 36);
	indexOptionalDirectory("data/quests"   , 0, -1, 37);
	indexOptionalDirectory("data/scripts"  , 0, -1, 38);
	indexOptionalDirectory("data/templates", 0, -1, 39);
	indexOptionalDirectory("data/textures" , 0, -1, 40);

	indexOptionalDirectory("data", ".*\\.bik", 0, 41);
	indexOptionalDirectory("data", ".*\\.tlk", 0, 42);

	progress.step("Indexing Windows-specific resources");
	indexMandatoryArchive(Aurora::kArchiveEXE, "witcher.exe", 43);

	progress.step("Indexing override files");
	indexOptionalDirectory("data/override", 0, 0, 50);

	loadLanguageFiles(progress, Aurora::kLanguageEnglish, Aurora::kLanguageEnglish);

	progress.step("Registering file formats");
	registerModelLoader(new WitcherModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTTF);
}

void WitcherEngine::initCursors() {
	CursorMan.add("cursor0" , "default"  , "up"  );
	CursorMan.add("cursor1" , "default"  , "down");

	CursorMan.setDefault("default", "up");
}

void WitcherEngine::initConfig() {
}

void WitcherEngine::initGameConfig() {
	ConfigMan.setString(Common::kConfigRealmGameTemp, "WITCHER_moduleDir",
		Common::FilePath::findSubDirectory(_target, "data/modules", true));
}

void WitcherEngine::unloadLanguageFiles() {
	TalkMan.removeMainTable();
}

void WitcherEngine::loadLanguageFiles(LoadProgress &progress,
		Aurora::Language langText, Aurora::Language langVoice) {

	progress.step(Common::UString::sprintf("Index language files (%s text + %s voices)",
				Aurora::getLanguageName(langText).c_str(), Aurora::getLanguageName(langVoice).c_str()));

	loadLanguageFiles(langText, langVoice);
}

void WitcherEngine::loadLanguageFiles(Aurora::Language langText, Aurora::Language langVoice) {
	unloadLanguageFiles();

	Common::UString archive;

	Aurora::ResourceManager::ChangeID change;

	archive = Common::UString::sprintf("lang_%d.key", getLanguageID(_game, langVoice));
	indexMandatoryArchive(Aurora::kArchiveKEY, archive, 20, &change);
	_languageResources.push_back(change);
	change.clear();

	archive = Common::UString::sprintf("M1_%d.key", getLanguageID(_game, langVoice));
	indexMandatoryArchive(Aurora::kArchiveKEY, archive, 21, &change);
	_languageResources.push_back(change);
	change.clear();

	archive = Common::UString::sprintf("M2_%d.key", getLanguageID(_game, langVoice));
	indexMandatoryArchive(Aurora::kArchiveKEY, archive, 22, &change);
	_languageResources.push_back(change);
	change.clear();

	archive = Common::UString::sprintf("dialog_%d", getLanguageID(_game, langText));
	TalkMan.addMainTable(archive);
}

void WitcherEngine::deinit() {
	delete _fps;
}

void WitcherEngine::playIntroVideos() {
	playVideo("publisher");
	playVideo("developer");
	playVideo("engine");
	playVideo("intro");
	playVideo("title");
}

void WitcherEngine::main() {
	Console console;
	Campaign campaign(console);

	const std::list<CampaignDescription> &campaigns = campaign.getCampaigns();
	if (campaigns.empty())
		error("No campaigns found");

	// Find the original The Witcher campaign
	const CampaignDescription *witcherCampaign = 0;
	for (std::list<CampaignDescription>::const_iterator c = campaigns.begin(); c != campaigns.end(); ++c)
		if (c->tag == "thewitcher")
			witcherCampaign = &*c;

	// If that's not available, load the first one found
	if (!witcherCampaign)
		witcherCampaign = &*campaigns.begin();

	campaign.load(*witcherCampaign);

	campaign.run();
}

} // End of namespace Witcher

} // End of namespace Engines

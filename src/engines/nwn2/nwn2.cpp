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
 *  Engine class handling Neverwinter Nights 2.
 */

#include <cassert>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filepath.h"
#include "src/common/filelist.h"
#include "src/common/readstream.h"
#include "src/common/configman.h"

#include "src/aurora/util.h"
#include "src/aurora/resman.h"
#include "src/aurora/language.h"
#include "src/aurora/talkman.h"
#include "src/aurora/talktable_tlk.h"

#include "src/graphics/aurora/cursorman.h"
#include "src/graphics/aurora/fontman.h"

#include "src/events/events.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"
#include "src/engines/aurora/model.h"

#include "src/engines/nwn2/nwn2.h"
#include "src/engines/nwn2/modelloader.h"
#include "src/engines/nwn2/console.h"
#include "src/engines/nwn2/game.h"

namespace Engines {

namespace NWN2 {

NWN2Engine::NWN2Engine() : _language(Aurora::kLanguageInvalid),
	_hasXP1(false), _hasXP2(false), _hasXP3(false) {

	_console.reset(new Console(*this));
}

NWN2Engine::~NWN2Engine() {
}

bool NWN2Engine::detectLanguages(Aurora::GameID UNUSED(game), const Common::UString &target,
                                 Aurora::Platform UNUSED(platform),
                                 std::vector<Aurora::Language> &languages) const {
	try {
		Common::FileList files;
		if (!files.addDirectory(target))
			return true;

		Common::UString tlk = files.findFirst("dialog.tlk", true);
		if (tlk.empty())
			return true;

		uint32 languageID = Aurora::TalkTable_TLK::getLanguageID(tlk);
		if (languageID == Aurora::kLanguageInvalid)
			return true;

		Aurora::Language language = LangMan.getLanguage(languageID);
		if (language == Aurora::kLanguageInvalid)
			return true;

		languages.push_back(language);

	} catch (...) {
	}

	return true;
}

bool NWN2Engine::getLanguage(Aurora::Language &language) const {
	language = _language;
	return true;
}

bool NWN2Engine::changeLanguage() {
	Aurora::Language language;
	if (!evaluateLanguage(false, language) || (_language != language))
		return false;

	return true;
}

Game &NWN2Engine::getGame() {
	assert(_game);

	return *_game;
}

void NWN2Engine::run() {
	init();
	if (EventMan.quitRequested())
		return;

	CursorMan.hideCursor();
	CursorMan.set();

	playIntroVideos();
	if (EventMan.quitRequested())
		return;

	CursorMan.showCursor();

	_game.reset(new Game(*this, *_console));
	_game->run();

	deinit();
}

void NWN2Engine::init() {
	LoadProgress progress(21);

	progress.step("Declare languages");
	declareLanguages();

	if (evaluateLanguage(true, _language))
		status("Setting the language to %s", LangMan.getLanguageName(_language).c_str());
	else
		warning("Failed to detect this game's language");

	LangMan.setCurrentLanguage(_language);

	progress.step("Loading user game config");
	initConfig();

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

void NWN2Engine::declareLanguages() {
	static const Aurora::LanguageManager::Declaration kLanguageDeclarations[] = {
		{ Aurora::kLanguageEnglish           ,   0, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageFrench            ,   1, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageGerman            ,   2, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageItalian           ,   3, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguageSpanish           ,   4, Common::kEncodingCP1252, Common::kEncodingCP1252 },
		{ Aurora::kLanguagePolish            ,   5, Common::kEncodingCP1250, Common::kEncodingCP1250 },
		{ Aurora::kLanguageKorean            , 128, Common::kEncodingCP949 , Common::kEncodingCP949  },
		{ Aurora::kLanguageChineseTraditional, 129, Common::kEncodingCP950 , Common::kEncodingCP950  },
		{ Aurora::kLanguageChineseSimplified , 130, Common::kEncodingCP936 , Common::kEncodingCP936  },
		{ Aurora::kLanguageJapanese          , 131, Common::kEncodingCP932 , Common::kEncodingCP932  }
	};

	LangMan.addLanguages(kLanguageDeclarations, ARRAYSIZE(kLanguageDeclarations));
}

void NWN2Engine::initResources(LoadProgress &progress) {
	// NWN2's new file types overlap with other engines in the 3000s block
	ResMan.addTypeAlias((Aurora::FileType) 3000, Aurora::kFileTypeOSC);
	ResMan.addTypeAlias((Aurora::FileType) 3001, Aurora::kFileTypeUSC);
	ResMan.addTypeAlias((Aurora::FileType) 3002, Aurora::kFileTypeTRN);
	ResMan.addTypeAlias((Aurora::FileType) 3003, Aurora::kFileTypeUTR);
	ResMan.addTypeAlias((Aurora::FileType) 3004, Aurora::kFileTypeUEN);
	ResMan.addTypeAlias((Aurora::FileType) 3005, Aurora::kFileTypeULT);
	ResMan.addTypeAlias((Aurora::FileType) 3006, Aurora::kFileTypeSEF);
	ResMan.addTypeAlias((Aurora::FileType) 3007, Aurora::kFileTypePFX);
	ResMan.addTypeAlias((Aurora::FileType) 3008, Aurora::kFileTypeCAM);
	ResMan.addTypeAlias((Aurora::FileType) 3009, Aurora::kFileTypeLFX);
	ResMan.addTypeAlias((Aurora::FileType) 3010, Aurora::kFileTypeBFX);
	ResMan.addTypeAlias((Aurora::FileType) 3011, Aurora::kFileTypeUPE);
	ResMan.addTypeAlias((Aurora::FileType) 3012, Aurora::kFileTypeROS);
	ResMan.addTypeAlias((Aurora::FileType) 3013, Aurora::kFileTypeRST);
	ResMan.addTypeAlias((Aurora::FileType) 3014, Aurora::kFileTypeIFX);
	ResMan.addTypeAlias((Aurora::FileType) 3015, Aurora::kFileTypePFB);
	ResMan.addTypeAlias((Aurora::FileType) 3016, Aurora::kFileTypeZIP);
	ResMan.addTypeAlias((Aurora::FileType) 3017, Aurora::kFileTypeWMP);
	ResMan.addTypeAlias((Aurora::FileType) 3018, Aurora::kFileTypeBBX);
	ResMan.addTypeAlias((Aurora::FileType) 3019, Aurora::kFileTypeTFX);
	ResMan.addTypeAlias((Aurora::FileType) 3020, Aurora::kFileTypeWLK);
	ResMan.addTypeAlias((Aurora::FileType) 3021, Aurora::kFileTypeXML);
	ResMan.addTypeAlias((Aurora::FileType) 3022, Aurora::kFileTypeSCC);
	ResMan.addTypeAlias((Aurora::FileType) 3033, Aurora::kFileTypePTX);
	ResMan.addTypeAlias((Aurora::FileType) 3034, Aurora::kFileTypeLTX);
	ResMan.addTypeAlias((Aurora::FileType) 3035, Aurora::kFileTypeTRX);

	progress.step("Setting base directory");
	ResMan.registerDataBase(_target);

	progress.step("Adding extra archive directories");
	indexMandatoryDirectory("data"   , 0, 0, 2);
	indexMandatoryDirectory("modules", 0, 0, 3);
	indexMandatoryDirectory("hak"    , 0, 0, 4);

	progress.step("Loading main resource files");

	indexMandatoryArchive("2da.zip"           , 10);
	indexMandatoryArchive("actors.zip"        , 11);
	indexMandatoryArchive("animtags.zip"      , 12);
	indexMandatoryArchive("convo.zip"         , 13);
	indexMandatoryArchive("ini.zip"           , 14);
	indexMandatoryArchive("lod-merged.zip"    , 15);
	indexMandatoryArchive("music.zip"         , 16);
	indexMandatoryArchive("nwn2_materials.zip", 17);
	indexMandatoryArchive("nwn2_models.zip"   , 18);
	indexMandatoryArchive("nwn2_vfx.zip"      , 19);
	indexMandatoryArchive("prefabs.zip"       , 20);
	indexMandatoryArchive("scripts.zip"       , 21);
	indexMandatoryArchive("sounds.zip"        , 22);
	indexMandatoryArchive("soundsets.zip"     , 23);
	indexMandatoryArchive("speedtree.zip"     , 24);
	indexMandatoryArchive("templates.zip"     , 25);
	indexMandatoryArchive("vo.zip"            , 26);
	indexMandatoryArchive("walkmesh.zip"      , 27);

	progress.step("Loading expansion 1 resource files");

	// Expansion 1: Mask of the Betrayer (MotB)
	_hasXP1 = ResMan.hasArchive("2da_x1.zip");
	indexOptionalArchive("2da_x1.zip"           , 50);
	indexOptionalArchive("actors_x1.zip"        , 51);
	indexOptionalArchive("animtags_x1.zip"      , 52);
	indexOptionalArchive("convo_x1.zip"         , 53);
	indexOptionalArchive("ini_x1.zip"           , 54);
	indexOptionalArchive("lod-merged_x1.zip"    , 55);
	indexOptionalArchive("music_x1.zip"         , 56);
	indexOptionalArchive("nwn2_materials_x1.zip", 57);
	indexOptionalArchive("nwn2_models_x1.zip"   , 58);
	indexOptionalArchive("nwn2_vfx_x1.zip"      , 59);
	indexOptionalArchive("prefabs_x1.zip"       , 60);
	indexOptionalArchive("scripts_x1.zip"       , 61);
	indexOptionalArchive("soundsets_x1.zip"     , 62);
	indexOptionalArchive("sounds_x1.zip"        , 63);
	indexOptionalArchive("speedtree_x1.zip"     , 64);
	indexOptionalArchive("templates_x1.zip"     , 65);
	indexOptionalArchive("vo_x1.zip"            , 66);
	indexOptionalArchive("walkmesh_x1.zip"      , 67);

	progress.step("Loading expansion 2 resource files");

	// Expansion 2: Storm of Zehir (SoZ)
	_hasXP2 = ResMan.hasArchive("2da_x2.zip");
	indexOptionalArchive("2da_x2.zip"           , 100);
	indexOptionalArchive("actors_x2.zip"        , 101);
	indexOptionalArchive("animtags_x2.zip"      , 102);
	indexOptionalArchive("lod-merged_x2.zip"    , 103);
	indexOptionalArchive("music_x2.zip"         , 104);
	indexOptionalArchive("nwn2_materials_x2.zip", 105);
	indexOptionalArchive("nwn2_models_x2.zip"   , 106);
	indexOptionalArchive("nwn2_vfx_x2.zip"      , 107);
	indexOptionalArchive("prefabs_x2.zip"       , 108);
	indexOptionalArchive("scripts_x2.zip"       , 109);
	indexOptionalArchive("soundsets_x2.zip"     , 110);
	indexOptionalArchive("sounds_x2.zip"        , 111);
	indexOptionalArchive("speedtree_x2.zip"     , 112);
	indexOptionalArchive("templates_x2.zip"     , 113);
	indexOptionalArchive("vo_x2.zip"            , 114);

	// Expansion 3: Mysteries of Westgate
	_hasXP3 = ResMan.hasArchive("westgate.hak");

	progress.step("Loading patch resource files");

	indexOptionalArchive("actors_v103x1.zip"         , 150);
	indexOptionalArchive("actors_v106.zip"           , 151);
	indexOptionalArchive("lod-merged_v101.zip"       , 152);
	indexOptionalArchive("lod-merged_v107.zip"       , 153);
	indexOptionalArchive("lod-merged_v121.zip"       , 154);
	indexOptionalArchive("lod-merged_x1_v121.zip"    , 155);
	indexOptionalArchive("lod-merged_x2_v121.zip"    , 156);
	indexOptionalArchive("nwn2_materials_v103x1.zip" , 157);
	indexOptionalArchive("nwn2_materials_v104.zip"   , 158);
	indexOptionalArchive("nwn2_materials_v106.zip"   , 159);
	indexOptionalArchive("nwn2_materials_v107.zip"   , 160);
	indexOptionalArchive("nwn2_materials_v110.zip"   , 161);
	indexOptionalArchive("nwn2_materials_v112.zip"   , 162);
	indexOptionalArchive("nwn2_materials_v121.zip"   , 163);
	indexOptionalArchive("nwn2_materials_x1_v113.zip", 164);
	indexOptionalArchive("nwn2_materials_x1_v121.zip", 165);
	indexOptionalArchive("nwn2_models_v103x1.zip"    , 166);
	indexOptionalArchive("nwn2_models_v104.zip"      , 167);
	indexOptionalArchive("nwn2_models_v105.zip"      , 168);
	indexOptionalArchive("nwn2_models_v106.zip"      , 169);
	indexOptionalArchive("nwn2_models_v107.zip"      , 160);
	indexOptionalArchive("nwn2_models_v112.zip"      , 171);
	indexOptionalArchive("nwn2_models_v121.zip"      , 172);
	indexOptionalArchive("nwn2_models_x1_v121.zip"   , 173);
	indexOptionalArchive("nwn2_models_x2_v121.zip"   , 174);
	indexOptionalArchive("templates_v112.zip"        , 175);
	indexOptionalArchive("templates_v122.zip"        , 176);
	indexOptionalArchive("templates_x1_v122.zip"     , 177);
	indexOptionalArchive("vo_103x1.zip"              , 178);
	indexOptionalArchive("vo_106.zip"                , 179);

	progress.step("Indexing extra sound resources");
	indexMandatoryDirectory("ambient"   , 0,  0, 200);
	indexOptionalDirectory ("ambient_x1", 0,  0, 201);
	indexOptionalDirectory ("ambient_x2", 0,  0, 202);
	progress.step("Indexing extra music resources");
	indexMandatoryDirectory("music"     , 0,  0, 203);
	indexOptionalDirectory ("music_x1"  , 0,  0, 204);
	indexOptionalDirectory ("music_x2"  , 0,  0, 205);
	progress.step("Indexing extra movie resources");
	indexMandatoryDirectory("movies"    , 0,  0, 206);
	progress.step("Indexing extra effects resources");
	indexMandatoryDirectory("effects"   , 0,  0, 207);
	progress.step("Indexing extra character resources");
	indexMandatoryDirectory("localvault", 0,  0, 208);
	progress.step("Indexing extra UI resources");
	indexMandatoryDirectory("ui"        , 0, -1, 209);

	progress.step("Indexing Windows-specific resources");
	indexMandatoryArchive("nwn2main.exe", 250);

	progress.step("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 500);

	progress.step("Loading main talk table");
	TalkMan.addTable("dialog", "dialogf", false, 0);

	progress.step("Registering file formats");
	registerModelLoader(new NWN2ModelLoader);
	FontMan.setFormat(Graphics::Aurora::kFontFormatTTF);
}

void NWN2Engine::initCursors() {
	CursorMan.add("cursor0" , "default"  , "up"  );
	CursorMan.add("cursor1" , "default"  , "down");

	CursorMan.setDefault("default", "up");
}

void NWN2Engine::initConfig() {
	// Enable/Disable the Proof-of-Concept software tinting
	ConfigMan.setBool(Common::kConfigRealmDefault, "tint", true);
}

void NWN2Engine::initGameConfig() {
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN2_hasXP1", _hasXP1);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN2_hasXP2", _hasXP2);
	ConfigMan.setBool(Common::kConfigRealmGameTemp, "NWN2_hasXP3", _hasXP3);

	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN2_campaignDir",
		Common::FilePath::findSubDirectory(_target, "campaigns", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN2_moduleDir",
		Common::FilePath::findSubDirectory(_target, "modules", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN2_localPCDir",
		Common::FilePath::findSubDirectory(_target, "localvault", true));
	ConfigMan.setString(Common::kConfigRealmGameTemp, "NWN2_serverPCDir",
		Common::FilePath::findSubDirectory(_target, "servervault", true));
}

void NWN2Engine::deinit() {
	_game.reset();
}

void NWN2Engine::playIntroVideos() {
	playVideo("atarilogo");
	playVideo("oeilogo");
	playVideo("wotclogo");
	playVideo("nvidialogo");
	playVideo("legal");
	playVideo("intro");
}

} // End of namespace NWN2

} // End of namespace Engines

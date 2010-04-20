/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn2/nwn2.cpp
 *  Engine class handling Neverwinter Nights 2
 */

#include "engines/nwn2/nwn2.h"

#include "common/util.h"
#include "common/strutil.h"
#include "common/filelist.h"
#include "common/stream.h"

#include "graphics/graphics.h"
#include "graphics/cube.h"
#include "graphics/font.h"
#include "graphics/text.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"

namespace NWN2 {

const NWN2EngineProbe kNWN2EngineProbe;

const std::string NWN2EngineProbe::kGameName = "Neverwinter Nights 2";

Aurora::GameID NWN2EngineProbe::getGameID() const {
	return Aurora::kGameIDNWN2;
}

const std::string &NWN2EngineProbe::getGameName() const {
	return kGameName;
}

bool NWN2EngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	// If either the ini file or the binary is found, this should be a valid path
	if (rootFiles.contains(".*/nwn2.ini", true))
		return true;
	if (rootFiles.contains(".*/nwn2main.exe", true))
		return true;

	return false;
}

Engines::Engine *NWN2EngineProbe::createEngine() const {
	return new NWN2Engine;
}


NWN2Engine::NWN2Engine() {
}

NWN2Engine::~NWN2Engine() {
}

void NWN2Engine::run(const std::string &directory) {
	_baseDirectory = directory;

	init();

	status("Successfully initialized the engine");

	playVideo("atarilogo");
	playVideo("oeilogo");
	playVideo("wotclogo");
	playVideo("nvidialogo");
	playVideo("legal");

	int channel = -1;

	Common::SeekableReadStream *wav = ResMan.getMusic("mus_mulsantir");
	if (wav) {
		// Cutting off the long silence at the end of mus_mulsantir :P
		wav = new Common::SeekableSubReadStream(wav, 0, 3545548, DisposeAfterUse::YES);

		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav, true);
	}

	Graphics::Cube *cube = 0;

	try {

		cube = new Graphics::Cube("wt_lake01_n");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
}

void NWN2Engine::init() {
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Loading main resource files");

	indexMandatoryZIP("2da.zip"           ,  0);
	indexMandatoryZIP("actors.zip"        ,  1);
	indexMandatoryZIP("animtags.zip"      ,  2);
	indexMandatoryZIP("convo.zip"         ,  3);
	indexMandatoryZIP("ini.zip"           ,  4);
	indexMandatoryZIP("lod-merged.zip"    ,  5);
	indexMandatoryZIP("music.zip"         ,  6);
	indexMandatoryZIP("nwn2_materials.zip",  7);
	indexMandatoryZIP("nwn2_models.zip"   ,  8);
	indexMandatoryZIP("nwn2_vfx.zip"      ,  9);
	indexMandatoryZIP("prefabs.zip"       , 10);
	indexMandatoryZIP("scripts.zip"       , 11);
	indexMandatoryZIP("sounds.zip"        , 12);
	indexMandatoryZIP("soundsets.zip"     , 13);
	indexMandatoryZIP("speedtree.zip"     , 14);
	indexMandatoryZIP("templates.zip"     , 15);
	indexMandatoryZIP("vo.zip"            , 16);
	indexMandatoryZIP("walkmesh.zip"      , 17);

	status("Loading expansions resource files");

	// Expansion 1: Mask of the Betrayer (MotB)
	indexOptionalZIP("2da_x1.zip"           , 20);
	indexOptionalZIP("actors_x1.zip"        , 21);
	indexOptionalZIP("animtags_x1.zip"      , 22);
	indexOptionalZIP("convo_x1.zip"         , 23);
	indexOptionalZIP("ini_x1.zip"           , 24);
	indexOptionalZIP("lod-merged_x1.zip"    , 25);
	indexOptionalZIP("music_x1.zip"         , 26);
	indexOptionalZIP("nwn2_materials_x1.zip", 27);
	indexOptionalZIP("nwn2_models_x1.zip"   , 28);
	indexOptionalZIP("nwn2_models_x2.zip"   , 29);
	indexOptionalZIP("nwn2_vfx_x1.zip"      , 30);
	indexOptionalZIP("prefabs_x1.zip"       , 31);
	indexOptionalZIP("scripts_x1.zip"       , 32);
	indexOptionalZIP("soundsets_x1.zip"     , 33);
	indexOptionalZIP("sounds_x1.zip"        , 34);
	indexOptionalZIP("speedtree_x1.zip"     , 35);
	indexOptionalZIP("templates_x1.zip"     , 36);
	indexOptionalZIP("vo_x1.zip"            , 37);
	indexOptionalZIP("walkmesh_x1.zip"      , 38);

	// Expansion 2: Storm of Zehir (SoZ)
	indexOptionalZIP("2da_x2.zip"           , 40);
	indexOptionalZIP("actors_x2.zip"        , 41);
	indexOptionalZIP("animtags_x2.zip"      , 42);
	indexOptionalZIP("lod-merged_x2.zip"    , 43);
	indexOptionalZIP("music_x2.zip"         , 44);
	indexOptionalZIP("nwn2_materials_x2.zip", 45);
	indexOptionalZIP("nwn2_vfx_x2.zip"      , 46);
	indexOptionalZIP("prefabs_x2.zip"       , 47);
	indexOptionalZIP("scripts_x2.zip"       , 48);
	indexOptionalZIP("soundsets_x2.zip"     , 49);
	indexOptionalZIP("sounds_x2.zip"        , 50);
	indexOptionalZIP("speedtree_x2.zip"     , 51);
	indexOptionalZIP("templates_x2.zip"     , 52);
	indexOptionalZIP("vo_x2.zip"            , 53);

	warning("TODO: Mysteries of Westgate (MoW) resource files");
	warning("TODO: Patch resource files");

	status("Finding further resource archives directories");
	ResMan.findSourceDirs();

	status("Loading secondary resources");
	ResMan.loadSecondaryResources(60);

	status("Loading override files");
	ResMan.loadOverrideFiles(70);
}

} // End of namespace NWN2

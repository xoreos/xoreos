/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor/kotor.cpp
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#include "engines/kotor/kotor.h"
#include "engines/kotor/area.h"

#include "engines/util.h"

#include "common/util.h"
#include "common/strutil.h"
#include "common/filelist.h"
#include "common/stream.h"

#include "graphics/graphics.h"

#include "graphics/aurora/cube.h"
#include "graphics/aurora/font.h"
#include "graphics/aurora/fontman.h"
#include "graphics/aurora/text.h"
#include "graphics/aurora/model_kotor.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/lytfile.h"
#include "aurora/resman.h"
#include "aurora/error.h"

namespace Engines {

namespace KotOR {

const KotOREngineProbe kKotOREngineProbe;

const Common::UString KotOREngineProbe::kGameName = "Star Wars: Knights of the Old Republic";

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const Common::UString &KotOREngineProbe::getGameName() const {
	return kGameName;
}

bool KotOREngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If either swkotor.ini or swkotor.exe exists, this should be a valid path
	return rootFiles.contains(".*/swkotor.(exe|ini)", true);
}

bool KotOREngineProbe::probe(Common::SeekableReadStream &stream) const {
	return false;
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine;
}


KotOREngine::KotOREngine() {
}

KotOREngine::~KotOREngine() {
}

void KotOREngine::run(const Common::UString &target) {
	_baseDirectory = target;

	init();

	status("Successfully initialized the engine");

	playVideo("leclogo");
	playVideo("biologo");
	playVideo("legal");

	playVideo("01a");

	Sound::ChannelHandle channel;

	Common::SeekableReadStream *wav = ResMan.getResource(Aurora::kResourceSound, "nm35aahhkd07134_");
	if (wav) {
		status("Found a wav. Trying to play it. Turn up your speakers");
		channel = SoundMan.playSoundFile(wav, Sound::kSoundTypeVoice);

		SoundMan.startChannel(channel);
	}

	// Test load up the Taris cantina

	indexMandatoryArchive(Aurora::kArchiveRIM, "tar_m03ae.rim", 100);

	Area *tarisCantina = new Area;

	tarisCantina->load("m03ae");
	tarisCantina->setPosition(-110.0, -110.0, 0.0);
	tarisCantina->show();

	Graphics::Aurora::FontHandle font = FontMan.get("dialogfont32x32");

	float textX = (-(GfxMan.getScreenWidth()  / 2.0))                             / 100.0;
	float textY = ( (GfxMan.getScreenHeight() / 2.0) - font.getFont().getScale()) / 100.0;

	Graphics::Aurora::Text *text = new Graphics::Aurora::Text(font, "");
	text->setPosition(textX, textY);
	text->show();

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);

		text->set(Common::UString::sprintf("%d fps", GfxMan.getFPS()));
	}

	delete tarisCantina;

	delete text;
}

void KotOREngine::init() {
	status("Setting base directory");
	ResMan.registerDataBaseDir(_baseDirectory);

	status("Adding extra archive directories");
	ResMan.addArchiveDir(Aurora::kArchiveBIF, "data");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "lips");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "texturepacks");
	ResMan.addArchiveDir(Aurora::kArchiveERF, "modules");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "rims");
	ResMan.addArchiveDir(Aurora::kArchiveRIM, "modules");

	status("Loading main KEY");
	indexMandatoryArchive(Aurora::kArchiveKEY, "chitin.key", 0);

	status("Loading global auxiliary resources");
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenu.rim"  , 10);
	indexMandatoryArchive(Aurora::kArchiveRIM, "mainmenudx.rim", 11);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legal.rim"     , 12);
	indexMandatoryArchive(Aurora::kArchiveRIM, "legaldx.rim"   , 13);
	indexMandatoryArchive(Aurora::kArchiveRIM, "global.rim"    , 14);
	indexMandatoryArchive(Aurora::kArchiveRIM, "globaldx.rim"  , 15);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargen.rim"   , 16);
	indexMandatoryArchive(Aurora::kArchiveRIM, "chargendx.rim" , 17);

	status("Loading high-res texture packs");
	indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_gui.erf", 20);
	indexMandatoryArchive(Aurora::kArchiveERF, "swpc_tex_tpa.erf", 21);

	status("Indexing extra sound resources");
	indexMandatoryDirectory("streamsounds", 0, -1, 30);
	status("Indexing extra voice resources");
	indexMandatoryDirectory("streamwaves" , 0, -1, 31);
	status("Indexing extra music resources");
	indexMandatoryDirectory("streammusic" , 0, -1, 32);
	status("Indexing extra movie resources");
	indexMandatoryDirectory("movies"      , 0, -1, 33);

	status("Indexing override files");
	indexOptionalDirectory("override", 0, 0, 40);
}

Graphics::Aurora::Model *KotOREngine::loadModel(const Common::UString &resref) {
	Common::SeekableReadStream *mdl = 0, *mdx = 0;

	Graphics::Aurora::Model *model = 0;

	try {
		mdl = ResMan.getResource(resref, Aurora::kFileTypeMDL);
		if (!mdl)
			throw Common::Exception("No such model");

		mdx = ResMan.getResource(resref, Aurora::kFileTypeMDX);
		if (!mdx)
			throw Common::Exception("No such model data");

		model = new Graphics::Aurora::Model_KotOR(*mdl, *mdx, false);
	} catch(...) {
		delete mdl;
		delete mdx;
		delete model;
		throw;
	}

	delete mdl;
	delete mdx;
	return model;
}

} // End of namespace KotOR

} // End of namespace Engines

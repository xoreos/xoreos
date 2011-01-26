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
#include "engines/kotor/util.h"
#include "engines/kotor/module.h"

#include "engines/util.h"

#include "common/util.h"
#include "common/strutil.h"
#include "common/filelist.h"
#include "common/filepath.h"
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

const KotOREngineProbeWin kKotOREngineProbeWin;
const KotOREngineProbeMac kKotOREngineProbeMac;

const Common::UString KotOREngineProbe::kGameName = "Star Wars: Knights of the Old Republic";

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const Common::UString &KotOREngineProbe::getGameName() const {
	return kGameName;
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine;
}

bool KotOREngineProbeWin::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If either swkotor.ini or swkotor.exe exists, this should be a valid path for the Windows port
	return rootFiles.contains(".*/swkotor.(exe|ini)", true);
}

bool KotOREngineProbeMac::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "Knights of the Old Republic.app" directory exists, this should be a valid path for the Mac OS X port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app");
	return !appDirectory.empty();
}


KotOREngine::KotOREngine() {
	_modelLoader = new KotORModelLoader;
}

KotOREngine::~KotOREngine() {
	delete _modelLoader;
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

	Module *tarisCantina = new Module(*_modelLoader);

	tarisCantina->load("tar_m03ae");
	tarisCantina->enter();

	Graphics::Aurora::FontHandle font = FontMan.get("dialogfont32x32");

	float textX = (-(GfxMan.getScreenWidth()  / 2.0))                             / 100.0;
	float textY = ( (GfxMan.getScreenHeight() / 2.0) - font.getFont().getScale()) / 100.0;

	Graphics::Aurora::Text *text = new Graphics::Aurora::Text(font, "");
	text->setPosition(textX, textY);
	text->show();

	EventMan.enableKeyRepeat();

	status("Entering event loop");

	while (!EventMan.quitRequested()) {
		Events::Event event;
		while (EventMan.pollEvent(event)) {
			if (event.type == Events::kEventKeyDown) {
				if      (event.key.keysym.sym == SDLK_UP)
					tarisCantina->move  ( 0.5);
				else if (event.key.keysym.sym == SDLK_DOWN)
					tarisCantina->move  (-0.5);
				else if (event.key.keysym.sym == SDLK_LEFT)
					tarisCantina->turn  ( 0.0, -5.0,  0.0);
				else if (event.key.keysym.sym == SDLK_RIGHT)
					tarisCantina->turn  ( 0.0,  5.0,  0.0);
				else if (event.key.keysym.sym == SDLK_PAGEUP)
					tarisCantina->turn  (-5.0,  0.0,  0.0);
				else if (event.key.keysym.sym == SDLK_PAGEDOWN)
					tarisCantina->turn  ( 5.0,  0.0,  0.0);
				else if (event.key.keysym.sym == SDLK_INSERT)
					tarisCantina->move  ( 0.0,  0.0,  0.5);
				else if (event.key.keysym.sym == SDLK_DELETE)
					tarisCantina->move  ( 0.0,  0.0, -0.5);
				else if (event.key.keysym.sym == SDLK_w)
					tarisCantina->move  ( 0.5);
				else if (event.key.keysym.sym == SDLK_s)
					tarisCantina->move  (-0.5);
				else if (event.key.keysym.sym == SDLK_a)
					tarisCantina->strafe(-0.5);
				else if (event.key.keysym.sym == SDLK_d)
					tarisCantina->strafe( 0.5);
				else if (event.key.keysym.sym == SDLK_q)
					tarisCantina->turn  ( 0.0, -5.0,  0.0);
				else if (event.key.keysym.sym == SDLK_e)
					tarisCantina->turn  ( 0.0,  5.0,  0.0);
				else if (event.key.keysym.sym == SDLK_END) {
					const float *orient = tarisCantina->getOrientation();

					tarisCantina->setOrientation(0.0, orient[1], orient[2]);
				} else if (event.key.keysym.sym == SDLK_t) {
					const float *pos    = tarisCantina->getPosition();
					const float *orient = tarisCantina->getOrientation();

					warning("%+8.3f %+8.3f %+8.3f; %+8.3f %+8.3f %+8.3f", pos[0], pos[1], pos[2], orient[0], orient[1], orient[2]);
				}
			}
		}

		EventMan.delay(10);

		text->set(Common::UString::sprintf("%d fps", GfxMan.getFPS()));
	}

	tarisCantina->leave();

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

} // End of namespace KotOR

} // End of namespace Engines

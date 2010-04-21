/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/sonic/sonic.cpp
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#include "engines/sonic/sonic.h"

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

namespace Sonic {

const SonicEngineProbe kSonicEngineProbe;

const std::string SonicEngineProbe::kGameName = "Sonic Chronicles: The Dark Brotherhood";

Aurora::GameID SonicEngineProbe::getGameID() const {
	return Aurora::kGameIDSonic;
}

const std::string &SonicEngineProbe::getGameName() const {
	return kGameName;
}

bool SonicEngineProbe::probe(const std::string &directory, const Common::FileList &rootFiles) const {
	return false;
}

bool SonicEngineProbe::probe(Common::SeekableReadStream *stream) const {
	delete stream;
	return false;
}

Engines::Engine *SonicEngineProbe::createEngine() const {
	return new SonicEngine;
}


SonicEngine::SonicEngine() {
}

SonicEngine::~SonicEngine() {
}

void SonicEngine::run(const std::string &target) {
	init();

	status("Successfully initialized the engine");
}

void SonicEngine::init() {
}

} // End of namespace Sonic

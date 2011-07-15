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

/** @file engines/sonic/sonic.cpp
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#include "engines/sonic/sonic.h"

#include "engines/aurora/util.h"
#include "engines/aurora/resources.h"

#include "common/util.h"
#include "common/filelist.h"
#include "common/stream.h"
#include "common/strutil.h"

#include "graphics/graphics.h"

#include "graphics/aurora/cube.h"
#include "graphics/aurora/text.h"

#include "sound/sound.h"

#include "events/events.h"

#include "aurora/resman.h"
#include "aurora/error.h"
#include "aurora/ndsrom.h"

namespace Engines {

namespace Sonic {

const SonicEngineProbe kSonicEngineProbe;

const Common::UString SonicEngineProbe::kGameName = "Sonic Chronicles: The Dark Brotherhood";

SonicEngineProbe::SonicEngineProbe() {
}

SonicEngineProbe::~SonicEngineProbe() {
}

Aurora::GameID SonicEngineProbe::getGameID() const {
	return Aurora::kGameIDSonic;
}

const Common::UString &SonicEngineProbe::getGameName() const {
	return kGameName;
}

bool SonicEngineProbe::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	return false;
}

bool SonicEngineProbe::probe(Common::SeekableReadStream &stream) const {
	return Aurora::NDSFile::isNDS(stream);
}

Engines::Engine *SonicEngineProbe::createEngine() const {
	return new SonicEngine;
}


SonicEngine::SonicEngine() {
}

SonicEngine::~SonicEngine() {
}

void SonicEngine::run(const Common::UString &target) {
	_romFile = target;

	init();

	status("Successfully initialized the engine");

	playIntroVideos();
}

void SonicEngine::init() {
	status("Indexing the ROM file");
	indexMandatoryArchive(Aurora::kArchiveNDS, _romFile, 0);
}

void SonicEngine::playIntroVideos() {
	// Play the two logo videos
	playVideo("bioware");
	playVideo("sega");

	// TODO: We need to support playing two videos at once. The two logo videos
	// are both on the bottom screen, but (most) other videos have a top screen
	// and bottom screen video.
}

} // End of namespace Sonic

} // End of namespace Engines

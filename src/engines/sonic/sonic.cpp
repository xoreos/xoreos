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
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#include "src/engines/sonic/sonic.h"

#include "src/engines/aurora/util.h"
#include "src/engines/aurora/loadprogress.h"
#include "src/engines/aurora/resources.h"

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/filelist.h"
#include "src/common/stream.h"
#include "src/common/strutil.h"
#include "src/common/configman.h"

#include "src/graphics/graphics.h"

#include "src/graphics/aurora/cube.h"
#include "src/graphics/aurora/fontman.h"

#include "src/sound/sound.h"

#include "src/events/events.h"

#include "src/aurora/resman.h"
#include "src/aurora/ndsrom.h"

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

bool SonicEngineProbe::probe(const Common::UString &UNUSED(directory),
                             const Common::FileList &UNUSED(rootFiles)) const {

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

void SonicEngine::run() {
	init();

	playIntroVideos();

	Graphics::Aurora::Cube *cube = 0;
	try {

		cube = new Graphics::Aurora::Cube("nintendosplash");

	} catch (Common::Exception &e) {
		Common::printException(e);
	}

	while (!EventMan.quitRequested()) {
		EventMan.delay(10);
	}

	delete cube;
}

void SonicEngine::init() {
	LoadProgress progress(3);

	ResMan.setHashAlgo(Common::kHashDJB2);

	progress.step("Indexing the ROM file");
	indexMandatoryArchive(Aurora::kArchiveNDS, _target, 1);
	progress.step("Indexing the main HERF file");
	indexMandatoryArchive(Aurora::kArchiveHERF, "test.herf", 2);

	ResMan.declareResource("nintendosplash.tga");

	progress.step("Successfully initialized the engine");
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

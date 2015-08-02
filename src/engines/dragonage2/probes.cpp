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
 *  Probing for an installation of Dragon Age II.
 */

#include "src/common/filelist.h"

#include "src/engines/dragonage2/probes.h"
#include "src/engines/dragonage2/dragonage2.h"

namespace Engines {

namespace DragonAge2 {

const DragonAge2EngineProbe kDragonAge2EngineProbe;

const Engines::EngineProbe * const kProbes[] = {
	&kDragonAge2EngineProbe,
	0
};


const Common::UString DragonAge2EngineProbe::kGameName = "Dragon Age II";

DragonAge2EngineProbe::DragonAge2EngineProbe() {
}

DragonAge2EngineProbe::~DragonAge2EngineProbe() {
}

Aurora::GameID DragonAge2EngineProbe::getGameID() const {
	return Aurora::kGameIDDragonAge2;
}

const Common::UString &DragonAge2EngineProbe::getGameName() const {
	return kGameName;
}

bool DragonAge2EngineProbe::probe(const Common::UString &UNUSED(directory),
                                  const Common::FileList &rootFiles) const {

	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains("/dragonage2launcher.exe", true))
		return true;

	return false;
}

bool DragonAge2EngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *DragonAge2EngineProbe::createEngine() const {
	return new DragonAge2Engine;
}

} // End of namespace DragonAge2

} // End of namespace Engines

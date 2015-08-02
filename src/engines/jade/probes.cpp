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
 *  Probing for an installation of Jade Empire.
 */

#include "src/common/filelist.h"

#include "src/engines/jade/probes.h"
#include "src/engines/jade/jade.h"

namespace Engines {

namespace Jade {

const JadeEngineProbe kJadeEngineProbe;

const Engines::EngineProbe * const kProbes[] = {
	&kJadeEngineProbe,
	0
};


const Common::UString JadeEngineProbe::kGameName = "Jade Empire";

JadeEngineProbe::JadeEngineProbe() {
}

JadeEngineProbe::~JadeEngineProbe() {
}

Aurora::GameID JadeEngineProbe::getGameID() const {
	return Aurora::kGameIDJade;
}

const Common::UString &JadeEngineProbe::getGameName() const {
	return kGameName;
}

bool JadeEngineProbe::probe(const Common::UString &UNUSED(directory),
                            const Common::FileList &rootFiles) const {

	// If the launcher binary is found, this should be a valid path
	if (rootFiles.contains("/JadeEmpire.exe", true))
		return true;

	return false;
}

bool JadeEngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *JadeEngineProbe::createEngine() const {
	return new JadeEngine;
}

} // End of namespace Jade

} // End of namespace Engines

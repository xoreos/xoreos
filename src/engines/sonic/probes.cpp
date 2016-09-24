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
 *  Probing for an installation of Sonic Chronicles: The Dark Brotherhood.
 */

#include "src/common/ustring.h"
#include "src/common/filelist.h"

#include "src/aurora/ndsrom.h"

#include "src/engines/engineprobe.h"

#include "src/engines/sonic/probes.h"
#include "src/engines/sonic/sonic.h"

namespace Engines {

namespace Sonic {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDSonic;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformNDS;
	}

	Engines::Engine *createEngine() const {
		return new SonicEngine;
	}

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &UNUSED(rootFiles)) const {
		return false;
	}

	bool probe(Common::SeekableReadStream &stream) const {
		Common::UString title, code, maker;
		if (!Aurora::NDSFile::isNDS(stream, title, code, maker))
			return false;

		return title == "SONICCHRON";
	}

};

const Common::UString EngineProbe::kGameName = "Sonic Chronicles: The Dark Brotherhood";


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbe);
}

} // End of namespace Sonic

} // End of namespace Engines

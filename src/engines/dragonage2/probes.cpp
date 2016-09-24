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

#include "src/common/ustring.h"
#include "src/common/filelist.h"

#include "src/engines/engineprobe.h"

#include "src/engines/dragonage2/probes.h"
#include "src/engines/dragonage2/dragonage2.h"

namespace Engines {

namespace DragonAge2 {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDDragonAge2;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformWindows;
	}

	Engines::Engine *createEngine() const {
		return new DragonAge2Engine;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {

		// If the launcher binary is found, this should be a valid path
		if (rootFiles.contains("/dragonage2launcher.exe", true))
			return true;

		return false;
	}

};

const Common::UString EngineProbe::kGameName = "Dragon Age II";


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbe);
}

} // End of namespace DragonAge2

} // End of namespace Engines

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
 *  Probing for an installation of The Witcher.
 */

#include "src/common/ustring.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/engineprobe.h"

#include "src/engines/witcher/probes.h"
#include "src/engines/witcher/witcher.h"

namespace Engines {

namespace Witcher {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDWitcher;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformWindows;
	}

	Engines::Engine *createEngine() const {
		return new WitcherEngine;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	bool probe(const Common::UString &directory, const Common::FileList &UNUSED(rootFiles)) const {

		// There should be a system directory
		Common::UString systemDir = Common::FilePath::findSubDirectory(directory, "system", true);
		if (systemDir.empty())
			return false;

		// The system directory has to be readable
		Common::FileList systemFiles;
		if (!systemFiles.addDirectory(systemDir))
			return false;

		// If either witcher.ini or witcher.exe exists, this should be a valid path
		return systemFiles.containsGlob(".*/witcher.(exe|ini)", true);
	}

};

const Common::UString EngineProbe::kGameName = "The Witcher";


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbe);
}

} // End of namespace Witcher

} // End of namespace Engines

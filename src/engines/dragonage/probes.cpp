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
 *  Probing for an installation of Dragon Age: Origins.
 */

#include "src/common/ustring.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/engineprobe.h"

#include "src/engines/dragonage/probes.h"
#include "src/engines/dragonage/dragonage.h"

namespace Engines {

namespace DragonAge {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDDragonAge;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	Engines::Engine *createEngine() const {
		return new DragonAgeEngine;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}
};

class EngineProbeWindows : public EngineProbe {
public:
	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformWindows;
	}

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {
		// If the launcher binary is found, this should be a valid path
		if (rootFiles.contains("/daoriginslauncher.exe", true))
			return true;

		return false;
	}
};

class EngineProbeXbox360 : public EngineProbe {
public:
	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformXbox360;
	}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
		/* If we find the Xbox executable and a Dragon Age specific file,
		 * this should be a valid path. */
		if (rootFiles.contains("default.xex", true) &&
			Common::FileList(Common::FilePath::findSubDirectory(directory, "modules/single player", true)).contains("singleplayer.cif", true))
			return true;

		return false;
	}
};

const Common::UString EngineProbe::kGameName = "Dragon Age: Origins";


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbeWindows);
	probes.push_back(new EngineProbeXbox360);
}

} // End of namespace DragonAge

} // End of namespace Engines

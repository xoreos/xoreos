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
 *  Probing for an installation of Star Wars: Knights of the Old Republic.
 */

#include "src/common/ustring.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/engineprobe.h"

#include "src/engines/kotor/probes.h"
#include "src/engines/kotor/kotor.h"

namespace Engines {

namespace KotOR {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDKotOR;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	Engines::Engine *createEngine() const {
		return new KotOREngine;
	}
};

const Common::UString EngineProbe::kGameName = "Star Wars: Knights of the Old Republic";


class EngineProbeWindows : public EngineProbe {
public:
	EngineProbeWindows() {}
	~EngineProbeWindows() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {

		// If swkotor.exe exists, this should be a valid path for the Windows port
		return rootFiles.contains("/swkotor.exe", true);
	}

};

class EngineProbeMac : public EngineProbe {
public:
	EngineProbeMac() {}
	~EngineProbeMac() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }

	bool probe(const Common::UString &directory, const Common::FileList &UNUSED(rootFiles)) const {

		// If the "Knights of the Old Republic.app" directory exists,
		// this should be a valid path for the Mac OS X port
		const Common::UString appDirectory =
			Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app", true);

		return !appDirectory.empty();
	}

};

class EngineProbeXbox : public EngineProbe {
public:
	EngineProbeXbox() {}
	~EngineProbeXbox() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformXbox; }

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const {

		// If the "dataxbox" directory exists and "players.erf" exists,
		// this should be a valid path for the Xbox port
		const Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");

		return !appDirectory.empty() && rootFiles.contains("/players.erf", true);
	}

};


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbeWindows);
	probes.push_back(new EngineProbeMac);
	probes.push_back(new EngineProbeXbox);
}

} // End of namespace KotOR

} // End of namespace Engines

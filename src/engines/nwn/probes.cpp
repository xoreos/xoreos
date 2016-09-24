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
 *  Probing for an installation of Neverwinter Nights.
 */

#include "src/common/ustring.h"
#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/engineprobe.h"

#include "src/engines/nwn/probes.h"
#include "src/engines/nwn/nwn.h"

namespace Engines {

namespace NWN {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDNWN;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	Engines::Engine *createEngine() const {
		return new NWNEngine;
	}
};

const Common::UString EngineProbe::kGameName = "Neverwinter Nights";


class EngineProbeWindows : public EngineProbe {
public:
	EngineProbeWindows() {}
	~EngineProbeWindows() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {

		// Look for the Windows binary nwmain.exe
		return rootFiles.contains("/nwmain.exe", true);
	}

};

class EngineProbeMac : public EngineProbe {
public:
	EngineProbeMac() {}
	~EngineProbeMac() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }

	bool probe(const Common::UString &directory, const Common::FileList &UNUSED(rootFiles)) const {

		// Look for the app directory containing the Mac OS X binary
		return !Common::FilePath::findSubDirectory(directory, "Neverwinter Nights.app", true).empty();
	}

};

class EngineProbeLinux: public EngineProbe {
public:
	EngineProbeLinux() {}
	~EngineProbeLinux() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformLinux; }

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {

		// Look for the Linux binary nwmain
		return rootFiles.contains("/nwmain", true);
	}

};

class EngineProbeFallback : public EngineProbe {
public:
	EngineProbeFallback() {}
	~EngineProbeFallback() {}

	Aurora::Platform getPlatform() const { return Aurora::kPlatformUnknown; }

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {

		// Don't accidentally trigger on NWN2
		if (rootFiles.contains("/nwn2.ini", true))
			return false;
		if (rootFiles.contains("/nwn2main.exe", true))
			return false;

		// As a fallback, look for the nwn.ini, nwnplayer.ini or nwncdkey.ini
		return rootFiles.contains("/nwn.ini", true) ||
		       rootFiles.contains("/nwnplayer.ini", true) ||
		       rootFiles.contains("/nwncdkey.ini", true);
	}

};


void createEngineProbes(std::list<const ::Engines::EngineProbe *> &probes) {
	probes.push_back(new EngineProbeWindows);
	probes.push_back(new EngineProbeMac);
	probes.push_back(new EngineProbeLinux);
	probes.push_back(new EngineProbeFallback);
}

} // End of namespace NWN

} // End of namespace Engines

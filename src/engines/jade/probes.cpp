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

#include "src/common/ustring.h"
#include "src/common/filelist.h"

#include "src/engines/jade/probes.h"
#include "src/engines/jade/jade.h"

namespace Engines {

namespace Jade {

class EngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	EngineProbe() {}
	~EngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDJade;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	Engines::Engine *createEngine() const {
		return new JadeEngine;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}
};

class EngineProbeWin : public EngineProbe {
public:
	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformWindows;
	}

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {
		// If the launcher binary is found, this should be a valid path
                if (rootFiles.contains("/JadeEmpire.exe", true))
                        return true;

                return false;
	}
};

class EngineProbeXbox : public EngineProbe {
public:
	Aurora::Platform getPlatform() const {
		return Aurora::kPlatformXbox;
	}

	bool probe(const Common::UString &UNUSED(directory), const Common::FileList &rootFiles) const {
                if (rootFiles.contains("/jadedirs.ini", true))
                        return true;

                return false;
        }
};

static const EngineProbeWin kEngineProbeWin;
static const EngineProbeXbox kEngineProbeXbox;


const Common::UString EngineProbe::kGameName = "Jade Empire";


const Engines::EngineProbe * const kProbes[] = {
	&kEngineProbeWin,
	&kEngineProbeXbox,
	0
};

} // End of namespace Jade

} // End of namespace Engines

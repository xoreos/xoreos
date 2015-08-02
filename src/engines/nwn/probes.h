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

#ifndef ENGINES_NWN_PROBES_H
#define ENGINES_NWN_PROBES_H

#include "src/common/ustring.h"

#include "src/engines/engineprobe.h"

namespace Engines {

namespace NWN {

class NWNEngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	NWNEngineProbe() {}
	~NWNEngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDNWN;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	Engines::Engine *createEngine() const;
};

class NWNEngineProbeWindows : public NWNEngineProbe {
public:
	NWNEngineProbeWindows() {}
	~NWNEngineProbeWindows() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }
};

class NWNEngineProbeMac : public NWNEngineProbe {
public:
	NWNEngineProbeMac() {}
	~NWNEngineProbeMac() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }
};

class NWNEngineProbeLinux: public NWNEngineProbe {
public:
	NWNEngineProbeLinux() {}
	~NWNEngineProbeLinux() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformLinux; }
};

class NWNEngineProbeFallback : public NWNEngineProbe {
public:
	NWNEngineProbeFallback() {}
	~NWNEngineProbeFallback() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformUnknown; }
};

extern const Engines::EngineProbe * const kProbes[];

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_PROBES_H

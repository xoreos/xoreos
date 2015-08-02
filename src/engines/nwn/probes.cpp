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

#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/nwn/probes.h"
#include "src/engines/nwn/nwn.h"

namespace Engines {

namespace NWN {

const NWNEngineProbeWindows  kNWNEngineProbeWin;
const NWNEngineProbeMac      kNWNEngineProbeMac;
const NWNEngineProbeLinux    kNWNEngineProbeLinux;
const NWNEngineProbeFallback kNWNEngineProbeFallback;

const Engines::EngineProbe * const kProbes[] = {
	&kNWNEngineProbeWin,
	&kNWNEngineProbeMac,
	&kNWNEngineProbeLinux,
	&kNWNEngineProbeFallback,
	0
};

const Common::UString NWNEngineProbe::kGameName = "Neverwinter Nights";

Engines::Engine *NWNEngineProbe::createEngine() const {
	return new NWNEngine;
}

bool NWNEngineProbeWindows::probe(const Common::UString &UNUSED(directory),
                                  const Common::FileList &rootFiles) const {

	// Look for the Windows binary nwmain.exe
	return rootFiles.contains("/nwmain.exe", true);
}

bool NWNEngineProbeMac::probe(const Common::UString &directory,
                              const Common::FileList &UNUSED(rootFiles)) const {

	// Look for the app directory containing the Mac OS X binary
	return !Common::FilePath::findSubDirectory(directory, "Neverwinter Nights.app", true).empty();
}

bool NWNEngineProbeLinux::probe(const Common::UString &UNUSED(directory),
                                const Common::FileList &rootFiles) const {

	// Look for the Linux binary nwmain
	return rootFiles.contains("/nwmain", true);
}

bool NWNEngineProbeFallback::probe(const Common::UString &UNUSED(directory),
                                   const Common::FileList &rootFiles) const {

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

} // End of namespace NWN

} // End of namespace Engines

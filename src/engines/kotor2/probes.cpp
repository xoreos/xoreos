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
 *  Probing for an installation of Star Wars: Knights of the Old Republic II - The Sith Lords.
 */

#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/kotor2/probes.h"
#include "src/engines/kotor2/kotor2.h"

namespace Engines {

namespace KotOR2 {

const KotOR2EngineProbeWin   kKotOR2EngineProbeWin;
const KotOR2EngineProbeLinux kKotOR2EngineProbeLinux;
const KotOR2EngineProbeMac   kKotOR2EngineProbeMac;
const KotOR2EngineProbeXbox  kKotOR2EngineProbeXbox;

const Common::UString KotOR2EngineProbe::kGameName = "Star Wars: Knights of the Old Republic II - The Sith Lords";

KotOR2EngineProbe::KotOR2EngineProbe() {
}

KotOR2EngineProbe::~KotOR2EngineProbe() {
}

Aurora::GameID KotOR2EngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR2;
}

const Common::UString &KotOR2EngineProbe::getGameName() const {
	return kGameName;
}

bool KotOR2EngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *KotOR2EngineProbe::createEngine() const {
	return new KotOR2Engine;
}


KotOR2EngineProbeWin::KotOR2EngineProbeWin() {
}

KotOR2EngineProbeWin::~KotOR2EngineProbeWin() {
}

bool KotOR2EngineProbeWin::probe(const Common::UString &UNUSED(directory),
                                 const Common::FileList &rootFiles) const {

	// If "swkotor2.exe" exists, this should be a valid path for the Windows port
	return rootFiles.contains("/swkotor2.exe", true);
}


KotOR2EngineProbeLinux::KotOR2EngineProbeLinux() {
}

KotOR2EngineProbeLinux::~KotOR2EngineProbeLinux() {
}

bool KotOR2EngineProbeLinux::probe(const Common::UString &directory,
                                   const Common::FileList &rootFiles) const {

	// The game binary found in the Aspyr Linux port
	if (!rootFiles.contains("/KOTOR2", false))
		return false;

	// The directory containing what was originally within the PE resources
	if (Common::FilePath::findSubDirectory(directory, "resources").empty())
		return false;
	// The directory containing the original game data files
	if (Common::FilePath::findSubDirectory(directory, "steamassets").empty())
		return false;

	return true;
}


KotOR2EngineProbeMac::KotOR2EngineProbeMac() {
}

KotOR2EngineProbeMac::~KotOR2EngineProbeMac() {
}

bool KotOR2EngineProbeMac::probe(const Common::UString &directory,
                                 const Common::FileList &UNUSED(rootFiles)) const {

	// The directory containing the Mac binary
	if (Common::FilePath::findSubDirectory(directory, "MacOS").empty())
		return false;
	// The directory containing what was originally within the PE resources
	if (Common::FilePath::findSubDirectory(directory, "Resources").empty())
		return false;
	// The directory containing the original game data files
	if (Common::FilePath::findSubDirectory(directory, "GameData").empty())
		return false;

	// The game binary found in the Aspyr Mac port
	Common::FileList binaryFiles(Common::FilePath::findSubDirectory(directory, "MacOS"));
	if (!binaryFiles.contains("KOTOR2", false))
		return false;

	return true;
}


KotOR2EngineProbeXbox::KotOR2EngineProbeXbox() {
}

KotOR2EngineProbeXbox::~KotOR2EngineProbeXbox() {
}

bool KotOR2EngineProbeXbox::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "dataxbox" directory exists and "weapons.erf" exists, this should be a valid path for the Xbox port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");
	return !appDirectory.empty() && rootFiles.contains("/weapons.erf", true);
}

} // End of namespace KotOR2

} // End of namespace Engines

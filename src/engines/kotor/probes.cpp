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

#include "src/common/filelist.h"
#include "src/common/filepath.h"

#include "src/engines/kotor/probes.h"
#include "src/engines/kotor/kotor.h"

namespace Engines {

namespace KotOR {

const KotOREngineProbeWin  kKotOREngineProbeWin;
const KotOREngineProbeMac  kKotOREngineProbeMac;
const KotOREngineProbeXbox kKotOREngineProbeXbox;

const Common::UString KotOREngineProbe::kGameName = "Star Wars: Knights of the Old Republic";

KotOREngineProbe::KotOREngineProbe() {
}

KotOREngineProbe::~KotOREngineProbe() {
}

Aurora::GameID KotOREngineProbe::getGameID() const {
	return Aurora::kGameIDKotOR;
}

const Common::UString &KotOREngineProbe::getGameName() const {
	return kGameName;
}

bool KotOREngineProbe::probe(Common::SeekableReadStream &UNUSED(stream)) const {
	return false;
}

Engines::Engine *KotOREngineProbe::createEngine() const {
	return new KotOREngine;
}


KotOREngineProbeWin::KotOREngineProbeWin() {
}

KotOREngineProbeWin::~KotOREngineProbeWin() {
}

bool KotOREngineProbeWin::probe(const Common::UString &UNUSED(directory),
                                const Common::FileList &rootFiles) const {

	// If swkotor.exe exists, this should be a valid path for the Windows port
	return rootFiles.contains("/swkotor.exe", true);
}


KotOREngineProbeMac::KotOREngineProbeMac() {
}

KotOREngineProbeMac::~KotOREngineProbeMac() {
}

bool KotOREngineProbeMac::probe(const Common::UString &directory,
                                const Common::FileList &UNUSED(rootFiles)) const {

	// If the "Knights of the Old Republic.app" directory exists, this should be a valid path for the Mac OS X port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "Knights of the Old Republic.app", true);
	return !appDirectory.empty();
}


KotOREngineProbeXbox::KotOREngineProbeXbox() {
}

KotOREngineProbeXbox::~KotOREngineProbeXbox() {
}

bool KotOREngineProbeXbox::probe(const Common::UString &directory, const Common::FileList &rootFiles) const {
	// If the "dataxbox" directory exists and "players.erf" exists, this should be a valid path for the Xbox port
	Common::UString appDirectory = Common::FilePath::findSubDirectory(directory, "dataxbox");
	return !appDirectory.empty() && rootFiles.contains("/players.erf", true);
}

} // End of namespace KotOR

} // End of namespace Engines

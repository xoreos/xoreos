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

#ifndef ENGINES_DRAGONAGE_PROBES_H
#define ENGINES_DRAGONAGE_PROBES_H

#include "src/common/ustring.h"

#include "src/engines/engineprobe.h"

namespace Engines {

namespace DragonAge {

class DragonAgeEngineProbe : public Engines::EngineProbe {
public:
	DragonAgeEngineProbe();
	~DragonAgeEngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const Engines::EngineProbe * const kProbes[];

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_PROBES_H

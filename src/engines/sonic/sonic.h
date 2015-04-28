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
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#ifndef ENGINES_SONIC_SONIC_H
#define ENGINES_SONIC_SONIC_H

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

namespace Sonic {

class SonicEngineProbe : public Engines::EngineProbe {
public:
	SonicEngineProbe();
	~SonicEngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformNDS; }

private:
	static const Common::UString kGameName;
};

extern const SonicEngineProbe kSonicEngineProbe;

class SonicEngine : public Engines::Engine {
public:
	SonicEngine();
	~SonicEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;


protected:
	void run();


private:
	Common::UString _romFile;

	void init();

	void playIntroVideos();

	static Common::UString getLanguageHERF(Aurora::Language language);
	static Common::UString getLanguageTLK (Aurora::Language language);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_SONIC_H

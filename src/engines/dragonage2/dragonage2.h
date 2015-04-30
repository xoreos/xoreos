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
 *  Engine class handling Dragon Age II
 */

#ifndef ENGINES_DRAGONAGE2_DRAGONAGE2_H
#define ENGINES_DRAGONAGE2_DRAGONAGE2_H

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace DragonAge2 {

class DragonAge2EngineProbe : public Engines::EngineProbe {
public:
	DragonAge2EngineProbe();
	~DragonAge2EngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const DragonAge2EngineProbe kDragonAge2EngineProbe;

class DragonAge2Engine : public Engines::Engine {
public:
	DragonAge2Engine();
	~DragonAge2Engine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();


protected:
	void run();


private:
	Aurora::Language _language;

	void init();
	void declareEncodings();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language language);
	void loadLanguageFiles(Aurora::Language language);

	void deinit();

	void playIntroVideos();
	void main();

	static Common::UString getLanguageString(Aurora::Language language);
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_DRAGONAGE2_H

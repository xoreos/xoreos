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
 *  Engine class handling Dragon Age: Origins
 */

#ifndef ENGINES_DRAGONAGE_DRAGONAGE_H
#define ENGINES_DRAGONAGE_DRAGONAGE_H

#include <list>

#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace DragonAge {

class Campaigns;

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

extern const DragonAgeEngineProbe kDragonAgeEngineProbe;

class DragonAgeEngine : public Engines::Engine {
public:
	DragonAgeEngine();
	~DragonAgeEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	/** Return the campaigns context. */
	Campaigns &getCampaigns();


protected:
	void run();


private:
	Aurora::Language _language;

	std::list<Common::ChangeID> _languageTLK;

	Campaigns *_campaigns;


	void init();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void loadPatches(const Common::UString &dir, uint32 priority);

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language language);
	void loadLanguageFiles(Aurora::Language language);
	void loadTalkTable(const Common::UString &tlk, Aurora::Language language, uint32 priority);

	void deinit();

	void playIntroVideos();
	void main();

	static Common::UString getLanguageString(Aurora::Language language);
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_DRAGONAGE_H

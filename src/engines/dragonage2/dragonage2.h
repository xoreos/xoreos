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

#include <list>

#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

#include "src/engines/aurora/resources.h"

#include "src/engines/dragonage2/types.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace DragonAge2 {

class Campaigns;

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

	/** Return the campaigns context. */
	Campaigns &getCampaigns();

	void loadResources  (const Common::UString &dir, uint32 priority, ChangeList &res, ChangeList &tlk);
	void loadTexturePack(const Common::UString &dir, uint32 priority, ChangeList &res, TextureQuality quality);

	void unloadTalkTables(ChangeList &changes);

protected:
	void run();


private:
	Aurora::Language _language;

	ChangeList _resources;
	ChangeList _languageTLK;

	Campaigns *_campaigns;


	void init();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void loadTalkTables(const Common::UString &dir, uint32 priority, ChangeList &changes);
	void loadResourceDir(const Common::UString &dir, uint32 priority, ChangeList &changes);

	void loadTalkTable(const Common::UString &tlk, const Common::UString &suffix,
	                   Aurora::Language language, uint32 priority, ChangeList &changes);

	void deinit();

	void playIntroVideos();
	void main();

	static Common::UString getLanguageString(Aurora::Language language);
};

} // End of namespace DragonAge2

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_DRAGONAGE2_H

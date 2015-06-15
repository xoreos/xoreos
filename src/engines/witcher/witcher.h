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
 *  Engine class handling The Witcher
 */

#ifndef ENGINES_WITCHER_WITCHER_H
#define ENGINES_WITCHER_WITCHER_H

#include "src/common/ustring.h"
#include "src/common/changeid.h"

#include "src/aurora/types.h"
#include "src/aurora/language.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace Witcher {

class Campaign;
class Module;

class WitcherEngineProbe : public Engines::EngineProbe {
public:
	WitcherEngineProbe();
	~WitcherEngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const WitcherEngineProbe kWitcherEngineProbe;

class WitcherEngine : public Engines::Engine {
public:
	WitcherEngine();
	~WitcherEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languagesText,
	                     std::vector<Aurora::Language> &languagesVoice) const;

	bool getLanguage(Aurora::Language &languageText, Aurora::Language &languageVoice) const;
	bool changeLanguage();

	/** Return the currently running campaign. */
	Campaign *getCampaign();
	/** Return the currently running module. */
	Module   *getModule();


protected:
	void run();


private:
	Aurora::Language _languageText;
	Aurora::Language _languageVoice;

	Campaign *_campaign;

	std::list<Common::ChangeID> _languageResources;
	Common::ChangeID _languageTLK;


	void init();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language langText, Aurora::Language langVoice);
	void loadLanguageFiles(Aurora::Language langText, Aurora::Language langVoice);

	void deinit();

	void playIntroVideos();
	void main();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_WITCHER_H

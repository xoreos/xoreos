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
#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/fonthandle.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace Sonic {

class Module;

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

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	Module *getModule();


protected:
	void run();


private:
	Aurora::Language _language;

	Common::ChangeID _languageHERF;
	Common::ChangeID _languageTLK;

	Graphics::Aurora::FontHandle _guiFont;
	Graphics::Aurora::FontHandle _quoteFont;

	Module *_module;


	void init();
	void declareLanguages();
	void declareResources();
	void initResources(LoadProgress &progress);
	void initConfig();
	void initGameConfig();

	void unloadLanguageFiles();
	void loadLanguageFiles(LoadProgress &progress, Aurora::Language language);
	void loadLanguageFiles(Aurora::Language language);

	void deinit();

	void main();

	bool waitClick();

	bool showLicenseSplash();
	bool showTitle();
	bool showMainMenu();
	bool showQuote();
	bool showChapter1();


	static Common::UString getLanguageHERF(Aurora::Language language);
	static Common::UString getLanguageTLK (Aurora::Language language);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_SONIC_H

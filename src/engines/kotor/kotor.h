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
 *  Engine class handling Star Wars: Knights of the Old Republic
 */

#ifndef ENGINES_KOTOR_KOTOR_H
#define ENGINES_KOTOR_KOTOR_H

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/sound/types.h"

#include "src/engines/engine.h"
#include "src/engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

class LoadProgress;

namespace KotOR {

class Module;

class KotOREngineProbe : public Engines::EngineProbe {
public:
	KotOREngineProbe();
	~KotOREngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(Common::SeekableReadStream &stream) const;
	virtual bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const = 0;

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

class KotOREngineProbeWin : public KotOREngineProbe {
public:
	KotOREngineProbeWin();
	~KotOREngineProbeWin();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }
};

class KotOREngineProbeMac : public KotOREngineProbe {
public:
	KotOREngineProbeMac();
	~KotOREngineProbeMac();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }
};

class KotOREngineProbeXbox : public KotOREngineProbe {
public:
	KotOREngineProbeXbox();
	~KotOREngineProbeXbox();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformXbox; }
};

extern const KotOREngineProbeWin  kKotOREngineProbeWin;
extern const KotOREngineProbeMac  kKotOREngineProbeMac;
extern const KotOREngineProbeXbox kKotOREngineProbeXbox;

class KotOREngine : public Engines::Engine {
public:
	KotOREngine();
	~KotOREngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	/** Return the currently running module. */
	Module *getModule();


protected:
	void run();


private:
	Aurora::Language _language;

	Module *_module;

	Sound::ChannelHandle _menuMusic;

	bool _hasLiveKey;

	bool hasYavin4Module() const;


	void init();

	void initConfig();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void initCursorsRemap();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();

	void playMenuMusic();
	void stopMenuMusic();

	void mainMenuLoop();
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_KOTOR_H

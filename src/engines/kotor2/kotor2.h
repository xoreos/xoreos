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
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#ifndef ENGINES_KOTOR2_KOTOR2_H
#define ENGINES_KOTOR2_KOTOR2_H

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

namespace KotOR2 {

class Module;

class KotOR2EngineProbe : public Engines::EngineProbe {
public:
	KotOR2EngineProbe();
	~KotOR2EngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const;
	virtual bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const = 0;

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

class KotOR2EngineProbeWin : public KotOR2EngineProbe {
public:
	KotOR2EngineProbeWin();
	~KotOR2EngineProbeWin();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }
};

class KotOR2EngineProbeLinux : public KotOR2EngineProbe {
public:
	KotOR2EngineProbeLinux();
	~KotOR2EngineProbeLinux();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformLinux; }
};

class KotOR2EngineProbeMac : public KotOR2EngineProbe {
public:
	KotOR2EngineProbeMac();
	~KotOR2EngineProbeMac();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }
};

class KotOR2EngineProbeXbox : public KotOR2EngineProbe {
public:
	KotOR2EngineProbeXbox();
	~KotOR2EngineProbeXbox();

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformXbox; }
};

extern const KotOR2EngineProbeWin   kKotOR2EngineProbeWin;
extern const KotOR2EngineProbeLinux kKotOR2EngineProbeLinux;
extern const KotOR2EngineProbeMac   kKotOR2EngineProbeMac;
extern const KotOR2EngineProbeXbox  kKotOR2EngineProbeXbox;

class KotOR2Engine : public Engines::Engine {
public:
	KotOR2Engine();
	~KotOR2Engine();

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

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_KOTOR2_H

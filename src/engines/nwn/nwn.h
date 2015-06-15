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
 *  Engine class handling Neverwinter Nights.
 */

#ifndef ENGINES_NWN_NWN_H
#define ENGINES_NWN_NWN_H

#include <vector>

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

namespace NWN {

class Version;
class ScriptFunctions;

class GUI;
class Module;

class NWNEngineProbe : public Engines::EngineProbe {
private:
	static const Common::UString kGameName;

public:
	NWNEngineProbe() {}
	~NWNEngineProbe() {}

	Aurora::GameID getGameID() const {
		return Aurora::kGameIDNWN;
	}

	const Common::UString &getGameName() const {
		return kGameName;
	}

	bool probe(Common::SeekableReadStream &UNUSED(stream)) const {
		return false;
	}

	Engines::Engine *createEngine() const;
};

class NWNEngineProbeWindows : public NWNEngineProbe {
public:
	NWNEngineProbeWindows() {}
	~NWNEngineProbeWindows() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }
};

class NWNEngineProbeMac : public NWNEngineProbe {
public:
	NWNEngineProbeMac() {}
	~NWNEngineProbeMac() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformMacOSX; }
};

class NWNEngineProbeLinux: public NWNEngineProbe {
public:
	NWNEngineProbeLinux() {}
	~NWNEngineProbeLinux() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformLinux; }
};

class NWNEngineProbeFallback : public NWNEngineProbe {
public:
	NWNEngineProbeFallback() {}
	~NWNEngineProbeFallback() {}

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	Aurora::Platform getPlatform() const { return Aurora::kPlatformUnknown; }
};

extern const NWNEngineProbeWindows  kNWNEngineProbeWin;
extern const NWNEngineProbeMac      kNWNEngineProbeMac;
extern const NWNEngineProbeLinux    kNWNEngineProbeLinux;
extern const NWNEngineProbeFallback kNWNEngineProbeFallback;

class NWNEngine : public Engines::Engine {
public:
	NWNEngine();
	~NWNEngine();

	bool detectLanguages(Aurora::GameID game, const Common::UString &target,
	                     Aurora::Platform platform,
	                     std::vector<Aurora::Language> &languages) const;

	bool getLanguage(Aurora::Language &language) const;
	bool changeLanguage();

	/** Return the currently running module. */
	Module *getModule();

	/** Overwrite all currently playing music. */
	void playMusic(const Common::UString &music = "");
	/** Force all currently playing music stopped. */
	void stopMusic();

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);
	/** Does a given module exist? */
	static bool hasModule(Common::UString &module);
	/** Return a list of local player characters. */
	static void getCharacters(std::vector<Common::UString> &characters, bool local);


protected:
	void run();


private:
	Version *_version;

	Aurora::Language _language;

	bool _hasXP1; // Shadows of Undrentide (SoU)
	bool _hasXP2; // Hordes of the Underdark (HotU)
	bool _hasXP3; // Kingmaker (resources also included in the final 1.69 patch)

	ScriptFunctions *_scriptFuncs;

	Module *_module;

	Sound::ChannelHandle _menuMusic;


	void init();

	void detectVersion();

	void initConfig();
	void declareLanguages();
	void initResources(LoadProgress &progress);
	void declareBogusTextures();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();

	void playMenuMusic(Common::UString music = "");
	void stopMenuMusic();

	void runMainMenu(GUI *mainMenu);

	void mainMenuLoop();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_NWN_H

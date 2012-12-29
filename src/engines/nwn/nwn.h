/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/nwn.h
 *  Engine class handling Neverwinter Nights.
 */

#ifndef ENGINES_NWN_NWN_H
#define ENGINES_NWN_NWN_H

#include <vector>

#include "common/ustring.h"

#include "aurora/types.h"

#include "sound/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Graphics {
	namespace Aurora {
		class FPS;
	}
}

namespace Engines {

namespace NWN {

class ScriptFunctions;

class NWNEngineProbe : public Engines::EngineProbe {
public:
	NWNEngineProbe();
	~NWNEngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const NWNEngineProbe kNWNEngineProbe;

class NWNEngine : public Engines::Engine {
public:
	NWNEngine();
	~NWNEngine();

	void run(const Common::UString &target);

	/** Return a list of all modules. */
	static void getModules(std::vector<Common::UString> &modules);
	/** Does a given module exist? */
	static bool hasModule(Common::UString &module);
	/** Return a list of local player characters. */
	static void getCharacters(std::vector<Common::UString> &characters, bool local);

private:
	Common::UString _baseDirectory;

	bool _hasXP1; // Shadows of Undrentide (SoU)
	bool _hasXP2; // Hordes of the Underdark (HotU)
	bool _hasXP3; // Kingmaker (resources also included in the final 1.69 patch)

	Graphics::Aurora::FPS *_fps;

	Sound::ChannelHandle _menuMusic;

	ScriptFunctions *_scriptFuncs;


	void init();

	void initConfig();
	void initResources();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();

	void playMenuMusic();
	void stopMenuMusic();

	void mainMenuLoop();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_NWN_H

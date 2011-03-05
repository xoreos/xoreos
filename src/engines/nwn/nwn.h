/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/nwn.h
 *  Engine class handling Neverwinter Nights.
 */

#ifndef ENGINES_NWN_NWN_H
#define ENGINES_NWN_NWN_H

#include "common/ustring.h"

#include "aurora/types.h"
#include "aurora/resman.h"

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

class NWNEngineProbe : public Engines::EngineProbe {
public:
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

private:
	Common::UString _baseDirectory;

	bool _hasXP1; // Shadows of Undrentide (SoU)
	bool _hasXP2; // Hordes of the Underdark (HotU)
	bool _hasXP3; // Kingmaker (resources also included in the final 1.69 patch)

	Graphics::Aurora::FPS *_fps;

	Sound::ChannelHandle _menuMusic;

	// Texture pack
	int _currentTexturePack;
	Aurora::ResourceManager::ChangeID _resTP[4];

	void init();

	void initConfig();
	void initResources();
	void initCursors();
	void initGameConfig();

	void deinit();

	void checkConfig();

	void playIntroVideos();

	void loadTexturePack();

	void playMenuMusic();
	void stopMenuMusic();

	void mainMenuLoop();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_NWN_H

/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/kotor2/kotor2.h
 *  Engine class handling Star Wars: Knights of the Old Republic II - The Sith Lords
 */

#ifndef ENGINES_KOTOR2_KOTOR2_H
#define ENGINES_KOTOR2_KOTOR2_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "sound/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

namespace KotOR2 {

class KotOR2EngineProbe : public Engines::EngineProbe {
public:
	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(Common::SeekableReadStream &stream) const { return false; }

	Engines::Engine *createEngine() const;

private:
	static const Common::UString kGameName;
};

class KotOR2EngineProbeWin : public KotOR2EngineProbe {
public:

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }
};

class KotOR2EngineProbeXbox : public KotOR2EngineProbe {
public:

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformXbox; }
};

extern const KotOR2EngineProbeWin  kKotOR2EngineProbeWin;
extern const KotOR2EngineProbeXbox kKotOR2EngineProbeXbox;

class KotOR2Engine : public Engines::Engine {
public:
	KotOR2Engine(Aurora::Platform platform);
	~KotOR2Engine();

	void run(const Common::UString &target);

private:
	Common::UString _baseDirectory;
	Aurora::Platform _platform;

	Graphics::Aurora::FPS *_fps;

	Sound::ChannelHandle _menuMusic;


	void init();

	void initConfig();
	void initResources();
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

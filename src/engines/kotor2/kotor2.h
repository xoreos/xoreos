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
	KotOR2EngineProbe();
	~KotOR2EngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(Common::SeekableReadStream &stream) const { return false; }
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

class KotOR2EngineProbeXbox : public KotOR2EngineProbe {
public:
	KotOR2EngineProbeXbox();
	~KotOR2EngineProbeXbox();

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

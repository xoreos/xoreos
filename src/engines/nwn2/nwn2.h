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

/** @file engines/nwn2/nwn2.h
 *  Engine class handling Neverwinter Nights 2
 */

#ifndef ENGINES_NWN2_NWN2_H
#define ENGINES_NWN2_NWN2_H

#include "common/ustring.h"

#include "aurora/types.h"

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

class LoadProgress;

namespace NWN2 {

class NWN2EngineProbe : public Engines::EngineProbe {
public:
	NWN2EngineProbe();
	~NWN2EngineProbe();

	Aurora::GameID getGameID() const;

	const Common::UString &getGameName() const;

	bool probe(const Common::UString &directory, const Common::FileList &rootFiles) const;
	bool probe(Common::SeekableReadStream &stream) const;

	Engines::Engine *createEngine() const;

	Aurora::Platform getPlatform() const { return Aurora::kPlatformWindows; }

private:
	static const Common::UString kGameName;
};

extern const NWN2EngineProbe kNWN2EngineProbe;

class NWN2Engine : public Engines::Engine {
public:
	NWN2Engine();
	~NWN2Engine();

	void run();

private:
	bool _hasXP1; // Mask of the Betrayer (MotB)
	bool _hasXP2; // Storm of Zehir (SoZ)
	bool _hasXP3; // Mysteries of Westgate (MoW)

	Graphics::Aurora::FPS *_fps;

	void init();
	void declareEncodings();
	void initResources(LoadProgress &progress);
	void initCursors();
	void initConfig();
	void initGameConfig();

	void deinit();

	void playIntroVideos();
	void main();
};

} // End of namespace NWN2

} // End of namespace Engines

#endif // ENGINES_NWN2_NWN2_H

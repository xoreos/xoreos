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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/sonic/sonic.h
 *  Engine class handling Sonic Chronicles: The Dark Brotherhood
 */

#ifndef ENGINES_SONIC_SONIC_H
#define ENGINES_SONIC_SONIC_H

#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/engine.h"
#include "engines/engineprobe.h"

namespace Common {
	class FileList;
}

namespace Engines {

namespace Sonic {

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

	void run(const Common::UString &target);

private:
	Common::UString _romFile;

	void init();

	void playIntroVideos();
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_SONIC_H

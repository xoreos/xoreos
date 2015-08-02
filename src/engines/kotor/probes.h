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
 *  Probing for an installation of Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_PROBES_H
#define ENGINES_KOTOR_PROBES_H

#include "src/common/ustring.h"

#include "src/engines/engineprobe.h"

namespace Engines {

namespace KotOR {

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

extern const Engines::EngineProbe * const kProbes[];

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_PROBES_H

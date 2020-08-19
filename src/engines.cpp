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
 *  Utility functions to handle the engines.
 */

#include "src/common/system.h"

#include "src/engines/engineprobe.h"

#ifdef ENABLE_NWN
#include "src/engines/nwn/probes.h"
#endif
#ifdef ENABLE_NWN2
#include "src/engines/nwn2/probes.h"
#endif
#ifdef ENABLE_KOTOR
#include "src/engines/kotor/probes.h"
#endif
#ifdef ENABLE_KOTOR2
#include "src/engines/kotor2/probes.h"
#endif
#ifdef ENABLE_JADE
#include "src/engines/jade/probes.h"
#endif
#ifdef ENABLE_WITCHER
#include "src/engines/witcher/probes.h"
#endif
#ifdef ENABLE_SONIC
#include "src/engines/sonic/probes.h"
#endif
#ifdef ENABLE_DRAGONAGE
#include "src/engines/dragonage/probes.h"
#endif
#ifdef ENABLE_DRAGONAGE2
#include "src/engines/dragonage2/probes.h"
#endif

#include "src/engines.h"

void createEngineProbes(std::list<const Engines::EngineProbe *> &probes) {
#ifdef ENABLE_NWN
	Engines::NWN::createEngineProbes(probes);
#endif
#ifdef ENABLE_NWN2
	Engines::NWN2::createEngineProbes(probes);
#endif
#ifdef ENABLE_KOTOR
	Engines::KotOR::createEngineProbes(probes);
#endif
#ifdef ENABLE_KOTOR2
	Engines::KotOR2::createEngineProbes(probes);
#endif
#ifdef ENABLE_JADE
	Engines::Jade::createEngineProbes(probes);
#endif
#ifdef ENABLE_WITCHER
	Engines::Witcher::createEngineProbes(probes);
#endif
#ifdef ENABLE_SONIC
	Engines::Sonic::createEngineProbes(probes);
#endif
#ifdef ENABLE_DRAGONAGE
	Engines::DragonAge::createEngineProbes(probes);
#endif
#ifdef ENABLE_DRAGONAGE2
	Engines::DragonAge2::createEngineProbes(probes);
#endif
}

void destroyEngineProbes(std::list<const Engines::EngineProbe *> &probes) {
	for (std::list<const Engines::EngineProbe *>::iterator p = probes.begin(); p != probes.end(); ++p)
		delete *p;

	probes.clear();
}

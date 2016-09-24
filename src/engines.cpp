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

#include "src/engines/engineprobe.h"

#include "src/engines/nwn/probes.h"
#include "src/engines/nwn2/probes.h"
#include "src/engines/kotor/probes.h"
#include "src/engines/kotor2/probes.h"
#include "src/engines/jade/probes.h"
#include "src/engines/witcher/probes.h"
#include "src/engines/sonic/probes.h"
#include "src/engines/dragonage/probes.h"
#include "src/engines/dragonage2/probes.h"

#include "src/engines.h"

void createEngineProbes(std::list<const Engines::EngineProbe *> &probes) {
	Engines::NWN::createEngineProbes(probes);
	Engines::NWN2::createEngineProbes(probes);
	Engines::KotOR::createEngineProbes(probes);
	Engines::KotOR2::createEngineProbes(probes);
	Engines::Jade::createEngineProbes(probes);
	Engines::Witcher::createEngineProbes(probes);
	Engines::Sonic::createEngineProbes(probes);
	Engines::DragonAge::createEngineProbes(probes);
	Engines::DragonAge2::createEngineProbes(probes);
}

void destroyEngineProbes(std::list<const Engines::EngineProbe *> &probes) {
	for (std::list<const Engines::EngineProbe *>::iterator p = probes.begin(); p != probes.end(); ++p)
		delete *p;

	probes.clear();
}

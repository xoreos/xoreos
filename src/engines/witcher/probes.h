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
 *  Probing for an installation of The Witcher.
 */

#ifndef ENGINES_WITCHER_PROBES_H
#define ENGINES_WITCHER_PROBES_H

#include <list>

namespace Engines {

class EngineProbe;

namespace Witcher {

/** Create all probes for this engine and add them to the list. */
void createEngineProbes(std::list<const EngineProbe *> &probes);

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_PROBES_H

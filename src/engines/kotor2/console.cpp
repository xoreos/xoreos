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
 *  Star Wars: Knights of the Old Republic II - The Sith Lords (debug) console.
 */

#include "src/engines/kotor2/console.h"
#include "src/engines/kotor2/kotor2.h"

namespace Engines {

namespace KotOR2 {

Console::Console(KotOR2Engine &engine) : KotORBase::Console(engine) {
}

const Common::UString &Console::getModuleDirOptionName() const {
	static Common::UString name("KOTOR2_moduleDir");
	return name;
}

} // End of namespace KotOR2

} // End of namespace Engines

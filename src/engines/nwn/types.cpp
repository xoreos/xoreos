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

/** @file engines/nwn/types.cpp
 *  Basic NWN type definitions.
 */

#include "engines/nwn/types.h"

namespace Engines {

namespace NWN {

Alignment getAlignmentLawChaos(uint8 lawChaos) {
	if (lawChaos >= 70)
		return kAlignmentLawful;
	if (lawChaos <= 30)
		return kAlignmentChaotic;

	return kAlignmentNeutral;
}

Alignment getAlignmentGoodEvil(uint8 goodEvil) {
	if (goodEvil >= 70)
		return kAlignmentGood;
	if (goodEvil <= 30)
		return kAlignmentEvil;

	return kAlignmentNeutral;
}

} // End of namespace NWN

} // End of namespace Engines

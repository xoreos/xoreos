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

/** @file engines/kotor/types.h
 *  Basic KotOR type definitions.
 */

#ifndef ENGINES_KOTOR_TYPES_H
#define ENGINES_KOTOR_TYPES_H

#include "common/types.h"

namespace Engines {

namespace KotOR {

enum ObjectType {
	kObjectTypeNone         = 0     ,
	kObjectTypeCreature     = 1 << 0,
	kObjectTypeItem         = 1 << 1,
	kObjectTypeTrigger      = 1 << 2,
	kObjectTypeDoor         = 1 << 3,
	kObjectTypeAreaOfEffect = 1 << 4,
	kObjectTypeWaypoint     = 1 << 5,
	kObjectTypePlaceable    = 1 << 6,
	kObjectTypeStore        = 1 << 7,
	kObjectTypeEncounter    = 1 << 8,
	kObjectTypeAll          = 0x7FFF
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_TYPES_H

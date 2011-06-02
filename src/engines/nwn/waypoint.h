/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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

/** @file engines/nwn/waypoint.h
 *  NWN waypoint.
 */

#ifndef ENGINES_NWN_WAYPOINT_H
#define ENGINES_NWN_WAYPOINT_H

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "engines/nwn/types.h"
#include "engines/nwn/object.h"

namespace Engines {

namespace NWN {

class Waypoint : public Object {
public:
	Waypoint();
	~Waypoint();

	void load(const Aurora::GFFStruct &waypoint);

	/** Does this waypoint have a map note? */
	bool hasMapNote() const;
	/** Return the waypoint's map note text. */
	const Common::UString getMapNote() const;

	/** Enable/Disable the waypoint's map note. */
	void enableMapNote(bool enabled);

private:
	bool _hasMapNote;         ///< Does this waypoint have a map note?
	Common::UString _mapNote; ///< The waypoint's map note text.

	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	void loadProperties(const Aurora::GFFStruct &gff);

	void clear();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_WAYPOINT_H

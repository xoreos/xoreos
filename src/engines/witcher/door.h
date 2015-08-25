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
 *  A door within a Witcher area.
 */

#ifndef ENGINES_WITCHER_DOOR_H
#define ENGINES_WITCHER_DOOR_H

#include "src/aurora/types.h"

#include "src/engines/witcher/situated.h"

namespace Engines {

namespace Witcher {

class Module;

class Waypoint;

class Door : public Situated {
public:
	/** Load from a door instance. */
	Door(Module &module, const Aurora::GFF3Struct &door);
	~Door();

	// Basic visuals

	void hide(); ///< Hide the door's model.

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the door. */
	void leave(); ///< The cursor left the door. */

	/** (Un)Highlight the door. */
	void highlight(bool enabled);

	/** The door was clicked. */
	bool click(Object *triggerer = 0);

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);

private:
	Module *_module; ///< The module the door is in.

	Common::UString _linkTag; ///< The waypoint tag this door links to.
	Waypoint       *_link;    ///< The waypoint this door links to.

	/** Load from a door instance. */
	void load(const Aurora::GFF3Struct &door);

	/** Evaluate our link. */
	void evaluateLink();
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_DOOR_H

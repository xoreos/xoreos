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
 *  A door in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#ifndef ENGINES_KOTOR2_DOOR_H
#define ENGINES_KOTOR2_DOOR_H

#include "src/aurora/types.h"

#include "src/engines/kotor2/situated.h"

namespace Engines {

namespace KotOR2 {

class Door : public Situated {
public:
	/** The state of a door. */
	enum State {
		kStateClosed    = 0, ///< Closed.
		kStateOpened1   = 1, ///< Opened in one direction (normally clock-wise).
		kStateOpened2   = 2, ///< Opened in the other direction (normally counter-clock-wise).
		kStateDestroyed = 3  ///< Destroyed.
	};

	/** Load from a door instance. */
	Door(const Aurora::GFF3Struct &door);
	~Door();

	// Basic visuals

	void hide(); ///< Hide the door's model.

	// Basic properties

	/** Is the door open? */
	bool isOpen() const;

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the door. */
	void leave(); ///< The cursor left the door. */

	/** (Un)Highlight the door. */
	virtual void highlight(bool enabled);

protected:
	/** Load door-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	uint32 _genericType; ///< Index into the generic door types.

	State _state; ///< The current state of the door.

	/** Load from a door instance. */
	void load(const Aurora::GFF3Struct &door);

	/** Load the appearance from this 2DA row. */
	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id);
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_DOOR_H

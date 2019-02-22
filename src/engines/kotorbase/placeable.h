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
 *  Placeable within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_PLACEABLE_H
#define ENGINES_KOTORBASE_PLACEABLE_H

#include "src/engines/kotorbase/item.h"
#include "src/engines/kotorbase/inventory.h"
#include "src/engines/kotorbase/situated.h"

namespace Engines {

namespace KotORBase {

class Placeable : public Situated {
public:
	/** The state of a placeable. */
	enum State {
		kStateDefault     = 0, ///< Default.
		kStateOpen        = 1, ///< Open.
		kStateClosed      = 2, ///< Closed.
		kStateDestroyed   = 3, ///< Destroyed.
		kStateActivated   = 4, ///< Activated.
		kStateDeactivated = 5  ///< Deactivated.
	};

	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF3Struct &placeable);

	// Basic visuals

	/** Hide the placeable's model. */
	void hide();

	// Basic properties

	/** Is the placeable open? */
	bool isOpen() const;
	/** Is the placeable activated? */
	bool isActivated() const;

	/** The opener object opens this placeable. */
	bool open(Object *opener);
	/** The closer object closes this placeable. */
	bool close(Object *closer);
	/** The user object activates this placeable. */
	bool activate(Object *opener);
	/** The user object deactivates this placeable. */
	bool deactivate(Object *closer);

	// Inventory

	bool hasInventory();
	Inventory &getInventory();

	// Object/cursor interactions

	/** The cursor entered the placeable. */
	void enter();
	/** The cursor left the placeable. */
	void leave();
	/** (Un)Highlight the placeable. */
	void highlight(bool enabled);
	/** The placeable was clicked. */
	bool click(Object *triggerer = 0);

protected:
	/** Load placeable-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	State _state; ///< The current state of the placeable.

	bool _hasInventory; ///< Does this placeable have an inventory?
	Inventory _inventory; ///< The current items of this placeable if it has an inventory.

	/** Load from a placeable instance. */
	void load(const Aurora::GFF3Struct &placeable);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_PLACEABLE_H

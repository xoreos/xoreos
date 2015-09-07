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
 *  A trigger in a Jade Empire area.
 */

#ifndef ENGINES_JADE_TRIGGER_H
#define ENGINES_JADE_TRIGGER_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/vertexbuffer.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/object.h"

namespace Engines {

namespace Jade {

class Trigger : public Object {
public:
	/** Load from a trigger instance. */
	Trigger(const Aurora::GFF3Struct &trigger);
	~Trigger();

	// Basic properties

	/** Does this trigger only fire once? */
	bool isOneShot() const;
	/** Does this trigger represent a trap? */
	bool isTrap() const;
	/** Is this trigger detectable? */
	bool isDetectable() const;
	/** Is this trigger disarmable? */
	bool isDisarmable() const;
	/** Is this trigger flagged? */
	bool isFlagged() const;
	/** Does this trigger represent an area transition? */
	bool isAreaTrans() const;
	/** Does this trigger have henchmen data? */
	bool isHenchmenData() const;
	/** Return the trigger's transition text. */
	Common::UString getTransitionText() const;

private:
	bool _isOneShot;      ///< Does this trigger only fire once?
	bool _isTrap;         ///< Does this trigger represent a trap?
	bool _isDetectable;   ///< Is this trigger detectable?
	bool _isDisarmable;   ///< Is this trigger disarmable?
	bool _isFlagged;      ///< Is this trigger flagged?
	bool _isAreaTrans;    ///< Does this trigger represent an area transition?
	bool _isHenchmenData; ///< Does this trigger have henchmen data?
	bool _isCombatArea;   ///< Does this trigger lead to combat?
	bool _isCombatActive; ///< Does this trigger have henchmen data?

	int _loadScreen;         ///< The loading screen used.

	Common::UString _transitionText; ///< The trigger's transition text.

	Graphics::VertexBuffer _vertexBuffer; ///< Geometry outline vertex buffer.

	/** Load from a trigger instance. */
	void load(const Aurora::GFF3Struct &trigger);
	/** Load the trigger blueprint properties. */
	void loadBlueprint(const Aurora::GFF3Struct &gff);
	/** Load the trigger instance properties. */
	void loadInstance(const Aurora::GFF3Struct &gff);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_TRIGGER_H

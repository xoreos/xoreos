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
 *  A placeable in a Jade Empire area.
 */

#ifndef ENGINES_JADE_PLACEABLE_H
#define ENGINES_JADE_PLACEABLE_H

#include <memory>

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/jade/object.h"

namespace Engines {

namespace Jade {

class Placeable : public Object {
public:
	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF3Struct &placeable);
	~Placeable();

	// Basic visuals

	void show(); ///< Show the placeable's model.
	void hide(); ///< Hide the placeable's model.

	// Basic properties

	/** The opener object opens this placeable. */
	bool open(Object *opener);
	/** The closer object closes this placeable. */
	bool close(Object *closer);

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the placeable.
	void leave(); ///< The cursor left the placeable.

	/** (Un)Highlight the placeable. */
	virtual void highlight(bool enabled);

protected:
	Common::UString _modelName; ///< The model's resource name.
	Common::UString _soundCue;  ///< The placeable's sound cue.
	Common::UString _resRef;    ///< The placeable's description resref.

	uint32_t _appearanceType;     ///< The index within the placeable 2DA.

	Object *_lastOpenedBy; ///< The object that last opened this placeable object.
	Object *_lastClosedBy; ///< The object that last closed this placeable object.

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The placeable's model.

	int32_t _state; ///< The placeable's current state.
	std::unique_ptr<Aurora::GFF3File> _fsm; ///< The placeable's state file.

private:
	/** Load from a placeable instance. */
	void load(const Aurora::GFF3Struct &placeable);
	/** Load the placeable's blueprint properties. */
	void loadBlueprint(const Aurora::GFF3Struct &gff);
	/** Load the placeable's instance properties. */
	void loadInstance(const Aurora::GFF3Struct &gff);
	/** Load properties from placeable.2da. */
	void loadProperties();
	/** Load appearance-specific properties. */
	void loadAppearance();
	/** Determines the result State according to the state model */
	int32_t nextState(const Common::UString &input);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_PLACEABLE_H

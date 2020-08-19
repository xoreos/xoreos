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
 *  A placeable object in a Sonic Chronicles: The Dark Brotherhood area.
 */

#ifndef ENGINES_SONIC_PLACEABLE_H
#define ENGINES_SONIC_PLACEABLE_H

#include <memory>

#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/sonic/object.h"

namespace Engines {

namespace Sonic {

class Placeable : public Object {
public:
	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF4Struct &placeable);
	~Placeable();

	// Basic visuals

	void show(); ///< Show the placeable's model.
	void hide(); ///< Hide the placeable's model.

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the placeable.
	void leave(); ///< The cursor left the placeable.

	/** (Un)Highlight the placeable. */
	void highlight(bool enabled);

	// Positioning

	/** Set the placeable's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the placeable's orientation. */
	virtual void setOrientation(float x, float y, float z, float angle);

protected:
	/** The resource change created by indexing the model's texture. */
	Common::ChangeID _modelTexture;
	/** The placeable's model. */
	std::unique_ptr<Graphics::Aurora::Model> _model;

	uint32_t _placeableID;  ///< The placeable's identifer from GFF.
	uint32_t _typeID;       ///< The placeable's type.
	uint32_t _appearanceID; ///< The placeable's appearance.

	/** The name of the model representing this placeable. */
	Common::UString _modelName;

	/** The scale modifying this placeable's model. */
	float _scale;


	/** Load from a placeable instance. */
	void load(const Aurora::GFF4Struct &placeable);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_PLACEABLE_H

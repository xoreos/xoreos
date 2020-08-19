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
 *  A placeable object in a Dragon Age II area.
 */

#ifndef ENGINES_DRAGONAGE2_PLACEABLE_H
#define ENGINES_DRAGONAGE2_PLACEABLE_H

#include <memory>

#include "src/common/ustring.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/dragonage2/object.h"

namespace Engines {

namespace DragonAge2 {

class Placeable : public Object {
public:
	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF3Struct &placeable);
	~Placeable();

	// Basic visuals

	void show(); ///< Show the placeable's model.
	void hide(); ///< Hide the placeable's model.

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the placeable.
	void leave(); ///< The cursor left the placeable.

	/** (Un)Highlight the placeable. */
	void highlight(bool enabled);

	/** The placeable was clicked. */
	bool click(Object *triggerer = 0);

	// Positioning

	/** Set the placeable's position. */
	void setPosition(float x, float y, float z);
	/** Set the placeable's orientation. */
	void setOrientation(float x, float y, float z, float angle);


private:
	/** The placeable's appearance; index into the Placeables MGDA. */
	uint32_t _appearanceID;

	std::unique_ptr<Graphics::Aurora::Model> _model; ///< The placeable's model.


	void load(const Aurora::GFF3Struct &placeable);
	void load(const Aurora::GFF3Struct &instance, const Aurora::GFF3Struct *blueprint);

	void loadProperties(const Aurora::GFF3Struct &gff);
};

} // End of namespace Dragon Age

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE2_PLACEABLE_H

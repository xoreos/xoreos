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
 *  A placeable in a Star Wars: Knights of the Old Republic area.
 */

#ifndef ENGINES_KOTOR_PLACEABLE_H
#define ENGINES_KOTOR_PLACEABLE_H

#include "src/aurora/types.h"

#include "src/engines/kotor/situated.h"

namespace Engines {

namespace KotOR {

class Placeable : public Situated {
public:
	/** Load from a placeable instance. */
	Placeable(const Aurora::GFF3Struct &placeable);
	~Placeable();

	// Basic visuals

	void hide(); ///< Hide the placeable's model.

	// Object/Cursor interactions

	void enter(); ///< The cursor entered the placeable. */
	void leave(); ///< The cursor left the placeable. */

	/** (Un)Highlight the placeable. */
	virtual void highlight(bool enabled);

protected:
	/** Load placeable-specific properties. */
	void loadObject(const Aurora::GFF3Struct &gff);
	/** Load appearance-specific properties. */
	void loadAppearance();

private:
	/** Load from a placeable instance. */
	void load(const Aurora::GFF3Struct &placeable);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_PLACEABLE_H

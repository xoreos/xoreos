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
 *  KotOR2 door.
 */

#ifndef ENGINES_KOTOR2_DOOR_H
#define ENGINES_KOTOR2_DOOR_H

#include "src/aurora/types.h"

#include "src/engines/kotor2/situated.h"

namespace Engines {

namespace KotOR2 {

class Door : public Situated {
public:
	Door(const Aurora::GFF3Struct &door);
	~Door();

	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);

protected:
	void loadObject(const Aurora::GFF3Struct &gff);
	void loadAppearance();

private:
	uint32 _genericType;

	void load(const Aurora::GFF3Struct &door);

	void loadAppearance(const Aurora::TwoDAFile &twoda, uint32 id);
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_DOOR_H

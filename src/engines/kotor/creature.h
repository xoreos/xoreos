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
 *  A creature.
 */

#ifndef ENGINES_KOTOR_CREATURE_H
#define ENGINES_KOTOR_CREATURE_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "graphics/aurora/types.h"

#include "engines/kotor/object.h"

namespace Engines {

namespace KotOR {

/** A KotOR creature. */
class Creature : public Object {
public:
	Creature(const Aurora::GFFStruct &creature);
	~Creature();

	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z);

	void enter();
	void leave();

	void highlight(bool enabled);

private:
	struct PartModels {
		Common::UString type;

		Common::UString body;
		Common::UString head;

		Common::UString bodyTexture;
	};

	uint32 _appearance;

	Graphics::Aurora::Model *_model;

	void load(const Aurora::GFFStruct &creature);
	void load(const Aurora::GFFStruct &instance, const Aurora::GFFStruct *blueprint);

	void loadProperties(const Aurora::GFFStruct &gff);
	void loadPortrait(const Aurora::GFFStruct &gff);
	void loadAppearance();

	void getPartModels(PartModels &parts, uint32 state = 'b');
	void loadBody(PartModels &parts);
	void loadHead(PartModels &parts);
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_CREATURE_H

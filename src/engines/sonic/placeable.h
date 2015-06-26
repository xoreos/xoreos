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
 *  A placeable object within a Sonic area.
 */

#ifndef ENGINES_SONIC_PLACEABLE_H
#define ENGINES_SONIC_PLACEABLE_H

#include "src/common/changeid.h"

#include "src/aurora/types.h"

#include "src/graphics/aurora/types.h"

#include "src/engines/sonic/object.h"

namespace Engines {

namespace Sonic {

class Placeable : public Object {
public:
	Placeable(const Aurora::GFF4Struct &placeable);
	~Placeable();

	void show();
	void hide();

	void enter();
	void leave();

	void highlight(bool enabled);

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z, float angle);

protected:
	Common::ChangeID _modelTexture;
	Graphics::Aurora::Model *_model;

	uint32 _typeID;
	uint32 _appearanceID;

	Common::UString _modelName;

	float _scale;


	void load(const Aurora::GFF4Struct &placeable);
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_PLACEABLE_H

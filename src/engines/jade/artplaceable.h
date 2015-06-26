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
 *  A simple model object within a Jade Empire area, guided by a state machine.
 */

#ifndef ENGINES_JADE_ARTPLACEABLE_H
#define ENGINES_JADE_ARTPLACEABLE_H

#include "src/graphics/aurora/types.h"

#include "src/engines/jade/object.h"

namespace Engines {

namespace Jade {

/** An object within a Jade area. */
class ArtPlaceable : public Object {
public:
	ArtPlaceable(const Common::UString &resRef, uint32 id, float x, float y, float z);
	~ArtPlaceable();


	void show();
	void hide();

	void setPosition(float x, float y, float z);
	void setOrientation(float x, float y, float z, float angle);

	void enter();
	void leave();

	void highlight(bool enabled);

protected:
	Graphics::Aurora::Model *_model;

	void load(const Common::UString &resRef, uint32 id, float x, float y, float z);
	void unload();
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_ARTPLACEABLE_H

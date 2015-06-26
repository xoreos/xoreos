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
 *  An object within a Sonic area.
 */

#ifndef ENGINES_SONIC_OBJECT_H
#define ENGINES_SONIC_OBJECT_H

#include "src/common/types.h"
#include "src/common/ustring.h"

namespace Engines {

namespace Sonic {

/** An object within a Sonic area. */
class Object {
public:
	Object();
	virtual ~Object();

	virtual void show() = 0;
	virtual void hide() = 0;

	const Common::UString &getTag() const;

	uint32 getID() const;
	uint32 getModelID() const;

	virtual void getPosition(float &x, float &y, float &z) const;
	virtual void getOrientation(float &x, float &y, float &z, float &angle) const;

	virtual void setPosition(float x, float y, float z);
	virtual void setOrientation(float x, float y, float z, float angle);

	virtual void enter() = 0;
	virtual void leave() = 0;

	virtual void highlight(bool enabled) = 0;

protected:
	Common::UString _tag;

	uint32 _id;
	uint32 _modelID;

	float _position[3];
	float _orientation[4];
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_OBJECT_H

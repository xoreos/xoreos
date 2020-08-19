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
 *  An object in a Sonic Chronicles: The Dark Brotherhood area.
 */

#ifndef ENGINES_SONIC_OBJECT_H
#define ENGINES_SONIC_OBJECT_H

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/object.h"

#include "src/engines/sonic/types.h"

namespace Engines {

namespace Sonic {

/** An object within a Sonic area. */
class Object : public Aurora::NWScript::Object {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	virtual void show(); ///< Show the object's model.
	virtual void hide(); ///< Hide the object's model.

	// Basic properties

	/** Return the ID of the object's model. */
	uint32_t getModelID() const;

	// Positioning

	/** Return the object's position within its area. */
	virtual void getPosition(float &x, float &y, float &z) const;
	/** Return the object's orientation. */
	virtual void getOrientation(float &x, float &y, float &z, float &angle) const;

	/** Set the object's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the object's orientation. */
	virtual void setOrientation(float x, float y, float z, float angle);

	// Object/Cursor interactions

	virtual void enter(); ///< The cursor entered the object.
	virtual void leave(); ///< The cursor left the object.

	/** (Un)Highlight the object. */
	virtual void highlight(bool enabled);

protected:
	ObjectType _type; ///< The object's type.

	uint32_t _modelID; ///< The ID of the object's model.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.
};

} // End of namespace Sonic

} // End of namespace Engines

#endif // ENGINES_SONIC_OBJECT_H

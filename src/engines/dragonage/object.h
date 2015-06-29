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
 *  An object in a Dragon Age: Origins area.
 */

#ifndef ENGINES_DRAGONAGE_OBJECT_H
#define ENGINES_DRAGONAGE_OBJECT_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

#include "src/engines/dragonage/scriptobject.h"

namespace Engines {

namespace DragonAge {

/** An object in a Dragon Age: Origins area. */
class Object : public ScriptObject {
public:
	Object(ObjectType type);
	virtual ~Object();

	// Basic visuals

	virtual void show(); ///< Show the object's model(s).
	virtual void hide(); ///< Hide the object's model(s).

	/** Return the object's model IDs. */
	const std::list<uint32> &getIDs() const;

	// Basic properties

	/** Return the object's name. */
	const Aurora::LocString &getName() const;
	/** Return the object's description. */
	const Aurora::LocString &getDescription() const;

	/** Return the object's default conversation (DLG). */
	const Common::UString &getConversation() const;

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulatable at all)?
	bool isUsable() const; ///< Can the object be used by the PC?

	bool isClickable() const; ///< Can the player click the object?

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

	virtual void enter(); ///< The cursor entered the object. */
	virtual void leave(); ///< The cursor left the object. */

	/** (Un)Highlight the object. */
	virtual void highlight(bool enabled);


protected:
	Aurora::LocString _name;        ///< The object's display name.
	Aurora::LocString _description; ///< The object's description.

	Common::UString _conversation; ///< The object's default conversation.

	Common::UString _soundSet; ///< The object's sound set.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	std::list<uint32> _ids; ///< The object's model IDs.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.


	void syncPosition();
	void syncOrientation();
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_OBJECT_H

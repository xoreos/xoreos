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
 *  An object in a Star Wars: Knights of the Old Republic II - The Sith Lords area.
 */

#ifndef ENGINES_KOTOR2_OBJECT_H
#define ENGINES_KOTOR2_OBJECT_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/object.h"

#include "src/sound/types.h"

#include "src/engines/kotor2/types.h"

#include "src/engines/kotor2/script/container.h"

namespace Engines {

namespace KotOR2 {

class Room;

class Object : public Aurora::NWScript::Object, public KotOR2::ScriptContainer {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	virtual void show(); ///< Show the object's model(s).
	virtual void hide(); ///< Hide the object's model(s).
	virtual void hideSoft(); ///< Hide the object's model(s) if applicable.
	virtual bool isVisible() const; ///< Is the object's model(s) visible?

	/** Return the object's model IDs. */
	const std::list<uint32> &getIDs() const;

	// Basic properties

	/** Return the object's name. */
	const Common::UString &getName() const;
	/** Return the object's description. */
	const Common::UString &getDescription() const;
	/** Return the object's portrait. */
	const Common::UString &getPortrait() const;

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulable at all)?
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

	void makeLookAt(float x, float y);
	void makeLookAt(Object *target);

	/** Get a room the object is in. */
	Room *getRoom();
	/** Set a room the object is in. */
	void setRoom(Room *room);

	// Object/Cursor interactions

	virtual void enter(); ///< The cursor entered the object.
	virtual void leave(); ///< The cursor left the object.

	/** (Un)Highlight the object. */
	virtual void highlight(bool enabled);

	/** The object was clicked. */
	virtual bool click(Object *triggerer = 0);

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);

	// Animation

	virtual void playAnimation(const Common::UString &anim,
	                           bool restart = true,
	                           float length = 0.0f,
	                           float speed = 1.0f);

protected:
	ObjectType _type; ///< The object's type.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _portrait; ///< The object's portrait.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	std::list<uint32> _ids; ///< The object's model IDs.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.

	Room *_room; ///< Room the object is in.
};

} // End of namespace KotOR2

} // End of namespace Engines

#endif // ENGINES_KOTOR2_OBJECT_H

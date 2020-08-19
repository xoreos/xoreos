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
 *  An object in a The Witcher area.
 */

#ifndef ENGINES_WITCHER_OBJECT_H
#define ENGINES_WITCHER_OBJECT_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/locstring.h"

#include "src/aurora/nwscript/object.h"

#include "src/sound/types.h"

#include "src/engines/witcher/types.h"
#include "src/engines/witcher/location.h"

#include "src/engines/witcher/nwscript/container.h"

namespace Engines {

namespace Witcher {

class Area;

/** An object within a Witcher area. */
class Object : public Aurora::NWScript::Object, public Witcher::ScriptContainer {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	/** Refresh all localized strings. */
	virtual void refreshLocalized();

	// Basic visuals

	virtual void loadModel();   ///< Load the object's model(s).
	virtual void unloadModel(); ///< Unload the object's model(s).

	virtual void show(); ///< Show the object's model(s).
	virtual void hide(); ///< Hide the object's model(s).

	/** Return the object's model IDs. */
	const std::list<uint32_t> &getIDs() const;

	// Basic properties

	/** Return the object's globally unique ID. */
	const Common::UString &getUniqueID() const;

	/** Return the object's name. */
	const Aurora::LocString &getName() const;
	/** Return the object's description. */
	const Aurora::LocString &getDescription() const;

	/** Return the object's default conversation (DLG). */
	const Common::UString &getConversation() const;

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulable at all)?
	bool isUsable() const; ///< Can the object be used by the PC?

	bool isClickable() const; ///< Can the player click the object?

	// Object/Object interactions

	/** Return the area this object is currently in. */
	Area *getArea() const;
	/** Set the area this object is currently in. */
	void setArea(Area *);

	// Positioning

	/** Return the object's position within its area. */
	virtual void getPosition(float &x, float &y, float &z) const;
	/** Return the object's orientation. */
	virtual void getOrientation(float &x, float &y, float &z, float &angle) const;

	/** Set the object's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the object's orientation. */
	virtual void setOrientation(float x, float y, float z, float angle);

	/** Create a Location out of the object's area, position and orientation. */
	Location getLocation() const;

	// Object/Cursor interactions

	virtual void enter(); ///< The cursor entered the object.
	virtual void leave(); ///< The cursor left the object.

	/** (Un)Highlight the object. */
	virtual void highlight(bool enabled);

	/** The object was clicked. */
	virtual bool click(Object *triggerer = 0);

	// Object (text) talking

	/** Speak the specified string. */
	void speakString(const Common::UString &string, uint32_t volume);
	/** Speak an one-liner from the specified conversation file. */
	void speakOneLiner(Common::UString conv, Object *tokenTarget = 0);

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);


protected:
	ObjectType _type; ///< The object's type.

	Common::UString _uniqueID; ///< A globally unique ID.

	Aurora::LocString _name;        ///< The object's name.
	Aurora::LocString _description; ///< The object's description.

	Common::UString _conversation; ///< The object's default conversation.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	std::list<uint32_t> _ids; ///< The object's model IDs.

	Area *_area; ///< The area the object is currently in.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.
};

} // End of namespace Witcher

} // End of namespace Engines

#endif // ENGINES_WITCHER_OBJECT_H

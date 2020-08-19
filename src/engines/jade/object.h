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
 *  An object within a Jade Empire area.
 */

#ifndef ENGINES_JADE_OBJECT_H
#define ENGINES_JADE_OBJECT_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/object.h"

#include "src/engines/jade/types.h"
#include "src/engines/jade/location.h"

#include "src/engines/jade/script/container.h"

namespace Aurora {

class GFF3Struct;

}

namespace Engines {

namespace Jade {

class Area;

/** An object within a Jade area. */
class Object : public Aurora::NWScript::Object, public Jade::ScriptContainer {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	virtual void loadModel();   ///< Load the object's model(s).
	virtual void unloadModel(); ///< Unload the object's model(s).

	virtual void show(); ///< Show the object's model(s).
	virtual void hide(); ///< Hide the object's model(s).

	/** Return the object's model IDs. */
	const std::list<uint32_t> &getIDs() const;

	// Basic properties

	/** Return the object's name. */
	const Common::UString &getName() const;
	/** Return the object's description. */
	const Common::UString &getDescription() const;

	/** Return the object's default conversation (DLG). */
	const Common::UString &getConversation() const;

	// Object/Object interactions

	/** Return the PC currently speaking with this object. */
	Aurora::NWScript::Object *getPCSpeaker() const;
	/** Set the PC currently speaking with this object. */
	void setPCSpeaker(Aurora::NWScript::Object *pc);

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulable at all)?
	bool isUsable() const; ///< Can the object be used by the PC?
	bool isActive() const; ///< Is the object currently active/available/visible?

	bool isNoCollide() const;          ///< Is collision checking disabled?
	void setNoCollide(bool noCollide); ///< Enable or disable collision checking

	bool isClickable() const; ///< Can the player click the object?

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

	/** Return the object that last triggered this object. */
	Object *getLastTriggerer() const;

	// Object (text) talking

	/** Speak the specified string. */
	void speakString(int32_t resref);

	// Animation

	/** Play an object animation. */
	virtual void playAnimation(const Common::UString &animation = "", bool restart = true, int32_t loopCount = 0);
	/** Play a default object animation. */
	virtual void playAnimation(Animation animation);

protected:
	ObjectType _type; ///< The object's type.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _conversation; ///< The object's default conversation.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?
	bool _active; ///< Is the object currently active/available/visible?

	bool _noCollide; ///< Is collision checking for the object disabled?

	std::list<uint32_t> _ids; ///< The object's model IDs.

	Aurora::NWScript::Object *_pcSpeaker; ///< The current PC speaking with the object.

	Area *_area; ///< The area the object is currently in.

	Object *_lastTriggerer; ///< The object that last used this object.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	/** Load the object's positional gff struct which contains the position and orientation. */
	void loadPositional(const Aurora::GFF3Struct &gff);
};

} // End of namespace Jade

} // End of namespace Engines

#endif // ENGINES_JADE_OBJECT_H

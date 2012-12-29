/* xoreos - A reimplementation of BioWare's Aurora engine
 *
 * xoreos is the legal property of its developers, whose names can be
 * found in the AUTHORS file distributed with this source
 * distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 3
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 *
 * The Infinity, Aurora, Odyssey, Eclipse and Lycium engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 */

/** @file engines/nwn/object.h
 *  An object within a NWN area.
 */

#ifndef ENGINES_NWN_OBJECT_H
#define ENGINES_NWN_OBJECT_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/nwscript/object.h"

#include "sound/types.h"

#include "engines/nwn/location.h"

#include "engines/nwn/script/container.h"

namespace Aurora {
	class SSFFile;

	namespace NWScript {
		class Object;
	}
}

namespace Engines {

namespace NWN {

class Area;

/** An object within a NWN area. */
class Object : public Aurora::NWScript::Object, public ScriptContainer {
public:
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	virtual void loadModel();   ///< Load the object's model(s).
	virtual void unloadModel(); ///< Unload the object's model(s).

	virtual void show(); ///< Show the object's model(s).
	virtual void hide(); ///< Hide the object's model(s).

	/** Return the object's model IDs. */
	const std::list<uint32> &getIDs() const;

	// Basic properties

	/** Return the object's name. */
	const Common::UString &getName() const;
	/** Return the object's description. */
	const Common::UString &getDescription() const;
	/** Return the object's portrait. */
	const Common::UString &getPortrait() const;

	/** Return the object's default conversation (DLG). */
	const Common::UString &getConversation() const;

	/** Return the object's sound set. */
	const Aurora::SSFFile *getSSF();

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulatable at all)?
	bool isUsable() const; ///< Can the object be used by the PC?

	bool isClickable() const; ///< Can the player click the object?

	// Object/Object interactions

	/** Return the PC currently speaking with this object. */
	Aurora::NWScript::Object *getPCSpeaker() const;
	/** Set the PC currently speaking with this object. */
	void setPCSpeaker(Aurora::NWScript::Object *pc);

	/** Return the area this object is currently in. */
	Area *getArea() const;
	/** Set the area this object is currently in. */
	void setArea(Area *);

	// Positioning

	/** Return the object's position within its area. */
	virtual void getPosition(float &x, float &y, float &z) const;
	/** Return the object's orientation. */
	virtual void getOrientation(float &x, float &y, float &z) const;

	/** Set the object's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the object's orientation. */
	virtual void setOrientation(float x, float y, float z);

	/** Create a Location out of the object's area, position and orientation. */
	Location getLocation() const;

	// Object/Cursor interactions

	virtual void enter(); ///< The cursor entered the object. */
	virtual void leave(); ///< The cursor left the object. */

	/** (Un)Highlight the object. */
	virtual void highlight(bool enabled);

	/** The object was clicked. */
	virtual bool click(Object *triggerer = 0);

	// Object (text) talking

	/** Speak the specified string. */
	void speakString(const Common::UString &string, uint32 volume);
	/** Speak an one-liner from the specified conversation file. */
	void speakOneLiner(Common::UString conv, Object *tokenTarget = 0);

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);

	// Animation

	/** Play an object animation. */
	virtual void playAnimation(const Common::UString &animation = "", bool restart = true, int32 loopCount = 0);
	void playAnimation(Animation animation);

protected:
	ObjectType _type; ///< The object's type.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _portrait; ///< The object's portrait.

	Common::UString _conversation; ///< The object's default conversation.

	uint32 _soundSet;      ///< The object's sound set, as an index into soundset.2da.
	Aurora::SSFFile *_ssf; ///< The object's sound set.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	std::list<uint32> _ids; ///< The object's model IDs.

	Aurora::NWScript::Object *_pcSpeaker; ///< The current PC speaking with the object.

	Area *_area; ///< The area the object is currently in.

	float _position[3];    ///< The object's position.
	float _orientation[3]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.


	Object(ObjectType type);

	/** Load the object's sound set. */
	void loadSSF();
	/** Begin a conversation between the triggerer and this object. */
	bool beginConversation(Object *triggerer);
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECT_H

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
 *  Object within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_OBJECT_H
#define ENGINES_KOTORBASE_OBJECT_H

#include <list>

#include "src/aurora/nwscript/object.h"

#include "src/sound/types.h"

#include "src/engines/kotorbase/types.h"

#include "src/engines/kotorbase/script/container.h"

namespace Engines {

namespace KotORBase {

class Room;
class Location;

class Object : public Aurora::NWScript::Object, public ScriptContainer {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	/** Return the object's model IDs. */
	const std::list<uint32_t> &getIDs() const;

	/** Is the object's model(s) visible? */
	virtual bool isVisible() const;

	/** Show the object's model(s). */
	virtual void show();
	/** Hide the object's model(s). */
	virtual void hide();
	/** Notify the object that it is no longer seen. */
	virtual void notifyNotSeen();

	// Basic properties

	/** Return the object's template resource name. */
	const Common::UString &getTemplateResRef() const;
	/** Return the object's name. */
	const Common::UString &getName() const;
	/** Return the object's description. */
	const Common::UString &getDescription() const;
	/** Return the object's portrait. */
	const Common::UString &getPortrait() const;
	/** Get a room the object is in. */
	const Room *getRoom();

	/** Set a room the object is in. */
	void setRoom(const Room *room);

	// Interactive properties

	virtual const std::vector<int> getPossibleActions() const;

	/** Is the object static (not manipulable at all)? */
	bool isStatic() const;
	/** Can the object be used by the PC? */
	bool isUsable() const;
	/** Can the player click the object? */
	bool isClickable() const;

	/** Toggle usability of this object. */
	virtual void setUsable(bool usable);

	// Positioning

	/** Return the object's position within its area. */
	virtual void getPosition(float &x, float &y, float &z) const;
	/** Return the object's orientation. */
	virtual void getOrientation(float &x, float &y, float &z, float &angle) const;

	/** Set the object's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the object's orientation. */
	virtual void setOrientation(float x, float y, float z, float angle);

	/** Make the object look at the specified location. */
	void makeLookAt(float x, float y);
	/** Make the object look at the specified object. */
	void makeLookAt(Object *target);

	/** Create a Location out of the object's area, position and orientation. */
	Location getLocation() const;

	float getDistanceTo(const Object *other) const;

	// Hit points

	/** Get the maximum hit points for the objects. */
	int getMaxHitPoints();
	/** Return the objects current hitpoints. */
	int getCurrentHitPoints();
	/** Get if the object has a minimum of one hp. */
	bool getMinOneHitPoints() const;

	/** Set the maximum hit points for the objects. */
	void setMaxHitPoints(int maxHP);
	/** Set the current hitpoints. */
	void setCurrentHitPoints(int hitpoints);
	/** Set if the object has a minimum of one hp. */
	void setMinOneHitPoints(bool enabled);

	// Faction

	/** Get the faction this object belongs to. */
	Faction getFaction() const;
	/** Set the faction of this object. */
	void setFaction(Faction faction);

	/** Is this object considered to be an enemy? */
	bool isEnemy() const;

	// Object/Cursor interactions

	virtual const Common::UString &getCursor() const;

	/** The cursor entered the object. */
	virtual void enter();
	/** The cursor left the object. */
	virtual void leave();
	/** (Un)highlight the object. */
	virtual void highlight(bool enabled);
	/** The object is clicked on. */
	virtual bool click(Object *triggerer = 0);

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);

	// Animation

	/** Play the object's model animation. */
	virtual void playAnimation(const Common::UString &anim,
	                           bool restart = true,
	                           float length = 0.0f,
	                           float speed = 1.0f);

	// Tooltip

	virtual void getTooltipAnchor(float &x, float &y, float &z) const;


	virtual bool isDead() const;

protected:
	Common::UString _templateResRef;
	ObjectType _type; ///< The object's type.

	std::list<uint32_t> _ids; ///< The object's model IDs.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.
	Common::UString _portrait;    ///< The object's portrait.
	const Room *_room;            ///< Room the object is in.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	Faction _faction; ///< To which faction does this object belong to.

	int _currentHitPoints; ///< The current hitpoints of the object.
	int _maxHitPoints;     ///< The maximum hitpoints of the object.
	bool _minOneHitPoint;  ///< If the object should have at least one hitpoint.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_OBJECT_H

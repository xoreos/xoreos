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
 *  An object in a Star Wars: Knights of the Old Republic area.
 */

#ifndef ENGINES_KOTOR_OBJECT_H
#define ENGINES_KOTOR_OBJECT_H

#include <list>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/object.h"

#include "src/sound/types.h"

#include "src/engines/kotor/types.h"

#include "src/engines/kotor/script/container.h"

namespace Engines {

namespace KotOR {

class Object : public Aurora::NWScript::Object, public KotOR::ScriptContainer {
public:
	Object(ObjectType type = kObjectTypeInvalid);
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

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

	/** Set the maximum hit points for the objects. */
	void setMaxHitPoints(int maxHP);
	/** Get the maximum hit points for the objects. */
	int getMaxHitPoints();
	/** Set the current hitpoints. */
	void setCurrentHitPoints(int hitpoints);
	/** Return the objects current hitpoints. */
	int getCurrentHitPoints();
	/** Set if the object has a minimum of one hp. */
	void setMinOneHitPoints(bool enabled);
	/** Get if the object has a minimum of one hp. */
	bool getMinOneHitPoints() const;

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

protected:
	ObjectType _type; ///< The object's type.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _portrait; ///< The object's portrait.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	int _currentHitPoints; ///< The current hitpoints of the object.
	int _maxHitPoints; ///< The maximum hitpoints of the object.
	bool _minOneHitPoint; ///< If the object should have at least one hitpoint.

	std::list<uint32> _ids; ///< The object's model IDs.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_OBJECT_H

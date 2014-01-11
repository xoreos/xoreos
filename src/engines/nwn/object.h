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

#include "engines/nwn/types.h"
#include "engines/nwn/location.h"

namespace Aurora {
	class SSFFile;
}

namespace Engines {

namespace NWN {

class Area;

/** An object within a NWN area. */
class Object : public Aurora::NWScript::Object {
public:
	virtual ~Object();

	/** Return the exact type of the object. */
	ObjectType getType() const;

	// Basic visuals

	/** Load the object's model(s). */
	virtual void loadModel();
	/** Unload the object's model(s). */
	virtual void unloadModel();

	/** Is the object currently visible? */
	virtual bool isVisible() const;

	/** Show/Hide the object's model(s). */
	virtual void setVisible(bool visible);

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

	/** Return the IDs of the object's model(s). */
	const std::vector<Common::UString> &getModelIDs() const;

	/** Is the object static (not manipulatable at all)? */
	bool isStatic() const;
	/** Can the object be used by the PC? */
	bool isUsable() const;

	/** Can the player click the object? */
	bool isClickable() const;

	// Mouse/Object interactions

	/** Is the object currently highlighed? */
	virtual bool getHighlight() const;

	/** Highlight/Dehighlight the object. */
	virtual void setHighlight(bool highlight);

	// Object/Object interactions

	/** Return the area this object is currently in. */
	Area *getArea() const;
	/** Set the area this object is currently in. */
	void setArea(Area *);

	// Positioning

	/** Return the object's position within its area. */
	virtual void getPosition(float &x, float &y, float &z) const;
	/** Return the object's orientation. */
	virtual void getOrientation(float &radian, float &x, float &y, float &z) const;

	/** Set the object's position within its area. */
	virtual void setPosition(float x, float y, float z);
	/** Set the object's orientation. */
	virtual void setOrientation(float radian, float x, float y, float z);

	/** Move the object relative to its current position. */
	virtual void move(float x, float y, float z);
	/** Rotate the object relative to its current orientation. */
	virtual void rotate(float radian, float x, float y, float z);

	/** Create a Location out of the object's area, position and orientation. */
	Location getLocation() const;

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);

protected:
	ObjectType _type; ///< The object's type.

	std::vector<Common::UString> _modelIDs; ///< IDs of the object's model(s).

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _portrait; ///< The object's portrait.

	Common::UString _conversation; ///< The object's default conversation.

	uint32 _soundSet;      ///< The object's sound set, as an index into soundset.2da.
	Aurora::SSFFile *_ssf; ///< The object's sound set.

	bool _visible; ///< Is the object currently visible?

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	bool _highlight; ///< Is the object currently highlighted?

	Area *_area; ///< The area the object is currently in.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.


	Object(ObjectType type);

	/** Load the object's sound set. */
	void loadSSF();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECT_H

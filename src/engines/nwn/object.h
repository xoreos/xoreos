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
 *  An object in a Neverwinter Nights area.
 */

#ifndef ENGINES_NWN_OBJECT_H
#define ENGINES_NWN_OBJECT_H

#include <list>
#include <memory>

#include "src/common/types.h"
#include "src/common/ustring.h"

#include "src/aurora/nwscript/object.h"

#include "src/sound/types.h"

#include "src/engines/nwn/types.h"
#include "src/engines/nwn/location.h"

#include "src/engines/nwn/gui/widgets/tooltip.h"

#include "src/engines/nwn/script/container.h"

namespace Aurora {
	class SSFFile;

	namespace NWScript {
		class Object;
	}
}

namespace Engines {

namespace NWN {

class Area;

class Object : public Aurora::NWScript::Object, public NWN::ScriptContainer {
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
	/** Return the object's portrait. */
	const Common::UString &getPortrait() const;

	/** Return the object's default conversation (DLG). */
	const Common::UString &getConversation() const;

	/** Return the object's sound set. */
	const Aurora::SSFFile *getSSF();

	// Interactive properties

	bool isStatic() const; ///< Is the object static (not manipulable at all)?
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
	virtual void setArea(Area *);

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
	virtual void speakString(const Common::UString &string, uint32_t volume);

	/** Speak an one-liner from the specified conversation file. */
	void speakOneLiner(Common::UString conv, Object *tokenTarget = 0);

	// Sound

	/** Stop the current object sound. */
	void stopSound();
	/** Play an object sound. */
	void playSound(const Common::UString &sound, bool pitchVariance = false);

	// Animation

	/** Play an object animation. */
	virtual void playAnimation(const Common::UString &animation = "", bool restart = true,
	                           float length = 0.0f, float speed = 1.0f);
	/** Play a default object animation. */
	virtual void playAnimation(Animation animation, bool restart = true,
	                           float length = 0.0f, float speed = 1.0f);

protected:
	ObjectType _type; ///< The object's type.

	Common::UString _name;        ///< The object's display name.
	Common::UString _description; ///< The object's description.

	Common::UString _portrait; ///< The object's portrait.

	Common::UString _conversation; ///< The object's default conversation.

	uint32_t _soundSet; ///< The object's sound set, as an index into soundset.2da.
	std::unique_ptr<Aurora::SSFFile> _ssf; ///< The object's sound set.

	bool _static; ///< Is the object static?
	bool _usable; ///< Is the object usable?

	std::list<uint32_t> _ids; ///< The object's model IDs.

	Aurora::NWScript::Object *_pcSpeaker; ///< The current PC speaking with the object.

	Area *_area; ///< The area the object is currently in.

	float _position[3];    ///< The object's position.
	float _orientation[4]; ///< The object's orientation.

	Sound::ChannelHandle _sound; ///< The currently playing object sound.

	std::unique_ptr<Tooltip> _tooltip; ///< The tooltip displayed over the object.


	/** Load the object's sound set. */
	void loadSSF();
	/** Begin a conversation between the triggerer and this object. */
	bool beginConversation(Object *triggerer);

	/** Create an empty tooltip. */
	virtual bool createTooltip(Tooltip::Type type);

	/** Create a tooltip with the name and/or portrait of the object. */
	bool createFeedbackTooltip();
	/** Create a tooltip with a line the object should speak. */
	bool createSpeechTooltip(const Common::UString &line);

	/** Destroy all tooltips on this object. */
	void destroyTooltip();

	/** Create and show a tooltip with the name and/or portrait of the object. */
	bool showFeedbackTooltip();
	/** Create and show a tooltip with a line the object speaks. */
	bool showSpeechTooltip(const Common::UString &line);
	/** Hide the tooltip again. */
	void hideTooltip();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECT_H

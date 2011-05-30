/* eos - A reimplementation of BioWare's Aurora engine
 *
 * eos is the legal property of its developers, whose names can be
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
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
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

#include "engines/nwn/scriptcontainer.h"

namespace Aurora {
	class SSFFile;

	namespace NWScript {
		class Object;
	}
}

namespace Engines {

namespace NWN {

/** An object within a NWN area. */
class Object : public Aurora::NWScript::Object, public ScriptContainer {
public:
	Object(ObjectType type);
	virtual ~Object();

	ObjectType getType() const;

	bool loaded() const;

	virtual void show();
	virtual void hide();

	const Common::UString &getName() const;
	const Common::UString &getDescription() const;

	const Common::UString &getPortrait() const;

	const Common::UString &getConversation() const;

	const Aurora::SSFFile *getSSF();

	bool isStatic() const;
	bool isUsable() const;

	bool isClickable() const;

	const std::list<uint32> &getIDs() const;

	Aurora::NWScript::Object *getPCSpeaker() const;
	void setPCSpeaker(Aurora::NWScript::Object *pc);

	Aurora::NWScript::Object *getArea() const;
	void setArea(Aurora::NWScript::Object *area);

	virtual void getPosition(float &x, float &y, float &z) const;
	virtual void getOrientation(float &x, float &y, float &z) const;

	virtual void setPosition(float x, float y, float z);
	virtual void setOrientation(float x, float y, float z);

	virtual void enter();
	virtual void leave();

	virtual void highlight(bool enabled);

	virtual void click();


	void stopSound();
	void playSound(const Common::UString &sound, bool pitchVariance = false);


protected:
	ObjectType _type;

	bool _loaded;

	Common::UString _name;
	Common::UString _description;

	Common::UString _portrait;

	Common::UString _conversation;

	uint32 _soundSet;

	Aurora::SSFFile *_ssf;

	bool _static;
	bool _usable;

	std::list<uint32> _ids;

	Aurora::NWScript::Object *_pcSpeaker;
	Aurora::NWScript::Object *_area;

	float _position[3];
	float _orientation[3];

	Sound::ChannelHandle _sound;


	void clear();

	void loadSSF();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECT_H

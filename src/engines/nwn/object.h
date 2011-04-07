/* eos - A reimplementation of BioWare's Aurora engine
 * Copyright (c) 2010-2011 Sven Hesse (DrMcCoy), Matthew Hoops (clone2727)
 *
 * The Infinity, Aurora, Odyssey and Eclipse engines, Copyright (c) BioWare corp.
 * The Electron engine, Copyright (c) Obsidian Entertainment and BioWare corp.
 *
 * This file is part of eos and is distributed under the terms of
 * the GNU General Public Licence. See COPYING for more informations.
 */

/** @file engines/nwn/object.h
 *  An object within a NWN area.
 */

#ifndef ENGINES_NWN_OBJECT_H
#define ENGINES_NWN_OBJECT_H

#include <list>

#include "common/types.h"
#include "common/ustring.h"

namespace Engines {

namespace NWN {

/** An object within a NWN area. */
class Object {
public:
	Object();
	virtual ~Object();

	virtual void show() = 0;
	virtual void hide() = 0;

	const Common::UString &getTag() const;
	const Common::UString &getName() const;
	const Common::UString &getDescription() const;

	const Common::UString &getPortrait() const;

	bool isStatic() const;
	bool isUsable() const;

	bool isClickable() const;

	const std::list<uint32> &getIDs() const;

	virtual void getPosition(float &x, float &y, float &z) const;
	virtual void getOrientation(float &x, float &y, float &z) const;

	virtual void setPosition(float x, float y, float z);
	virtual void setOrientation(float x, float y, float z);

	virtual void enter() = 0;
	virtual void leave() = 0;

	virtual void highlight(bool enabled) = 0;

protected:
	bool _loaded;

	Common::UString _tag;
	Common::UString _name;
	Common::UString _description;

	Common::UString _portrait;

	bool _static;
	bool _usable;

	std::list<uint32> _ids;

	float _position[3];
	float _orientation[3];

	void clear();
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_OBJECT_H

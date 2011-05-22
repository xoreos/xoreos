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

/** @file aurora/nwscript/variable.h
 *  NWScript variable.
 */

#ifndef AURORA_NWSCRIPT_VARIABLE_H
#define AURORA_NWSCRIPT_VARIABLE_H

#include <vector>

#include "common/types.h"
#include "common/ustring.h"

#include "aurora/types.h"

#include "aurora/nwscript/types.h"

namespace Aurora {

namespace NWScript {

class Object;

class Variable {
public:
	Variable(Type type = kTypeVoid);
	Variable(const Variable &var);
	~Variable();

	void setType(Type type);

	Variable &operator=(const Variable &var);

	Variable &operator=(int32 value);
	Variable &operator=(float value);
	Variable &operator=(const Common::UString &value);
	Variable &operator=(Object *value);

	// TODO: Arithmetic operators

	bool operator==(const Variable &var) const;
	bool operator!=(const Variable &var) const;

	// TODO: Other comparison operators

	Type getType() const;

	int32 getInt() const;
	float getFloat() const;
	const Common::UString &getString() const;
	Object *getObject() const;

private:
	Type _type;

	union {
		int32 _int;
		float _float;
		Common::UString *_string;
		Object *_object;
	} _value;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_VARIABLE_H

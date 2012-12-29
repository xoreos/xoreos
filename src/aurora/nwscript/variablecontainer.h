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

/** @file aurora/nwscript/variablecontainer.h
 *  An NWScript variable container.
 */

#ifndef AURORA_NWSCRIPT_VARIABLECONTAINER_H
#define AURORA_NWSCRIPT_VARIABLECONTAINER_H

#include <map>

#include "common/ustring.h"

#include "aurora/nwscript/variable.h"

namespace Aurora {

namespace NWScript {

class VariableContainer {
public:
	VariableContainer();
	virtual ~VariableContainer();

	bool hasVariable(const Common::UString &var) const;

	Variable &getVariable(const Common::UString &var, Type type = kTypeVoid);
	const Variable &getVariable(const Common::UString &var) const;

	void setVariable(const Common::UString &var, const Variable &value);

	void removeVariable(const Common::UString &var);
	void clearVariables();

private:
	typedef std::map<Common::UString, Variable> VariableMap;

	VariableMap _variables;
};

} // End of namespace NWScript

} // End of namespace Aurora

#endif // AURORA_NWSCRIPT_VARIABLECONTAINER_H

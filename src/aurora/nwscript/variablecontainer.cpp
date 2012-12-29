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

/** @file aurora/nwscript/variablecontainer.cpp
 *  An NWScript variable container.
 */

#include "common/error.h"

#include "aurora/nwscript/variablecontainer.h"

namespace Aurora {

namespace NWScript {

VariableContainer::VariableContainer() {
}

VariableContainer::~VariableContainer() {
}

bool VariableContainer::hasVariable(const Common::UString &var) const {
	return _variables.find(var) != _variables.end();
}

Variable &VariableContainer::getVariable(const Common::UString &var, Type type) {
	VariableMap::iterator v = _variables.find(var);
	if ((v == _variables.end()) && (type != kTypeVoid)) {
		std::pair<VariableMap::iterator, bool> result;

		result = _variables.insert(std::make_pair(var, Variable(type)));

		v = result.first;
	}

	return v->second;
}

const Variable &VariableContainer::getVariable(const Common::UString &var) const {
	VariableMap::const_iterator v = _variables.find(var);
	if (v == _variables.end())
		throw Common::Exception("VariableContainer::getVariable(): No such variable \"%s\"", var.c_str());

	return v->second;
}

void VariableContainer::setVariable(const Common::UString &var, const Variable &value) {
	_variables[var] = value;
}

void VariableContainer::removeVariable(const Common::UString &var) {
	VariableMap::iterator v = _variables.find(var);
	if (v != _variables.end())
		_variables.erase(v);
}

void VariableContainer::clearVariables() {
	_variables.clear();
}

} // End of namespace NWScript

} // End of namespace Aurora

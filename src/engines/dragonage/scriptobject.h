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
 *  An object context from the view of the script interpreter.
 */

#ifndef ENGINES_DRAGONAGE_SCRIPTOBJECT_H
#define ENGINES_DRAGONAGE_SCRIPTOBJECT_H

#include "src/aurora/types.h"

#include "src/aurora/nwscript/object.h"

#include "src/engines/dragonage/types.h"

namespace Engines {

namespace DragonAge {

class ScriptObject : public ::Aurora::NWScript::Object {
public:
	ScriptObject(ObjectType type);
	~ScriptObject();

	ObjectType getObjectType() const;

protected:
	void readVarTable(const Aurora::GFF3List &varTable);
	void readVarTable(const Aurora::GFF4List &varTable);

private:
	ObjectType _type;
};

} // End of namespace DragonAge

} // End of namespace Engines

#endif // ENGINES_DRAGONAGE_SCRIPTOBJECT_H

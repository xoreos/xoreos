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

#include "src/common/util.h"
#include "src/common/ustring.h"
#include "src/common/error.h"

#include "src/aurora/gff3file.h"
#include "src/aurora/gff4file.h"

#include "src/engines/dragonage2/scriptobject.h"

namespace Engines {

namespace DragonAge2 {

static const uint32 kVARSID = MKTAG('V', 'A', 'R', 'S');

using ::Aurora::GFF3File;
using ::Aurora::GFF3Struct;
using ::Aurora::GFF3List;

using ::Aurora::GFF4File;
using ::Aurora::GFF4Struct;
using ::Aurora::GFF4List;

using namespace ::Aurora::GFF4FieldNamesEnum;


ScriptObject::ScriptObject(ObjectType type) : _type(type) {
}

ScriptObject::~ScriptObject() {
}

ObjectType ScriptObject::getObjectType() const {
	return _type;
}

void ScriptObject::readVarTable(const GFF3List &varTable) {
	for (GFF3List::const_iterator v = varTable.begin(); v != varTable.end(); ++v) {
		const Common::UString name  = (*v)->getString ("Name");
		const uint8           type  = (*v)->getUint   ("Type");

		if (name.empty())
			continue;

		switch (type) {
			case  0:
			case  4:
				setVariable(name, Aurora::NWScript::Variable());
				break;

			case  1:
				setVariable(name, (int32) (*v)->getSint("Value"));
				break;

			case  2:
				setVariable(name, (float) (*v)->getDouble("Value"));
				break;

			case  3:
			case 12:
				setVariable(name, (*v)->getString("Value"));
				break;

			default:
				throw Common::Exception("Unknown variable type %u (\"%s\")", type, name.c_str());
		}
	}
}

void ScriptObject::readVarTable(const GFF4List &varTable) {
	for (GFF4List::const_iterator v = varTable.begin(); v != varTable.end(); ++v) {
		if (!*v || ((*v)->getLabel() != kVARSID))
			continue;

		const Common::UString name  = (*v)->getString (kGFF4ScriptVarTableName);
		const uint8           type  = (*v)->getUint   (kGFF4ScriptVarTableType);
		const GFF4Struct     *value = (*v)->getGeneric(kGFF4ScriptVarTableValue);

		if (name.empty() || (type == 0) || !value || !value->hasField(0))
			continue;

		switch (type) {
			case  4:
				setVariable(name, Aurora::NWScript::Variable());
				break;

			case  1:
				setVariable(name, (int32) value->getSint(0));
				break;

			case  2:
				setVariable(name, (float) value->getDouble(0));
				break;

			case  3:
			case 12:
				setVariable(name, value->getString(0));
				break;

			default:
				throw Common::Exception("Unknown variable type %u (\"%s\")", type, name.c_str());
		}
	}
}

} // End of namespace DragonAge2

} // End of namespace Engines

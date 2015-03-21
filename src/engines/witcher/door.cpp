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
 *  A door within a Witcher area.
 */

#include "src/common/util.h"

#include "src/aurora/gfffile.h"

#include "src/graphics/aurora/model.h"

#include "src/engines/witcher/door.h"
#include "src/engines/witcher/waypoint.h"
#include "src/engines/witcher/module.h"

namespace Engines {

namespace Witcher {

Door::Door(Module &module, const Aurora::GFFStruct &door) : Situated(kObjectTypeDoor),
	_module(&module), _link(0) {

	load(door);
}

Door::~Door() {
}

void Door::load(const Aurora::GFFStruct &door) {
	Common::UString temp = door.getString("TemplateResRef");

	Aurora::GFFFile *utd = 0;
	if (!temp.empty()) {
		try {
			utd = new Aurora::GFFFile(temp, Aurora::kFileTypeUTD, MKTAG('U', 'T', 'D', ' '));
		} catch (...) {
		}
	}

	Situated::load(door, utd ? &utd->getTopLevel() : 0);

	delete utd;
}

void Door::loadObject(const Aurora::GFFStruct &gff) {
	_linkTag = gff.getString("LinkedTo");
}

void Door::enter() {
	highlight(true);

	evaluateLink();
}

void Door::leave() {
	highlight(false);
}

void Door::highlight(bool enabled) {
	if (_model)
		_model->drawBound(enabled);
}

bool Door::click(Object *UNUSED(triggerer)) {
	return true;
}

void Door::evaluateLink() {
	if (_link || _linkTag.empty())
		return;

	Aurora::NWScript::Object *object = _module->findObject(_linkTag);
	_link = dynamic_cast<Waypoint *>(object);
}

} // End of namespace Witcher

} // End of namespace Engines

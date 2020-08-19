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
 *  Trigger in a Neverwinter Nights 2 area.
 */

#include "src/common/endianness.h"

#include "src/aurora/resman.h"
#include "src/aurora/gff3file.h"

#include "src/engines/aurora/util.h"

#include "src/engines/nwn2/trigger.h"
#include "src/engines/nwn2/area.h"
#include "src/engines/nwn2/trap.h"

namespace Engines {

namespace NWN2 {

Trigger::Trigger(const Aurora::GFF3Struct &gff)
		: ::Engines::Trigger(),
		  Object(kObjectTypeTrigger),
		  Trap(gff) {
	load(gff);
}

void Trigger::show() {
	Renderable::show();
}

void Trigger::hide() {
	Renderable::hide();
}

void Trigger::notifyNotSeen() {
}

void Trigger::createTrap(uint8_t trapType, uint32_t faction,
                         const Common::UString &disarm,
                         const Common::UString &triggered) {
	Trap::createTrap(trapType, faction, disarm, triggered);
	_faction = faction; // Overriding trigger faction

	setScript(kScriptDisarm, disarm);
	if (triggered != "")
		setScript(kScriptTrapTriggered, triggered);
}

bool Trigger::isVisible() const {
	return Renderable::isVisible();
}

void Trigger::load(const Aurora::GFF3Struct &gff) {
	Common::UString temp = gff.getString("TemplateResRef");

	std::unique_ptr<Aurora::GFF3File> utt;
	if (!temp.empty())
		utt.reset(loadOptionalGFF3(temp, Aurora::kFileTypeUTT, MKTAG('U', 'T', 'T', ' ')));

	loadBlueprint(utt->getTopLevel());

	if (!utt)
		warning("Trigger \"%s\" has no blueprint", temp.c_str());

	// Faction

	_faction = gff.getUint("Faction", _faction);

	float x, y, z;
	x = (float)gff.getDouble("XPosition");
	y = (float)gff.getDouble("YPosition");
	z = (float)gff.getDouble("ZPosition");
	glm::vec3 position(x, y, z);
	setPosition(x, y, z);

	x = (float)gff.getDouble("XOrientation");
	y = (float)gff.getDouble("YOrientation");
	z = (float)gff.getDouble("ZOrientation");
	glm::vec3 orientation(x, y, z);

	const Aurora::GFF3List &geometry = gff.getList("Geometry");
	for (Aurora::GFF3List::const_iterator p = geometry.begin();
			p != geometry.end();
			++p) {
		x = (float)(*p)->getDouble("PointX");
		y = (float)(*p)->getDouble("PointY");
		z = (float)(*p)->getDouble("PointZ");
		_geometry.push_back(position + glm::vec3(x, y, z));
	}
}

void Trigger::loadBlueprint(const Aurora::GFF3Struct &gff) {
	Object::_tag = gff.getString("Tag");

	_name = gff.getString("LocalizedName");

	readScripts(gff);
}

uint8_t Trigger::getReputation(Object *source) const {
	return getArea()->getFactionReputation(source, _faction);
}

} // End of namespace NWN2

} // End of namespace Engines

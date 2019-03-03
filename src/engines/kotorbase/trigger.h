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
 *  Trigger within an area in KotOR games.
 */

#ifndef ENGINES_KOTORBASE_TRIGGER_H
#define ENGINES_KOTORBASE_TRIGGER_H

#include "src/engines/aurora/trigger.h"

#include "src/engines/kotorbase/object.h"

namespace Engines {

namespace KotORBase {

class Trigger : public ::Engines::Trigger, public Object {
public:
	Trigger(const Aurora::GFF3Struct &gff);

	// Basic visuals

	bool isVisible() const;

	void show();
	void hide();
	void notifyNotSeen();

protected:
	void load(const Aurora::GFF3Struct &gff);
	void loadBlueprint(const Aurora::GFF3Struct &gff);
};

} // End of namespace KotORBase

} // End of namespace Engines

#endif // ENGINES_KOTORBASE_TRIGGER_H

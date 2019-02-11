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
 *  Creature action in Star Wars: Knights of the Old Republic.
 */

#ifndef ENGINES_KOTOR_ACTION_H
#define ENGINES_KOTOR_ACTION_H

#include "src/engines/kotorbase/types.h"

namespace Engines {

namespace KotOR {

class Object;

class Action {
public:
	Action(ActionType type);
	Action(const Action &action);

	void setPoint(float x, float y, float z);
	void setObject(Object *o);
	void setRange(float range);

	ActionType getType() const;
	void getPoint(float &x, float &y, float &z) const;
	Object *getObject() const;
	float getRange() const;

private:
	ActionType _type;
	float _point[3];
	Object *_object;
	float _range;
};

} // End of namespace KotOR

} // End of namespace Engines

#endif // ENGINES_KOTOR_ACTION_H

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
 *  A location within a Neverwinter Nights area.
 */

#ifndef ENGINES_NWN_LOCATION_H
#define ENGINES_NWN_LOCATION_H

#include "src/aurora/nwscript/enginetype.h"

namespace Engines {

namespace NWN {

class Area;

class Location : public Aurora::NWScript::EngineType {
public:
	Location() = default;
	Location(const Location &) = default;
	~Location() = default;

	/** Clone factory method. */
	Location *clone() const;

	/** Return the location's area. */
	Area *getArea() const;
	/** Set the location's area. */
	void setArea(Area *area);

	/** Return the location's position. */
	void getPosition(float &x, float &y, float &z) const;
	/** Set the location's position. */
	void setPosition(float  x, float  y, float  z);

	/** Return the location's orientation. */
	float getFacing() const;
	/** Set the location's orientation. */
	void setFacing(float facing);

private:
	Area *_area { nullptr }; ///< The location's area.

	float _position[3] { 0.0f, 0.0f, 0.0f }; ///< The location's position.
	float _facing { 0.0f };                  ///< The location's orientation.
};

} // End of namespace NWN

} // End of namespace Engines

#endif // ENGINES_NWN_LOCATION_H

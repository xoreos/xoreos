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
 *  Creature search criteria for KotOR games.
 */

#include "src/engines/kotorbase/creaturesearch.h"

namespace Engines {

namespace KotORBase {

CreatureSearchCriteria::CreatureSearchCriteria() :
		firstCriteriaType(kCreatureTypeInvalid),
		firstCriteriaValue(0),
		secondCriteriaType(kCreatureTypeInvalid),
		secondCriteriaValue(0),
		thirdCriteriaType(kCreatureTypeInvalid),
		thirdCriteriaValue(0) {
}

CreatureSearchCriteria::CreatureSearchCriteria(CreatureType _firstCriteriaType, int _firstCriteriaValue) :
		firstCriteriaType(_firstCriteriaType),
		firstCriteriaValue(_firstCriteriaValue),
		secondCriteriaType(kCreatureTypeInvalid),
		secondCriteriaValue(0),
		thirdCriteriaType(kCreatureTypeInvalid),
		thirdCriteriaValue(0) {
}

} // End of namespace KotORBase

} // End of namespace Engines

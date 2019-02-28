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
 *  Wrapper for random number generation.
 */

#ifndef COMMON_RANDOM_H
#define COMMON_RANDOM_H

#include <random>

#include "src/common/singleton.h"

namespace Common {

class Random : public Singleton<Random> {
public:
	Random();

	/** Generate a random number between min (inclusive) and max (exclusive). */
	int getNext(int min, int max);

	/** Generate a random number between min (inclusive) and max (exclusive). */
	float getNext(float min, float max);

private:
	std::mt19937 _generator;
};

} // End of namespace Common

#define RNG Common::Random::instance()

#endif // COMMON_RANDOM_H

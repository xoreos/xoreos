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

#include "src/common/random.h"

DECLARE_SINGLETON(Common::Random);

namespace Common {

Random::Random() {
	std::random_device rd;
	_generator.seed(rd());
}

int Random::getNext(int min, int max) {
	std::uniform_int_distribution<int> dist(min, max - 1);
	return dist(_generator);
}

float Random::getNext(float min, float max) {
	std::uniform_real_distribution<float> dist(min, max);
	return dist(_generator);
}

} // End of namespace Common

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
 *  Counting FPS.
 */

#ifndef GRAPHICS_FPSCOUNTER_H
#define GRAPHICS_FPSCOUNTER_H

#include <memory>

#include "src/common/types.h"

namespace Graphics {

/** A class counting frames per second. */
class FPSCounter  {
public:
	/** Average the FPS over that many seconds. */
	FPSCounter(size_t secs);
	~FPSCounter();

	/** Get the current FPS value. */
	uint32_t getFPS() const;

	/** Reset the counter. */
	void reset();

	/** Signal a finished frame. */
	void finishedFrame();

private:
	uint32_t _lastSampled;     ///< The last time a finished frame was signaled.

	size_t _seconds;       ///< Number of seconds over which to average the FPS.
	size_t _currentSecond; ///< Current second we measure.

	bool _hasFullSeconds; ///< Got all samples?

	uint32_t _fps; ///< The current FPS value.

	std::unique_ptr<uint32_t[]> _frames; ///< All frame counters.

	void calculateFPS(); ///< Calculate the average FPS value.
};

} // End of namespace Graphics

#endif // GRAPHICS_FPSCOUNTER_H

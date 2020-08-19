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

#include <cassert>

#include "src/common/util.h"

#include "src/graphics/fpscounter.h"

#include "src/events/events.h"

namespace Graphics {

FPSCounter::FPSCounter(size_t secs) : _seconds(secs) {
	assert(_seconds > 0);

	_frames = std::make_unique<uint32_t[]>(_seconds);

	reset();
}

FPSCounter::~FPSCounter() {
}

uint32_t FPSCounter::getFPS() const {
	return _fps;
}

void FPSCounter::reset() {
	_lastSampled = 0;

	_currentSecond = 0;

	_hasFullSeconds = false;

	_fps = 0;

	for (size_t i = 0; i < _seconds; i++)
		_frames[i] = 0;
}

void FPSCounter::finishedFrame() {
	uint32_t now = EventMan.getTimestamp();

	if (_lastSampled == 0)
		_lastSampled = now;

	if ((now - _lastSampled) >= 1000) {
		// We had one second worth of frames

		// Advance to the next second
		_currentSecond = (_currentSecond + 1) % _seconds;
		_lastSampled = now;

		// We filled all samples
		if (_currentSecond == 0)
			_hasFullSeconds = true;

		// Calculate the new FPS value
		calculateFPS();

		// Reset the counter
		_frames[_currentSecond] = 0;
	}

	// Another frame!
	_frames[_currentSecond]++;
}

void FPSCounter::calculateFPS() {
	size_t seconds = _hasFullSeconds ? _seconds : _currentSecond;
	uint32_t frames = 0;
	for (size_t i = 0; i < seconds; i++)
		frames += _frames[i];

	_fps = seconds ? (frames / seconds) : 0;
}

} // End of namespace Graphics

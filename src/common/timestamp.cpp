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
 *  Lossless timestamping.
 */

/* Based on ScummVM (<http://scummvm.org>) code, which is released
 * under the terms of version 2 or later of the GNU General Public
 * License.
 *
 * The original copyright note in ScummVM reads as follows:
 *
 * ScummVM is the legal property of its developers, whose names
 * are too numerous to list here. Please refer to the COPYRIGHT
 * file distributed with this source distribution.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <cassert>

#include "src/common/algorithm.h"
#include "src/common/error.h"
#include "src/common/rational.h"
#include "src/common/timestamp.h"

namespace Common {

Timestamp::Timestamp(uint64_t ms, uint64_t fr) {
	if (fr == 0)
		throw Exception("Invalid frame rate 0");

	_secs = ms / 1000;
	_framerateFactor = 1000 / gcd<uint64_t>(1000, fr);
	_framerate = fr * _framerateFactor;

	// Note that _framerate is always divisible by 1000.
	_numFrames = (ms % 1000) * (_framerate / 1000);
}

Timestamp::Timestamp(uint64_t s, uint64_t frames, uint64_t fr) {
	if (fr == 0)
		throw Exception("Invalid frame rate 0");

	_secs = s + (frames / fr);
	_framerateFactor = 1000 / gcd<uint64_t>(1000, fr);
	_framerate = fr * _framerateFactor;
	_numFrames = (frames % fr) * _framerateFactor;
}

Timestamp::Timestamp(uint64_t s, uint64_t frames, const Rational &newFramerate) {
	if (newFramerate <= 0)
		throw Exception("Invalid frame rate %d/%d", newFramerate.getNumerator(), newFramerate.getDenominator());

	uint64_t fr;
	if (newFramerate.getDenominator() == 1) {
		fr = newFramerate.getNumerator();
	} else {
		Rational time = newFramerate.getInverse() * frames;
		frames = time.getNumerator();
		fr = time.getDenominator();
	}

	_secs = s + (frames / fr);
	_framerateFactor = 1000 / gcd<uint64_t>(1000, fr);
	_framerate = fr * _framerateFactor;
	_numFrames = (frames % fr) * _framerateFactor;
}

Timestamp Timestamp::convertToFramerate(uint64_t newFramerate) const {
	assert(_framerate != 0);
	assert(newFramerate != 0);

	Timestamp ts(*this);

	if (ts.framerate() != newFramerate) {
		ts._framerateFactor = 1000 / gcd<uint64_t>(1000, newFramerate);
		ts._framerate = newFramerate * ts._framerateFactor;

		const uint64_t g = gcd(_framerate, ts._framerate);
		const uint64_t p = _framerate / g;
		const uint64_t q = ts._framerate / g;

		// Convert the frame offset to the new framerate.
		// We round to the nearest (as opposed to always
		// rounding down), to minimize rounding errors during
		// round trip conversions.
		ts._numFrames = (ts._numFrames * q + p/2) / p;

		ts.normalize();
	}

	return ts;
}

void Timestamp::normalize() {
	// Convert negative _numFrames values to positive ones by adjusting _secs
	if (_numFrames < 0) {
		int64_t secsub = 1 + (-_numFrames / _framerate);

		_numFrames += _framerate * secsub;
		_secs -= secsub;
	}

	// Wrap around if necessary
	_secs += (_numFrames / _framerate);
	_numFrames %= _framerate;
}

bool Timestamp::operator==(const Timestamp &ts) const {
	return cmp(ts) == 0;
}

bool Timestamp::operator!=(const Timestamp &ts) const {
	return cmp(ts) != 0;
}

bool Timestamp::operator<(const Timestamp &ts) const {
	return cmp(ts) < 0;
}

bool Timestamp::operator<=(const Timestamp &ts) const {
	return cmp(ts) <= 0;
}

bool Timestamp::operator>(const Timestamp &ts) const {
	return cmp(ts) > 0;
}

bool Timestamp::operator>=(const Timestamp &ts) const {
	return cmp(ts) >= 0;
}

int64_t Timestamp::cmp(const Timestamp &ts) const {
	assert(_framerate != 0);
	assert(ts._framerate != 0);

	int64_t delta = _secs - ts._secs;
	if (!delta) {
		const uint64_t g = gcd(_framerate, ts._framerate);
		const uint64_t p = _framerate / g;
		const uint64_t q = ts._framerate / g;

		delta = (_numFrames * q - ts._numFrames * p);
	}

	return delta;
}


Timestamp Timestamp::addFrames(int64_t frames) const {
	Timestamp ts(*this);

	// The frames are given in the original framerate, so we have to
	// adjust by _framerateFactor accordingly.
	ts._numFrames += frames * _framerateFactor;
	ts.normalize();

	return ts;
}

Timestamp Timestamp::addMsecs(int64_t ms) const {
	Timestamp ts(*this);
	ts._secs += ms / 1000;
	// Add the remaining frames. Note that _framerate is always divisible by 1000.
	ts._numFrames += (ms % 1000) * (ts._framerate / 1000);

	ts.normalize();

	return ts;
}

void Timestamp::addIntern(const Timestamp &ts) {
	if (_framerate != ts._framerate)
		throw Exception("Cannot add two timestamps of different frame rates");

	_secs += ts._secs;
	_numFrames += ts._numFrames;

	normalize();
}

Timestamp Timestamp::operator-() const {
	Timestamp result(*this);
	result._secs = -_secs;
	result._numFrames = -_numFrames;
	result.normalize();
	return result;
}

Timestamp Timestamp::operator+(const Timestamp &ts) const {
	Timestamp result(*this);
	result.addIntern(ts);
	return result;
}

Timestamp Timestamp::operator-(const Timestamp &ts) const {
	Timestamp result(*this);
	result.addIntern(-ts);
	return result;
}

int64_t Timestamp::frameDiff(const Timestamp &ts) const {
	assert(_framerate != 0);
	assert(ts._framerate != 0);

	int64_t delta = 0;
	if (_secs != ts._secs)
		delta = (_secs - ts._secs) * _framerate;

	delta += _numFrames;

	if (_framerate == ts._framerate) {
		delta -= ts._numFrames;
	} else {
		// We need to multiply by the quotient of the two framerates.
		// We cancel the GCD in this fraction to reduce the risk of
		// overflows.
		const uint64_t g = gcd(_framerate, ts._framerate);
		const uint64_t p = _framerate / g;
		const uint64_t q = ts._framerate / g;

		delta -= ((long)ts._numFrames * p + q/2) / (long)q;
	}

	return delta / (int64_t)_framerateFactor;
}

int64_t Timestamp::msecsDiff(const Timestamp &ts) const {
	return msecs() - ts.msecs();
}

int64_t Timestamp::msecs() const {
	// Note that _framerate is always divisible by 1000.
	return _secs * 1000 + _numFrames / (_framerate / 1000);
}


} // End of namespace Common

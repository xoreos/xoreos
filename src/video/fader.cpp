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
 *  Testing implementation for the VideoDecoder interface.
 */

#include "src/common/util.h"

#include "src/graphics/images/surface.h"

#include "src/video/fader.h"

#include "src/events/events.h"

namespace Video {

Fader::Fader(uint32 width, uint32 height, int n) : _c(0), _n(n), _lastUpdate(0) {
	initVideo(width, height);
}

Fader::~Fader() {
}

bool Fader::hasTime() const {
	if (!_started)
		return true;

	if ((EventMan.getTimestamp() - _lastUpdate) < 20)
		return true;

	return false;
}

void Fader::processData() {
	uint32 curTime  = EventMan.getTimestamp();
	uint32 diffTime = curTime - _lastUpdate;
	if (_started && (diffTime < 20))
		return;

	if (!_started) {
		_lastUpdate = curTime;
		_started    = true;

		_c = 0;
	} else
		_c += 2;

	// Fade from black to green
	byte *data = _surface->getData();
	for (uint32 i = 0; i < _height; i++) {
		byte *rowData = data;

		for (uint32 j = 0; j < _width; j++, rowData += 4) {
			rowData[0] = 0;
			rowData[1] = _c;
			rowData[2] = 0;
			rowData[3] = 255;
		}

		data += _surface->getWidth() * 4;
	}

	// Keep a red square in the middle
	int xPos = (_width  / 2) - 2;
	int yPos = (_height / 2) - 2;
	int dPos = (yPos * _surface->getWidth() + xPos) * 4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			_surface->getData()[dPos + j * 4 + 0] =   0;
			_surface->getData()[dPos + j * 4 + 1] =   0;
			_surface->getData()[dPos + j * 4 + 2] = 255;
			_surface->getData()[dPos + j * 4 + 3] = 255;
		}
		dPos += _surface->getWidth() * 4;
	}

	_lastUpdate = curTime;

	if (_c == 0)
		if (_n-- <= 0)
			finish();

	_needCopy = true;
}

} // End of namespace Video

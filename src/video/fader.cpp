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

#include <cassert>

#include "src/common/util.h"

#include "src/graphics/images/surface.h"

#include "src/video/fader.h"

#include "src/events/events.h"

namespace Video {

Fader::Fader(uint32_t width, uint32_t height, int n) {
	addTrack(new FaderVideoTrack(width, height, n));
	initVideo();
}

void Fader::decodeNextTrackFrame(VideoTrack &track) {
	assert(_surface);
	static_cast<FaderVideoTrack &>(track).drawFrame(*_surface);
	_needCopy = true;
}

Fader::FaderVideoTrack::FaderVideoTrack(uint32_t width, uint32_t height, int n) : _width(width), _height(height), _curFrame(-1), _c(0), _n(n) {
}

void Fader::FaderVideoTrack::drawFrame(Graphics::Surface &surface) {
	// Fade from black to green
	byte *data = surface.getData();
	for (uint32_t i = 0; i < _height; i++) {
		byte *rowData = data;

		for (uint32_t j = 0; j < _width; j++, rowData += 4) {
			rowData[0] = 0;
			rowData[1] = _c;
			rowData[2] = 0;
			rowData[3] = 255;
		}

		data += surface.getWidth() * 4;
	}

	// Keep a red square in the middle
	int xPos = (_width  / 2) - 2;
	int yPos = (_height / 2) - 2;
	int dPos = (yPos * surface.getWidth() + xPos) * 4;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			surface.getData()[dPos + j * 4 + 0] =   0;
			surface.getData()[dPos + j * 4 + 1] =   0;
			surface.getData()[dPos + j * 4 + 2] = 255;
			surface.getData()[dPos + j * 4 + 3] = 255;
		}
		dPos += surface.getWidth() * 4;
	}

	_c += 2;
	_curFrame++;
}

} // End of namespace Video

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

#ifndef VIDEO_FADER_H
#define VIDEO_FADER_H

#include "src/video/decoder.h"

namespace Video {

/** A quick test fader "video". */
class Fader : public VideoDecoder {
public:
	Fader(uint32 width, uint32 height, int n);
	~Fader();

	bool hasTime() const;

protected:
	void processData();

private:
	byte _c;
	int _n;

	uint32 _lastUpdate;
};

} // End of namespace Video

#endif // VIDEO_FADER_H

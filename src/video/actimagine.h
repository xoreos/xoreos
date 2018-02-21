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
 *  Decoding Actimagine videos.
 */

#ifndef VIDEO_ACTIMAGINE_H
#define VIDEO_ACTIMAGINE_H

#include "src/common/types.h"
#include "src/common/scopedptr.h"

#include "src/video/decoder.h"

namespace Common {
	class SeekableReadStream;
}

namespace Video {

/** A decoder for Actimagine videos. */
class ActimagineDecoder : public VideoDecoder {
public:
	ActimagineDecoder(Common::SeekableReadStream *vx);
	~ActimagineDecoder();

	uint32 getNextFrameStartTime() const;

protected:
	void startVideo();
	void processData();

private:
	Common::ScopedPtr<Common::SeekableReadStream> _vx;

	/** Load an Actimagine file. */
	void load();
};

} // End of namespace Video

#endif // VIDEO_ACTIMAGINE_H

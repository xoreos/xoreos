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
 *  A video player.
 */

#include <boost/scope_exit.hpp>

#include "src/common/util.h"
#include "src/common/error.h"
#include "src/common/ustring.h"
#include "src/common/readstream.h"
#include "src/common/debug.h"

#include "src/video/decoder.h"
#include "src/video/actimagine.h"
#include "src/video/bink.h"
#include "src/video/matroska.h"
#include "src/video/quicktime.h"
#include "src/video/xmv.h"

#include "src/video/aurora/videoplayer.h"

#include "src/events/events.h"
#include "src/events/requests.h"

#include "src/aurora/resman.h"

namespace Video {

namespace Aurora {

VideoPlayer::VideoPlayer(const Common::UString &video) {
	load(video);
}

VideoPlayer::~VideoPlayer() {
}

void VideoPlayer::load(const Common::UString &name) {
	::Aurora::FileType type;

	std::unique_ptr<Common::SeekableReadStream>
		video(ResMan.getResource(::Aurora::kResourceVideo, name, &type));
	if (!video)
		throw Common::Exception("No such video resource \"%s\"", name.c_str());

	// Loading the different video formats
	switch (type) {
		case ::Aurora::kFileTypeBIK:
			_video = std::make_unique<Bink>(video.release());
			break;
		case ::Aurora::kFileTypeMOV:
			_video = std::make_unique<QuickTimeDecoder>(video.release());
			break;
		case ::Aurora::kFileTypeXMV:
			_video = std::make_unique<XboxMediaVideo>(video.release());
			break;
		case ::Aurora::kFileTypeVX:
			_video = std::make_unique<ActimagineDecoder>(video.release());
			break;
		case ::Aurora::kFileTypeWBM:
			_video = std::make_unique<Matroska>(video.release());
			break;
		default:
			break;
	}

	if (!_video)
		throw Common::Exception("Unsupported video resource type %d", (int) type);

	_video->setScale(VideoDecoder::kScaleUpDown);
}

void VideoPlayer::play() {
	RequestMan.sync();

	_video->start();

	BOOST_SCOPE_EXIT( (&_video) ) {
		_video->abort();
	} BOOST_SCOPE_EXIT_END

	uint32_t width = _video->getWidth();
	uint32_t height = _video->getHeight();

	debugC(Common::kDebugVideo, 1, "Starting video (%ux%u)", width, height);

	bool brk = false;

	Events::Event event;
	while (!EventMan.quitRequested()) {

		while (EventMan.pollEvent(event)) {
			if ((event.type == Events::kEventKeyDown && event.key.keysym.sym == SDLK_ESCAPE) ||
			    (event.type == Events::kEventMouseUp))
				brk = true;
		}

		if (brk || !_video->isPlaying())
			break;

		EventMan.delay(10);
	}

	if (EventMan.quitRequested() || brk)
		debugC(Common::kDebugVideo, 1, "Aborting video");
	else
		debugC(Common::kDebugVideo, 1, "Ending video");
}

} // End of namespace Aurora

} // End of namespace Video
